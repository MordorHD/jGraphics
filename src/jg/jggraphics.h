#ifndef __JGGRAPHICS_H__
#define __JGGRAPHICS_H__

// forward declaration to prevent weird nesting
struct ApplicationTag;
struct ColorPaletteTag;

#ifdef __WIN32 // Windows

    #include <windows.h>
    #include "typedefs.h"

    typedef struct GraphicsTag {
        HDC dc;
        struct ColorPaletteTag *palette;
        int transformMatrix[7];
    } JGGRAPHICS__, *JGGRAPHICS;

    /**
     * Creates a JGGRAPHICS instance.
     *
     * @param struct ApplicationTag* - JGAPPLICATION, cannot be null
     * @return An allocated JGGRAPHICS instance
     **/
    JGGRAPHICS JGCreateGraphics(struct ApplicationTag*);

    /**
     * Frees all resources associated with the given JGTEXT instance.
     *
     * @param struct ApplicationTag* - JGAPPLICATION, cannot be null
     * @param JGGRAPHICS             - JGGRAPHICS, can be null
     * @return If the parameter was null
     **/
    bool JGDestroyGraphics(struct ApplicationTag*, JGGRAPHICS);

    // testing
    #define JGSetFillColor(g, color) SetDCBrushColor((g)->dc, (COLORREF) (color))
    #define JGSetStrokeColor(g, color) SetDCPenColor((g)->dc, (COLORREF) (color))
    #define JGFillRect(g, x, y, w, h) { RECT _R_ = { x, y, x + w, y + h}; FillRect((g)->dc, &_R_, GetStockObject(DC_BRUSH)); }
    #define JGSetDrawTextColor(g, color) SetTextColor((g)->dc, (COLORREF) (color))
    #define JGDrawText(g, text, len, x, y) { RECT _R_ = { x, y, 0, 0 }; DrawText((g)->dc, text, len, &_R_, DT_NOPREFIX | DT_NOCLIP | DT_LEFT | DT_TOP | DT_SINGLELINE); }
    #define JGDrawCenteredText(g, text, len, x, y, w, h) { RECT _R_ = { x, y, (x) + (w), (y) + (h) }; DrawText((g)->dc, text, len, &_R_, DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE); }

#elif defined(__unix__) // Unix systems

#elif // Mac OS

#endif

#endif // __JGGRAPHICS_H__
