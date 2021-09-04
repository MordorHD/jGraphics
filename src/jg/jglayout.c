#include "jglayout.h"
#include "jgbase.h"

static void JGFlowLayoutFunc(JGLAYOUT layout, JGCOMPONENT *chn, int cnt, unit_t x, unit_t y, unit_t w, unit_t h)
{
    if(cnt == 0)
        return;
    unit_t cw = w / cnt;
    JGCOMPONENT dchn;
    JGRECT rect = {
        .x = x,
        .y = y,
        .width = cw,
        .height = h
    };
    while(cnt--)
    {
        dchn = *chn;
        if(!(dchn->state & JGCOMP_STATE_FIXEDSIZE))
            JGSetBounds(dchn, rect);
        else
        {
            JGPOINT p = { .x = rect.x + ((cw - dchn->width) >> 1),
                          .y =      y + ((h - dchn->height) >> 1) };
            JGSetPos(dchn, p);
        }
        rect.x += cw;
        chn++;
    }
}

static void JGStackLayoutFunc(JGLAYOUT layout, JGCOMPONENT *chn, int cnt, unit_t x, unit_t y, unit_t w, unit_t h)
{
    if(cnt == 0)
        return;
    unit_t ch = h / cnt;
    JGCOMPONENT dchn;
    JGRECT rect = {
        .x = x,
        .y = y,
        .width = w,
        .height = ch
    };
    while(cnt--)
    {
        dchn = *chn;
        if(!(dchn->state & JGCOMP_STATE_FIXEDSIZE))
            JGSetBounds(dchn, rect);
        else
        {
            JGPOINT pos = { .x =      x + ((w - dchn->width) >> 1),
                            .y = rect.y + ((ch - dchn->height) >> 1) };
            JGSetPos(dchn, pos);
        }
        rect.y += ch;
        chn++;
    }
}

bool JGGetStockLayout(JGLAYOUT *layout, int type)
{
    layout->type = type;
    switch(type)
    {
    case JGLAYOUT_FLOW:
        layout->layoutFunc = JGFlowLayoutFunc;
        break;
    case JGLAYOUT_STACK:
        layout->layoutFunc = JGStackLayoutFunc;
        break;
    default:
        return 0;
    }
    return 1;
}
