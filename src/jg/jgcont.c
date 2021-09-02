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
    JGCONTAINER cont = malloc(sizeof(JGCONTAINER__));
    cont->children = NULL;
    cont->childCnt = 0;
    cont->layout = NULL;
    return JGCreateComponent(JGCOMP_TYPE_CONTAINER, cont, JGCOMP_STATE_FORWARD | JGCOMP_STATE_LAYOUT, NULL, JGContainerPainter);
}

// returns new location of added component
void JGAddChild0(JGCONTAINER cont, JGCOMPONENT comp)
{
    int cnt = cont->childCnt++;
    cont->children = realloc(cont->children, (cnt + 1) * sizeof(JGCOMPONENT));
    comp->parent = cont;
    *(cont->children + cnt) = comp;
}

bool JGRemoveChild0(JGCONTAINER cont, int index)
{
    if(cont->childCnt == 0)
        return 0;
    int cnt = --cont->childCnt - index;
    while(cnt--) *(cont->children + cnt) = *(cont->children + cnt + 1);
    return 1;
}

int JGIndexOfChild0(JGCONTAINER cont, JGCOMPONENT comp)
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
