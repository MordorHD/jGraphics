#include "jgbase.h"

// implementations from "jgbase.h"
color_t JGBrighterColor(color_t color, float factor)
{
    char r = (char) (factor * ((color & 0xFF0000) >> 16));
    char g = (char) (factor * ((color & 0x00FF00) >> 8));
    char b = (char) (factor *  (color & 0x0000FF));
    return (r << 16) | (g << 8) | b;
}

bool JGGetStockPalette(JGCOLORPALETTE *pal, int type)
{
    switch(type)
    {
    case JGPALETTE_STYLE_SKY:
        pal->bgC0Color = JGRGB(135, 206, 235);
        pal->bgB0Color = JGRGB(235, 166, 135);
        pal->bgB1Color = JGRGB(235, 217, 135);
        pal->fgColor   = JGRGB(210, 120,  78);
        pal->txColor   = JGRGB(200, 135, 235);
        break;
    default:
        return 0;
    }
    return 1;
}

bool JGDestroyColorPalette(JGCOLORPALETTE *palette)
{
    if(palette == NULL)
        return 0;
    free(palette);
    return 1;
}
