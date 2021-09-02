#include "jglayout.h"
#include "jgbase.h"

//////////////////////////////////
//          FLOWLAYOUT          //
//////////////////////////////////

void JGFlowLayoutFunc(JGCOMPONENT comp)
{
    JGCONTAINER cont = comp->container;
    int cnt = cont->childCnt;
    int cw = comp->width / cnt;
    int rx = comp->x;
    JGCOMPONENT chn = cont->children;
    while(cnt--)
    {
        chn->x = rx;
        chn->y = comp->y;
        chn->width = cw;
        chn->height = comp->height;
        rx += cw;
        chn++;
    }
}

JGFLOWLAYOUT JGCreateFlowLayout(void)
{
    return JGCreateLayout(JGLAYOUTT_FLOW, layout->layoutFunc);
}

bool JGDestroyFlowLayout(JGFLOWLAYOUT);
