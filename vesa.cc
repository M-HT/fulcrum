#include "vesa.h"

static SDL_Surface *curscreen;
static unsigned int curmemsize;

static int was_keypress = 0;

extern "C"
{

int setmode(tvesa &vesa)
{
    bool keepscreen = false;
    SDL_Surface *screen = SDL_GetVideoSurface();
    if (screen != NULL)
    {
        if ((screen->w != vesa.xres) || (screen->h != vesa.yres) || (screen->format->BitsPerPixel != vesa.bpplist[0]))
        {
            screen = NULL;
        }
        else
        {
            keepscreen = true;
        }
    }

    if (screen == NULL)
    {

        Uint32 flags;

#ifdef PANDORA
        flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN | SDL_NOFRAME;
#else
        flags = SDL_SWSURFACE;
#endif

        screen = SDL_SetVideoMode (vesa.xres, vesa.yres, vesa.bpplist[0], flags);
    }

    if (screen == NULL)
    {
        return 0;
    }

    curscreen = vesa.screen = screen;

    vesa.xbytes = screen->pitch;
    curmemsize = vesa.memsize = vesa.xbytes * vesa.yres;

    vesa.redpos = screen->format->Rshift;
    vesa.greenpos = screen->format->Gshift;
    vesa.bluepos = screen->format->Bshift;

    if (!keepscreen)
    {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_WM_SetCaption ("Fulcrum", NULL);

        clearlinbuf();
    }

    return 1;
}

int clearlinbuf(void)
{
    if (curscreen == NULL) return 0;

    SDL_LockSurface(curscreen);

    memset(curscreen->pixels, 0, curmemsize);

    SDL_UnlockSurface(curscreen);
    SDL_Flip(curscreen);

    return 1;
}

void textmode(void)
{
    clearlinbuf();
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

