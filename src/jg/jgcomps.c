#include "jgcomps.h"
#include "jgbase.h"

//////////////////////////////////
//          COMPONENT           //
//////////////////////////////////

JGCOMPONENT JGCreateComponent(int type, void *base, int state, JGLISTENER listener, JGPAINTER painter)
{
    JGCOMPONENT comp = malloc(sizeof(JGCOMPONENT__));
    comp->type = type;
    comp->state = state | JGCOMPS_REDRAW;
    comp->parent = NULL;
    comp->listener = malloc(sizeof(JGLISTENER));
    *(comp->listener) = listener;
    comp->listenerCnt = 1;
    comp->painter = painter;
    // test values
    comp->x = 10;
    comp->y = 10;
    comp->width = 50;
    comp->height = 50;
    comp->container = base;
    return comp;
}

// destroying specific component types
static inline void JGDestroyComponentInfo(void *info, int type)
{
    switch(type)
    {
    case JGCOMPT_CONTAINER:
    {
        JGCONTAINER cont = (JGCONTAINER) info;
        int cnt = cont->childCnt;
        JGCOMPONENT chn = cont->children;
        while(cnt--)
        {
            JGDestroyComponent(chn);
            chn++;
        }
        free(cont->children);
        break;
    }
    case JGCOMPT_BUTTON:
    {
        JGBUTTON button = (JGBUTTON) info;
        JGDestroyText(button->text);
        break;
    }
    }
    free(info);
}

// destroying a component, freeing it from memory, includes all children
bool JGDestroyComponent(JGCOMPONENT comp)
{
    if(comp == NULL)
        return 0;
    JGDestroyComponentInfo(comp->container, comp->type);
    if(comp->parent != NULL)
    {
        int index = JGIndexOfChild0(comp->parent, comp);
        if(index >= 0)
            JGRemoveChild0(comp->parent, index);
    }
    free(comp->listener);
    free(comp);
    return 1;
}

// handling events
short JGDispatchEvent(JGCOMPONENT comp, const JGEVENT *event)
{
    int extraDispatch = 0;
    switch(event->type)
    {
    case JGEVENT_ID_MOUSEPRESSED:
        if(comp->state & JGCOMPS_MOUSEIN)
        {
            comp->state |= JGCOMPS_PRESSED;
            if(comp->state & JGCOMPS_PSEVENT)
                comp->state |= JGCOMPS_REDRAW;
        }
        break;
    case JGEVENT_ID_MOUSERELEASED:
        if((comp->state & JGCOMPS_MOUSEIN) && (comp->state & JGCOMPS_PRESSED))
        {
            comp->state &= ~JGCOMPS_PRESSED;
            comp->state ^= JGCOMPS_TOGGLED;
            if(comp->state & JGCOMPS_TGEVENT)
                comp->state |= JGCOMPS_REDRAW;
            extraDispatch = JGEVENT_ID_TOGGLE;
        }
        break;
    case JGEVENT_ID_MOUSEMOVED:
        if(!(comp->state & JGCOMPS_MOUSEIN))
        {
            if(JGRectContains(comp, event->x, event->y))
            {
                comp->state |= JGCOMPS_MOUSEIN;
                extraDispatch = JGEVENT_ID_HOVER;
                if(comp->state & JGCOMPS_MIEVENT)
                    comp->state |= JGCOMPS_REDRAW;
            }
        }
        else if(!JGRectContains(comp, event->x, event->y))
        {
            comp->state &= ~JGCOMPS_MOUSEIN;
            extraDispatch = JGEVENT_ID_LEAVE;
            if(comp->state & JGCOMPS_MOEVENT)
                comp->state |= JGCOMPS_REDRAW;
        }
        break;
    }
    int cnt = comp->listenerCnt;
    JGLISTENER *lsn = comp->listener;
    while(cnt--)
    {
        (*lsn)(comp, event);
        lsn++;
    }
    if(extraDispatch != 0)
    {
        JGEVENT leEvent;
        leEvent.type = extraDispatch;
        leEvent.x = event->x;
        leEvent.y = event->y;
        leEvent.button = event->button;
        leEvent.pressedButton = event->pressedButton;
        JGDispatchEvent(comp, &leEvent);
    }
    return comp->state;
}

// and forwarding them to children
short JGDispatchEventAndForward(JGCOMPONENT comp, const JGEVENT *event)
{
    short st = JGDispatchEvent(comp, event);
    if(comp->state & JGCOMPS_FORWARD)
    {
        int cnt = comp->container->childCnt;
        JGCOMPONENT chn = comp->container->children;
        while(cnt--)
        {
            st |= JGDispatchEventAndForward(chn, event);
            chn++;
        }
    }
    return st;
}

void JGRedrawComponent(JGCOMPONENT comp, JGGRAPHICS g, int flags)
{
    if(comp->state & JGCOMPS_REDRAW || (flags & JGRC_FORCE))
    {
        comp->painter(comp, g);
        comp->state &= ~JGCOMPS_REDRAW;
    }
    if(comp->state & JGCOMPS_FORWARD)
    {
        int cnt = comp->container->childCnt;
        JGCOMPONENT chn = comp->container->children;
        while(cnt--)
        {
            JGRedrawComponent(chn, g, flags);
            chn++;
        }
    }
}

bool JGRectContains(JGCOMPONENT comp, int x, int y)
{
    return x > comp->x && x < comp->x + comp->width
        && y > comp->y && y < comp->y + comp->height;
}

bool JGRectInstersects(JGCOMPONENT comp, int x, int y, int w, int h)
{
    return comp->x < x + w && comp->x + comp->width > x
        && comp->y < y + h && comp->y + comp->height > y;
}

void JGSetBounds(JGCOMPONENT comp, int x, int y, int w, int h)
{
    comp->x = x;
    comp->y = y;
    comp->width = w;
    comp->height = h;
}

void JGSetPos(JGCOMPONENT comp, int x, int y)
{
    comp->x = x;
    comp->y = y;
}

void JGSetSize(JGCOMPONENT comp, int w, int h)
{
    comp->width = w;
    comp->height = h;
}
