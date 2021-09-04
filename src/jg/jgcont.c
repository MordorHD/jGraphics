#include "jgcomps.h"
#include "jgbase.h"

//////////////////////////////////
//          CONTAINER           //
//////////////////////////////////

static void JGContainerPainter(JGCOMPONENT comp, JGGRAPHICS g)
{
    if(!(comp->state & JGCOMP_STATE_NOTDRAWBG))
        JGFillRect(g, &comp->rect);
}

JGCOMPONENT JGCreateContainer(void)
{
    JGCOMPONENT comp = JGCreateComponent(JGCOMP_TYPE_CONTAINER, JGCOMP_STATE_FORWARD | JGCOMP_STATE_LAYOUT, NULL, JGContainerPainter);
    comp->container.children = NULL;
    comp->container.childCnt = 0;
    comp->container.layout.type = 0;
    return comp;
}

// returns new location of added component
void JGAddChild0(JGCONTAINER *cont, JGCOMPONENT comp)
{
    int cnt = cont->childCnt++;
    cont->children = realloc(cont->children, (cnt + 1) * sizeof(JGCOMPONENT));
    comp->parent = cont->children;
    *(cont->children + cnt) = comp;
}

bool JGRemoveChild0(JGCONTAINER *cont, int index)
{
    if(cont->childCnt == 0)
        return 0;
    int cnt = --cont->childCnt - index;
    while(cnt--) *(cont->children + cnt) = *(cont->children + cnt + 1);
    return 1;
}

int JGIndexOfChild0(JGCONTAINER *cont, JGCOMPONENT comp)
{
    int cnt = cont->childCnt;
    JGCOMPONENT *chn = cont->children;
    while(cnt--)
    {
        if(*chn == comp)
            return cont->childCnt - cnt - 1;
        chn++;
    }
    return -1;
}
