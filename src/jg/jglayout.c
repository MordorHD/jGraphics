#include "jglayout.h"
#include "jgbase.h"

JGLAYOUT JGCreateLayout(int type, JGLAYOUTCOMPONENTS layoutFunc)
{
    JGLAYOUT layout = malloc(sizeof(JGLAYOUT__));
    layout->type = type;
    layout->layoutFunc = layoutFunc;
    return layout;
}

static void JGFlowLayoutFunc(JGLAYOUT layout, JGCOMPONENT *chn, int cnt, unit_t x, unit_t y, unit_t w, unit_t h)
{
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

JGLAYOUT JGCreateFlowLayout(void)
{
    return JGCreateLayout(JGLAYOUTT_FLOW, JGFlowLayoutFunc);
}

static void JGStackLayoutFunc(JGLAYOUT layout, JGCOMPONENT *chn, int cnt, unit_t x, unit_t y, unit_t w, unit_t h)
{
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

JGLAYOUT JGCreateStackLayout(void)
{
    return JGCreateLayout(JGLAYOUTT_STACK, JGStackLayoutFunc);
}

bool JGDestroyLayout(JGLAYOUT layout)
{
    free(layout);
    return 1;
}
