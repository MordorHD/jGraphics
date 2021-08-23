#include "jgcomps.h"

//////////////////////////////////
//          BUTTON              //
//////////////////////////////////

#include "jgbase.h"

static int JGButtonListener(JGCOMPONENT comp, const JGEVENT *event)
{
    return 0;
}

static void JGButtonPainter(JGCOMPONENT comp, JGGRAPHICS g)
{
    color_t old = comp->state & JGCOMPS_MOUSEIN ? JGSetFillColor(g, g->palette->fgColor) :
                  comp->state & JGCOMPS_PRESSED ? JGSetFillColor(g, g->palette->bgB1Color) :
                                                  JGSetFillColor(g, g->palette->bgB0Color);
    JGFillRect(g, comp->x, comp->y, comp->width, comp->height);
    JGTEXT text = comp->button->text;
    JGDrawCenteredText(g, text->text, text->len, comp->x, comp->y, comp->width, comp->height);
    JGSetFillColor(g, old);
}

JGCOMPONENT JGCreateButton(void)
{
    JGBUTTON button = malloc(sizeof(JGBUTTON__));
    button->text = JGCreateText(NULL);
    return JGCreateComponent(JGCOMPT_BUTTON, button, JGCOMPS_MIEVENT | JGCOMPS_MOEVENT | JGCOMPS_PSEVENT | JGCOMPS_TGEVENT, JGButtonListener, JGButtonPainter);
}
