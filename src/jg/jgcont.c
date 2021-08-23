#include "jgcomps.h"
#include "jgbase.h"

//////////////////////////////////
//          CONTAINER           //
//////////////////////////////////

static int JGContainerListener(JGCOMPONENT comp, const JGEVENT *event)
{
    return 0;
}

static void JGContainerPainter(JGCOMPONENT comp, JGGRAPHICS g)
{
    JGFillRect(g, comp->x, comp->y, comp->width, comp->height);
}

JGCOMPONENT JGCreateContainer(void)
{
    JGCONTAINER cont = malloc(sizeof(JGCONTAINER__));
    cont->children = NULL;
    cont->childCnt = 0;
    return JGCreateComponent(JGCOMPT_CONTAINER, cont, JGCOMPS_FORWARD, JGContainerListener, JGContainerPainter);
}

// returns new location of added component
JGCOMPONENT JGAddChild0(JGCONTAINER cont, JGCOMPONENT comp)
{
    int cnt = cont->childCnt++;
    cont->children = realloc(cont->children, (cnt + 1) * sizeof(JGCOMPONENT__));
    comp->parent = cont;
    *(cont->children + cnt) = *comp;
    return cont->children + cnt;
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
    int index = (comp - cont->children) / sizeof(JGCOMPONENT__);
    return index < 0 || index >= cont->childCnt ? -1 : index;
}
