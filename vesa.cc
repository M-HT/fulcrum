#include "vesa.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

static int was_keypress = 0;

extern "C"
{

int setmode(tvesa &vesa)
{
    SDL_RendererInfo info;
    Uint32 format, altformat1, altformat2, altformat3;
    int w, h, bpp_index, fmt_index, bytes, bpp;
    Uint32 Rmask, Gmask, Bmask, Amask, mask;

    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }

    if (window == NULL)
    {
        Uint32 flags;

        flags = SDL_WINDOW_HIDDEN;

#if defined(PANDORA) || defined(PYRA)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif

        window = SDL_CreateWindow("Fulcrum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, vesa.xres, vesa.yres, flags);
        if (window == NULL)
        {
            return 0;
        }

        renderer = SDL_CreateRenderer(window, -1, (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) ? SDL_RENDERER_PRESENTVSYNC : 0);
        if (renderer == NULL)
        {
            SDL_DestroyWindow(window);
            window = NULL;
            return 0;
        }

        SDL_ShowWindow(window);
        SDL_ShowCursor(SDL_DISABLE);
    }
    else
    {
        if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP))
        {
            SDL_GetWindowSize(window, &w, &h);
            if (w != vesa.xres || h != vesa.yres)
            {
                SDL_SetWindowSize(window, vesa.xres, vesa.yres);
            }
        }
    }

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP)
    {
        SDL_GetWindowSize(window, &w, &h);

        if (((float)w) / vesa.xres <= ((float)h) / vesa.yres) {
          h = (w * vesa.yres) / vesa.xres;
        } else {
          w = (h * vesa.xres) / vesa.yres;
        }

        if (w < vesa.xres && h < vesa.yres)
        {
            vesa.xres = w;
            vesa.yres = h;
        }

        SDL_RenderSetLogicalSize(renderer, w, h);
    }

    SDL_GetRendererInfo(renderer, &info);

    if (info.max_texture_width < vesa.xres || info.max_texture_height < vesa.yres)
    {
        return 0;
    }

    format = altformat1 = altformat2 = altformat3 = 0;
    for (bpp_index = 0; bpp_index < 4; bpp_index++)
    {
        if (vesa.bpplist[bpp_index] == 0) continue;

        bytes = (vesa.bpplist[bpp_index] + 7) >> 3;

        for (fmt_index = 0; fmt_index < (int)info.num_texture_formats; fmt_index++)
        {
            if (SDL_BYTESPERPIXEL(info.texture_formats[fmt_index]) != (unsigned int)bytes) continue;

            if (bytes <= 2)
            {
                if (SDL_BITSPERPIXEL(info.texture_formats[fmt_index]) != (unsigned int)vesa.bpplist[bpp_index]) continue;
            }

            if (!SDL_PixelFormatEnumToMasks(info.texture_formats[fmt_index], &bpp, &Rmask, &Gmask, &Bmask, &Amask)) continue;

            if (!Rmask || !Gmask || !Bmask) continue;

            if (vesa.bpplist[bpp_index] == 32)
            {
                if (Rmask == 0x00FF0000 &&
                    Gmask == 0x0000FF00 &&
                    Bmask == 0x000000FF)
                {
                    if (Amask == 0)
                    {
                        format = info.texture_formats[fmt_index];
                        break;
                    }
                    else
                    {
                        altformat1 = info.texture_formats[fmt_index];
                    }
                }
                else
                {
                    if (Amask == 0)
                    {
                        if (altformat2 == 0) altformat2 = info.texture_formats[fmt_index];
                    }
                    else
                    {
                        if (altformat3 == 0) altformat3 = info.texture_formats[fmt_index];
                    }
                }
            }
            else
            {
                format = info.texture_formats[fmt_index];
                break;
            }
        }

        if (format != 0) break;

        if (vesa.bpplist[bpp_index] == 32)
        {
            if (altformat1 != 0)
            {
                format = altformat1;
                break;
            }
            else if (altformat2 != 0)
            {
                format = altformat2;
                break;
            }
            else if (altformat3 != 0)
            {
                format = altformat3;
                break;
            }
        }
    }

    if (format == 0)
    {
        return 0;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, vesa.xres, vesa.yres);
    if (texture == NULL)
    {
        return 0;
    }

    vesa.texture = texture;
    vesa.renderer = renderer;

    vesa.pbytes = SDL_BYTESPERPIXEL(format);
    vesa.xbytes = vesa.xres * vesa.pbytes;
    vesa.memsize = vesa.xbytes * vesa.yres;

    SDL_PixelFormatEnumToMasks(format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);

    vesa.redpos = 0;
    for (mask = Rmask; !(mask & 1); mask >>= 1) vesa.redpos++;
    vesa.greenpos = 0;
    for (mask = Gmask; !(mask & 1); mask >>= 1) vesa.greenpos++;
    vesa.bluepos = 0;
    for (mask = Bmask; !(mask & 1); mask >>= 1) vesa.bluepos++;

    vesa.redbits = 0;
    for (mask = Rmask >> vesa.redpos; mask & 1; mask >>= 1) vesa.redbits++;
    vesa.greenbits = 0;
    for (mask = Gmask >> vesa.greenpos; mask & 1; mask >>= 1) vesa.greenbits++;
    vesa.bluebits = 0;
    for (mask = Bmask >> vesa.bluepos; mask & 1; mask >>= 1) vesa.bluebits++;

    return 1;
}

int clearlinbuf(void)
{
    if (renderer == NULL) return 0;

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    return 1;
}

void textmode(void)
{
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }
}

int keypressed(void)
{
    SDL_Event event;

    if (was_keypress) return 1;

    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    was_keypress = 1;
                    return 1;
                }

                break;
            case SDL_QUIT:
                was_keypress = 1;
                return 1;
            default:
                break;
        }
    }

    return 0;
}


}

