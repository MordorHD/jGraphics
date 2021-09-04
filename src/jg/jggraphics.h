#ifndef __JGGRAPHICS_H__
#define __JGGRAPHICS_H__

#include "jggeom.h"

// forward declaration to prevent weird nesting
struct ApplicationTag;
struct ColorPaletteTag;

#include "typedefs.h"

typedef struct ColorPaletteTag {
    color_t bgC0Color;
    color_t bgB0Color;
    color_t bgB1Color;
    color_t fgColor;
    color_t txColor;
} JGCOLORPALETTE;

#define JGPALETTE_STYLE_SKY 0x1

/**
 * Creates a color palette with given style (prefix JGCCP_).
 *
 * @param int - Style
 * @return Allocated JGCOLORPALETTE instance
 **/
bool JGGetStockPalette(JGCOLORPALETTE*, int);

/**
 * Brightens or darkens a given color.
 *
 * @param color_t - Color to brighten
 * @param float   - Brightening factor (color gets darker if this is less than 1)
 * @return Brightened color
 **/
color_t JGBrighterColor(color_t, float);

typedef struct ImageTag {
    color_t *pixels;
    unit_t width;
    unit_t height;
} JGIMAGE;

typedef struct GraphicsTag {
    JGCOLORPALETTE palette;
    unitf_t tm[7];
    JGIMAGE image;
    color_t fill;
    color_t stroke;
} JGGRAPHICS__, *JGGRAPHICS;

/**
 * Creates a JGGRAPHICS instance.
 *
 * @param struct ApplicationTag* - JGAPPLICATION, cannot be null
 * @return An allocated JGGRAPHICS instance
 **/
JGGRAPHICS JGCreateGraphics(JGIMAGE*, JGCOLORPALETTE*);

/**
 * Frees all resources associated with the given JGTEXT instance.
 *
 * @param struct ApplicationTag* - JGAPPLICATION
 * @param JGGRAPHICS             - JGGRAPHICS
 * @return If the parameter JGGRAPHICS was null
 **/
bool JGDestroyGraphics(JGGRAPHICS);

void JGCopyImage(JGIMAGE*, unit_t, unit_t, unit_t, unit_t, CLP(JGIMAGE), unit_t, unit_t);

void JGFillRect(JGGRAPHICS, CLP(JGRECT));
void JGDrawRect(JGGRAPHICS, unit_t, unit_t, unit_t, unit_t);

void JGFillEllipse(JGGRAPHICS, unit_t, unit_t, unit_t, unit_t);
void JGDrawEllipse(JGGRAPHICS, unit_t, unit_t, unit_t, unit_t);

void JGFillCircle(JGGRAPHICS, unit_t, unit_t, unit_t);
void JGDrawCircle(JGGRAPHICS, unit_t, unit_t, unit_t);

void JGDrawLine(JGGRAPHICS, JGLINE);
void JGDrawLine_(JGGRAPHICS, unit_t, unit_t, unit_t, unit_t);

#define JGSetFillColor(g, color) ((g)->fill=color)
#define JGSetStrokeColor(g, color) ((g)->stroke=color)

#endif // __JGGRAPHICS_H__
