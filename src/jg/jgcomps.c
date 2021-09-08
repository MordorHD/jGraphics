#include "jgcomps.h"
#include "jgbase.h"
#include "jggeom.h"

//////////////////////////////////
//          COMPONENT           //
//////////////////////////////////

JGRECT JGNULLRECT = { .x = 0, .y = 0, .width = 0, .height = 0 };

JGCOMPONENT JGCreateComponent(int type, int state, JGLISTENER listener, JGPAINTER painter)
{
    JGCOMPONENT comp = malloc(sizeof(JGCOMPONENT__));
    comp->rect = JGNULLRECT;
    comp->type = type;
    comp->state = state | JGCOMP_STATE_REDRAW;
    comp->parent = NULL;
    if(listener != NULL)
    {
        comp->listener = malloc(sizeof(JGLISTENER));
        *(comp->listener) = listener;
        comp->listenerCnt = 1;
    }
    else
    {
        comp->listener = NULL;
        comp->listenerCnt = 0;
    }
    comp->painter = painter;
    return comp;
}

// destroying a component, freeing it from memory, includes all children
bool JGDestroyComponent(JGCOMPONENT comp)
{
    if(comp == NULL)
        return 0;
    switch(comp->type)
    {
    case JGCOMP_TYPE_CONTAINER:
    {
        JGCONTAINER cont = comp->container;
        int cnt = cont.childCnt;
        JGCOMPONENT *chn = cont.children;
        while(cnt--)
        {
            JGDestroyComponent(*chn);
            chn++;
        }
        free(cont.children);
        break;
    }
    case JGCOMP_TYPE_BUTTON:
    case JGCOMP_TYPE_LABEL:
    {
        free(comp->text.text);
        break;
    }
    }

    JGCOMPONENT *parent;
    if((parent = comp->parent) != NULL)
    {
        /*int index = JGIndexOfChild0(parent, comp);
        if(index >= 0)
            JGRemoveChild0(parent, index);*/
    }
    free(comp->listener);
    free(comp);
    return 1;
}

// handling events
short JGDispatchEvent(JGCOMPONENT comp, CLP(JGEVENT) event)
{
    int extraDispatch = 0;
    switch(event->id)
    {
    case JGEVENT_ID_MOUSEPRESSED:
        if(comp->state & JGCOMP_STATE_MOUSEIN)
        {
            comp->state |= JGCOMP_STATE_PRESSED;
            if(comp->state & JGCOMP_STATE_PSEVENT)
                comp->state |= JGCOMP_STATE_REDRAW;
        }
        break;
    case JGEVENT_ID_MOUSERELEASED:
        if((comp->state & JGCOMP_STATE_MOUSEIN) && (comp->state & JGCOMP_STATE_PRESSED))
        {
            comp->state ^= JGCOMP_STATE_TOGGLED;
            if(comp->state & JGCOMP_STATE_TGEVENT)
                comp->state |= JGCOMP_STATE_REDRAW;
            extraDispatch = JGEVENT_ID_TOGGLE;
        }
        comp->state &= ~JGCOMP_STATE_PRESSED;
        break;
    case JGEVENT_ID_MOUSEMOVED:
        if(!(comp->state & JGCOMP_STATE_MOUSEIN))
        {
            if(JGRectContains(&comp->rect, &event->mousePos))
            {
                comp->state |= JGCOMP_STATE_MOUSEIN;
                extraDispatch = JGEVENT_ID_HOVER;
                if(comp->state & JGCOMP_STATE_MIEVENT)
                    comp->state |= JGCOMP_STATE_REDRAW;
            }
        }
        else if(!JGRectContains(&comp->rect, &event->mousePos))
        {
            comp->state &= ~JGCOMP_STATE_MOUSEIN;
            extraDispatch = JGEVENT_ID_LEAVE;
            if(comp->state & JGCOMP_STATE_MOEVENT)
                comp->state |= JGCOMP_STATE_REDRAW;
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
        leEvent.id = extraDispatch;
        leEvent.mousePos = event->mousePos;
        leEvent.mouseButton = event->mouseButton;
        leEvent.pressedButton = event->pressedButton;
        JGDispatchEvent(comp, &leEvent);
    }
    return comp->state;
}

// and forwarding them to children
short JGDispatchEventAndForward(JGCOMPONENT comp, CLP(JGEVENT) event)
{
    short st = JGDispatchEvent(comp, event);
    if(comp->state & JGCOMP_STATE_FORWARD)
    {
        int cnt = comp->container.childCnt;
        JGCOMPONENT *chn = comp->container.children;
        while(cnt--)
        {
            st |= JGDispatchEventAndForward(*chn, event);
            chn++;
        }
    }
    return st;
}

void JGRedrawComponent(JGCOMPONENT comp, JGGRAPHICS g, int flags)
{
    if(comp->painter != NULL && ((comp->state & JGCOMP_STATE_REDRAW) || (flags & JGRC_FORCE)))
    {
        comp->painter(comp, g);
        comp->state &= ~JGCOMP_STATE_REDRAW;
    }
    if(comp->state & JGCOMP_STATE_FORWARD)
    {
        int cnt = comp->container.childCnt;
        JGCOMPONENT *chn = comp->container.children;
        while(cnt--)
        {
            JGRedrawComponent(*chn, g, flags);
            chn++;
        }
    }
}

void JGAddListener(JGCOMPONENT comp, JGLISTENER listener)
{
    comp->listener = realloc(comp->listener, sizeof(JGLISTENER) * (comp->listenerCnt + 1));
    *(comp->listener + comp->listenerCnt) = listener;
    comp->listenerCnt++;
}

bool JGSetBounds(JGCOMPONENT comp, JGRECT rect)
{
    bool bp = JGSetPos(comp, rect.point);
    bool bs = JGSetSize(comp, rect.size);
    return bp || bs;
}

bool JGSetPos(JGCOMPONENT comp, JGPOINT pos)
{
    unit_t dx = pos.x - comp->rect.x;
    unit_t dy = pos.y - comp->rect.y;
    if(dx || dy)
    {
        // update children position
        if(comp->state & JGCOMP_STATE_LAYOUT)
        {
            JGCONTAINER cont = comp->container;
            int cnt = cont.childCnt;
            JGCOMPONENT *chn = cont.children;
            JGCOMPONENT dchn;
            while(cnt--)
            {
                dchn = *chn;
                JGPOINT p = { dchn->x + dx, dchn->y + dy };
                JGSetPos(dchn, p);
                chn++;
            }
        }
        JGEVENT event = { .id = JGEVENT_ID_POS,
                          .oldPosX = comp->x,
                          .oldPosY = comp->y,
                          .posX = pos.x,
                          .posY = pos.y };
        comp->rect.point = pos;
        comp->state |= JGCOMP_STATE_REDRAW;
        JGDispatchEvent(comp, &event);
        return 1;
    }
    return 0;
}

bool JGSetSize(JGCOMPONENT comp, JGSIZE size)
{
    unit_t dw = size.width - comp->rect.width;
    unit_t dh = size.height - comp->rect.height;
    if(dw || dh)
    {
        // update layout
        if(comp->state & JGCOMP_STATE_LAYOUT)
        {
            JGCONTAINER cont = comp->container;
            JGLAYOUT layout;
            if((layout = cont.layout).type != 0)
                layout.layoutFunc(layout, cont.children, cont.childCnt, comp->x, comp->y, size.width, size.height);
        }
        JGEVENT event = { .id = JGEVENT_ID_SIZE,
                          .oldSizeX = comp->width,
                          .oldSizeY = comp->height,
                          .sizeX = size.width,
                          .sizeY = size.height };
        comp->rect.size = size;
        comp->state |= JGCOMP_STATE_REDRAW;
        JGDispatchEvent(comp, &event);
        return 1;
    }
    return 0;
}
