#include "jgcomps.h"
#include "jgbase.h"

//////////////////////////////////
//          BUTTON              //
//////////////////////////////////

static void JGButtonPainter(JGCOMPONENT comp, JGGRAPHICS g)
{
    if(!(comp->state & JGCOMP_STATE_NOTDRAWBG))
    {
        color_t old = comp->state & JGCOMP_STATE_PRESSED ?
                          (comp->state & JGCOMP_STATE_MOUSEIN ? JGSetFillColor(g, g->palette->bgB1Color) :
                                                           JGSetFillColor(g, g->palette->bgB0Color)) :
                      comp->state & JGCOMP_STATE_MOUSEIN ? JGSetFillColor(g, g->palette->fgColor) :
                                                      JGSetFillColor(g, g->palette->bgB0Color);
        JGFillRect(g, &comp->rect);
        JGSetFillColor(g, old);
    }
    // suspended until further notice
    /*JGTEXT text = comp->text;
    HFONT font = CreateFont(comp->height, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    HFONT oldFont = SelectFont(g->dc, font);
    JGDrawCenteredText(g, text->text, text->len, comp->x, comp->y, comp->width, comp->height);
    SelectObject(g->dc, oldFont);*/
}

JGCOMPONENT JGCreateButton(string_t text)
{
    return JGCreateComponent(JGCOMP_TYPE_BUTTON, JGCreateText(text), JGCOMP_STATE_MIEVENT | JGCOMP_STATE_MOEVENT | JGCOMP_STATE_PSEVENT | JGCOMP_STATE_TGEVENT, NULL, JGButtonPainter);
}
