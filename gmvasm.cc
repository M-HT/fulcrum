#include <stdint.h>
#include "vesa.h"

#define ct_r16 (0*256)
#define ct_g16 (1*256)
#define ct_b16 (2*256)

#define ct_r32 (0*256)
#define ct_g32 (1*256)
#define ct_b32 (2*256)

extern "C"
{

void InitZeroMem(char *pMem, long lNum)
{
    memset(pMem, 0, lNum);
}


void TransformTo32Bit(char *pcSrc, char *cpDest, char *pcPalette, int iNumPixels)
{
    uint8_t *SrcScreenBuffer = (uint8_t *)pcSrc;
    uint8_t *DestScreenBuffer = (uint8_t *)cpDest;
    uint8_t *Palette = (uint8_t *)pcPalette;

    for (; iNumPixels != 0; DestScreenBuffer+=4,SrcScreenBuffer++,iNumPixels--)
    {
        uint32_t palindex = 3 * *SrcScreenBuffer;

        DestScreenBuffer[0] = Palette[palindex + 2];
        DestScreenBuffer[1] = Palette[palindex + 1];
        DestScreenBuffer[2] = Palette[palindex];
        DestScreenBuffer[3] = 0;
    }
}


void makecoltab16(tvesa &vesa, void *_coltab)
{
    uint16_t *coltab = (uint16_t *)_coltab;

    for (unsigned int i=0; i<256; i++)
    {
        coltab[i + ct_r16] = ((i << vesa.redbits  ) >> 8) << vesa.redpos;
        coltab[i + ct_g16] = ((i << vesa.greenbits) >> 8) << vesa.greenpos;
        coltab[i + ct_b16] = ((i << vesa.bluebits ) >> 8) << vesa.bluepos;
    }
}

void makecoltab32(tvesa &vesa, void *_coltab)
{
    uint32_t *coltab = (uint32_t *)_coltab;

    for (unsigned int i=0; i<256; i++)
    {
        coltab[i + ct_r32] = ((i << vesa.redbits  ) >> 8) << vesa.redpos;
        coltab[i + ct_g32] = ((i << vesa.greenbits) >> 8) << vesa.greenpos;
        coltab[i + ct_b32] = ((i << vesa.bluebits ) >> 8) << vesa.bluepos;
    }
}

static void copydirect32(uint32_t *src, uint32_t *dest, tvesa *pVesa)
{
    for (int iYCounter = pVesa->yres; iYCounter != 0; iYCounter--)
    {
        memcpy(dest, src, pVesa->xres * 4);
        src += pVesa->xres;
        dest = (uint32_t *) (((uintptr_t)dest) + pVesa->xbytes);
    }
}

static void copydirect24(uint32_t *src, uint8_t *dest, tvesa *pVesa)
{
    for (int iYCounter = pVesa->yres; iYCounter != 0; iYCounter--)
    {
        for (int iXCounter = pVesa->xres >> 2; iXCounter != 0; iXCounter--)
        {
            uint32_t srcdata1 = src[0];
            uint32_t srcdata2 = src[1];
            ((uint32_t *)dest)[0] = srcdata1 | (srcdata2 << 24);
            uint32_t srcdata3 = src[2];
            ((uint32_t *)dest)[1] = (srcdata2 >> 8) | (srcdata3 << 16);
            uint32_t srcdata4 = src[3];
            ((uint32_t *)dest)[2] = (srcdata3 >> 16) | (srcdata4 << 8);

            src+=4;
            dest+=12;
        }

        dest += pVesa->xbytes - 3*pVesa->xres;
    }
}

static void copybuffer32(uint32_t *src, uint32_t *dest, tvesa *pVesa, uint32_t *coltab)
{
    for (int iYCounter = pVesa->yres; iYCounter != 0; iYCounter--)
    {
        for (int iXCounter = pVesa->xres; iXCounter != 0; iXCounter--)
        {
            uint32_t srcdata = *src;
            *dest = coltab[ct_b32 + (srcdata & 0xff)] | coltab[ct_g32 + ((srcdata >> 8) & 0xff)] | coltab[ct_r32 + ((srcdata >> 16) & 0xff)];
            src++;
            dest++;
        }
    }
}

static void copybuffer24(uint32_t *src, uint8_t *dest, tvesa *pVesa, uint32_t *coltab)
{
    for (int iYCounter = pVesa->yres; iYCounter != 0; iYCounter--)
    {
        for (int iXCounter = pVesa->xres >> 2; iXCounter != 0; iXCounter--)
        {
            uint32_t srcdata1 = src[0];
            uint32_t dstdata1 = coltab[ct_b32 + (srcdata1 & 0xff)] | coltab[ct_g32 + ((srcdata1 >> 8) & 0xff)] | coltab[ct_r32 + ((srcdata1 >> 16) & 0xff)];
            uint32_t srcdata2 = src[1];
            uint32_t dstdata2 = coltab[ct_b32 + (srcdata2 & 0xff)] | coltab[ct_g32 + ((srcdata2 >> 8) & 0xff)] | coltab[ct_r32 + ((srcdata2 >> 16) & 0xff)];
            ((uint32_t *)dest)[0] = dstdata1 | (dstdata2 << 24);
            uint32_t srcdata3 = src[2];
            uint32_t dstdata3 = coltab[ct_b32 + (srcdata3 & 0xff)] | coltab[ct_g32 + ((srcdata3 >> 8) & 0xff)] | coltab[ct_r32 + ((srcdata3 >> 16) & 0xff)];
            ((uint32_t *)dest)[1] = (dstdata2 >> 8) | (dstdata3 << 16);
            uint32_t srcdata4 = src[3];
            uint32_t dstdata4 = coltab[ct_b32 + (srcdata4 & 0xff)] | coltab[ct_g32 + ((srcdata4 >> 8) & 0xff)] | coltab[ct_r32 + ((srcdata4 >> 16) & 0xff)];
            ((uint32_t *)dest)[2] = (dstdata3 >> 16) | (dstdata4 << 8);

            src+=4;
            dest+=12;
        }

        dest += pVesa->xbytes - 3*pVesa->xres;
    }
}

static void copybuffer16(uint32_t *src, uint16_t *dest, tvesa *pVesa, uint16_t *coltab)
{
    for (int iYCounter = pVesa->yres; iYCounter != 0; iYCounter--)
    {
        for (int iXCounter = pVesa->xres >> 1; iXCounter != 0; iXCounter--)
        {
            uint32_t srcdata1 = src[0];
            uint32_t dstdata1 = coltab[ct_b16 + (srcdata1 & 0xff)] | coltab[ct_g16 + ((srcdata1 >> 8) & 0xff)] | coltab[ct_r16 + ((srcdata1 >> 16) & 0xff)];
            uint32_t srcdata2 = src[1];
            uint32_t dstdata2 = coltab[ct_b16 + (srcdata2 & 0xff)] | coltab[ct_g16 + ((srcdata2 >> 8) & 0xff)] | coltab[ct_r16 + ((srcdata2 >> 16) & 0xff)];
            *((uint32_t *)dest) = dstdata1 | (dstdata2 << 16);
            src+=2;
            dest+=2;
        }
    }
}


void CopyBufferASM(char *pSrcScreenBuffer, tvesa *pVesa, void *coltab)
{
    SDL_LockSurface(pVesa->screen);

    if (coltab == NULL)
    {
        if (pVesa->pbytes > 3)
        {
            copydirect32((uint32_t *)pSrcScreenBuffer, (uint32_t *)pVesa->screen->pixels, pVesa);
        }
        else
        {
            copydirect24((uint32_t *)pSrcScreenBuffer, (uint8_t *)pVesa->screen->pixels, pVesa);
        }
    }
    else
    {
        switch (pVesa->pbytes)
        {
            case 4:
                copybuffer32((uint32_t *)pSrcScreenBuffer, (uint32_t *)pVesa->screen->pixels, pVesa, (uint32_t *)coltab);
                break;
            case 3:
                copybuffer24((uint32_t *)pSrcScreenBuffer, (uint8_t *)pVesa->screen->pixels, pVesa, (uint32_t *)coltab);
                break;
            default:
                copybuffer16((uint32_t *)pSrcScreenBuffer, (uint16_t *)pVesa->screen->pixels, pVesa, (uint16_t *)coltab);
                break;
        }
    }

    SDL_UnlockSurface(pVesa->screen);

    SDL_Flip(pVesa->screen);
}


void TransformToBumpmapASM(char *src, char *dest, int len)
{
    uint8_t *pSrc = (uint8_t *)src;
    uint8_t *pDest = (uint8_t *)dest;
    uint8_t srcofs = 0;

    for (; len != 0; srcofs++,len--)
    {
        int value = ( ((int)pSrc[(srcofs & 0xffffff00) | ((srcofs + 1) & 0xffffff00) ]) - ((int)pSrc[srcofs]) ) + 128;
        if (value < 0) value = 0;
        else if (value > 255) value = 255;

        *pDest = (uint8_t) value;
        pDest++;

        value = ( ((int)pSrc[(srcofs & 0xffff00ff) | ((srcofs + 256) & 0x0000ff00) ]) - ((int)pSrc[srcofs]) ) + 128;
        if (value < 0) value = 0;
        else if (value > 255) value = 255;

        *pDest = (uint8_t) value;
        pDest++;
    }
}

}
