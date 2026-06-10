#include "vesa.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

static int was_keypress = 0;

extern "C"
{

int setmode(tvesa &vesa)
{
    SDL_PropertiesID props;
    SDL_PixelFormat format, altformat1, altformat2, altformat3;
    int w, h, bpp_index, fmt_index, bytes, bpp;
    Uint32 Rmask, Gmask, Bmask, Amask, mask;
    const SDL_PixelFormat *texture_formats;
    bool propok, fullscreen;

    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }

#if defined(PANDORA) || defined(PYRA)
    fullscreen = true;
#else
    fullscreen = false;
#endif

    if (window == NULL)
    {
        props = SDL_CreateProperties();
        if (props == 0)
        {
            return 0;
        }

        propok = SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Fulcrum");
        propok &= SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
        propok &= SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
        propok &= SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, vesa.xres);
        propok &= SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, vesa.yres);
        propok &= SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true);
        propok &= SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, fullscreen);

        if (!propok)
        {
            SDL_DestroyProperties(props);
            return 0;
        }

        window = SDL_CreateWindowWithProperties(props);
        SDL_DestroyProperties(props);
        if (window == NULL)
        {
            return 0;
        }

        renderer = SDL_CreateRenderer(window, NULL);
        if (renderer == NULL)
        {
            SDL_DestroyWindow(window);
            window = NULL;
            return 0;
        }

        if (fullscreen)
        {
            SDL_SetRenderVSync(renderer, 1);
        }

        SDL_ShowWindow(window);
        SDL_HideCursor();
    }
    else
    {
        if (!fullscreen)
        {
            SDL_GetWindowSize(window, &w, &h);
            if (w != vesa.xres || h != vesa.yres)
            {
                SDL_SetWindowSize(window, vesa.xres, vesa.yres);
            }
        }
    }

    if (fullscreen)
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

        SDL_SetRenderLogicalPresentation(renderer, w, h, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }

    props = SDL_GetRendererProperties(renderer);
    if (props == 0)
    {
        return 0;
    }

    w = (int)SDL_GetNumberProperty(props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0);
    if (w && (w < vesa.xres || w < vesa.yres))
    {
        return 0;
    }

    format = altformat1 = altformat2 = altformat3 = SDL_PIXELFORMAT_UNKNOWN;
    for (bpp_index = 0; bpp_index < 4; bpp_index++)
    {
        if (vesa.bpplist[bpp_index] == 0) continue;

        bytes = (vesa.bpplist[bpp_index] + 7) >> 3;

        texture_formats = (const SDL_PixelFormat *) SDL_GetPointerProperty(props, SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER, NULL);

        if (texture_formats != NULL)
        for (fmt_index = 0; texture_formats[fmt_index] != SDL_PIXELFORMAT_UNKNOWN; fmt_index++)
        {
            if (SDL_BYTESPERPIXEL(texture_formats[fmt_index]) != (unsigned int)bytes) continue;

            if (bytes <= 2)
            {
                if (SDL_BITSPERPIXEL(texture_formats[fmt_index]) != (unsigned int)vesa.bpplist[bpp_index]) continue;
            }

            if (!SDL_ISPIXELFORMAT_PACKED(texture_formats[fmt_index])) continue;
            if (SDL_PIXELLAYOUT(texture_formats[fmt_index]) == SDL_PACKEDLAYOUT_2101010 || SDL_PIXELLAYOUT(texture_formats[fmt_index]) == SDL_PACKEDLAYOUT_1010102) continue;

            if (!SDL_GetMasksForPixelFormat(texture_formats[fmt_index], &bpp, &Rmask, &Gmask, &Bmask, &Amask)) continue;

            if (!Rmask || !Gmask || !Bmask) continue;

            if (vesa.bpplist[bpp_index] == 32)
            {
                if (Rmask == 0x00FF0000 &&
                    Gmask == 0x0000FF00 &&
                    Bmask == 0x000000FF)
                {
                    if (Amask == 0)
                    {
                        format = texture_formats[fmt_index];
                        break;
                    }
                    else
                    {
                        altformat1 = texture_formats[fmt_index];
                    }
                }
                else
                {
                    if (Amask == 0)
                    {
                        if (altformat2 == SDL_PIXELFORMAT_UNKNOWN) altformat2 = texture_formats[fmt_index];
                    }
                    else
                    {
                        if (altformat3 == SDL_PIXELFORMAT_UNKNOWN) altformat3 = texture_formats[fmt_index];
                    }
                }
            }
            else
            {
                format = texture_formats[fmt_index];
                break;
            }
        }

        if (format != SDL_PIXELFORMAT_UNKNOWN) break;

        if (vesa.bpplist[bpp_index] == 32)
        {
            if (altformat1 != SDL_PIXELFORMAT_UNKNOWN)
            {
                format = altformat1;
                break;
            }
            else if (altformat2 != SDL_PIXELFORMAT_UNKNOWN)
            {
                format = altformat2;
                break;
            }
            else if (altformat3 != SDL_PIXELFORMAT_UNKNOWN)
            {
                format = altformat3;
                break;
            }
        }
    }

    if (format == SDL_PIXELFORMAT_UNKNOWN)
    {
        return 0;
    }

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

    SDL_GetMasksForPixelFormat(format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);

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

    vesa.alphamask = Amask;

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
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                if (event.key.key == SDLK_ESCAPE)
                {
                    was_keypress = 1;
                    return 1;
                }

                break;
            case SDL_EVENT_QUIT:
                was_keypress = 1;
                return 1;
            default:
                break;
        }
    }

    return 0;
}


}

