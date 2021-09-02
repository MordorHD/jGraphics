#include "jgbase.h"

// implementations from "jgbase.h"
color_t JGBrighterColor(color_t color, float factor)
{
    char r = (char) (factor * ((color & 0xFF0000) >> 16));
    char g = (char) (factor * ((color & 0x00FF00) >> 8));
    char b = (char) (factor *  (color & 0x0000FF));
    return (r << 16) | (g << 8) | b;
}

JGCOLORPALETTE JGCreateColorPalette(int type)
{
    JGCOLORPALETTE palette = malloc(sizeof(JGCOLORPALETTE__));
    switch(type)
    {
    case 0:
        break;
    case JGCCP_SKY:
        palette->bgC0Color = JGRGB(135, 206, 235);
        palette->bgB0Color = JGRGB(235, 166, 135);
        palette->bgB1Color = JGRGB(235, 217, 135);
        palette->fgColor   = JGRGB(210, 120,  78);
        palette->txColor   = JGRGB(200, 135, 235);
        break;
    }
    return palette;
}

bool JGDestroyColorPalette(JGCOLORPALETTE palette)
{
    if(palette == NULL)
        return 0;
    free(palette);
    return 1;
}
