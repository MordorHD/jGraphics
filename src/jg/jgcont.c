#include "jgcomps.h"
#include "jgbase.h"

//////////////////////////////////
//          CONTAINER           //
//////////////////////////////////

static void JGContainerPainter(JGCOMPONENT cont, JGGRAPHICS g)
{
    if(!(cont->state & JGCOMP_STATE_NOTDRAWBG))
        JGFillRect(g, &cont->rect);
}

JGCOMPONENT JGCreateContainer(void)
{
    JGCOMPONENT cont = JGCreateComponent(JGCOMP_TYPE_CONTAINER, JGCOMP_STATE_FORWARD | JGCOMP_STATE_LAYOUT, NULL, JGContainerPainter);
    cont->container.children = NULL;
    cont->container.childCnt = 0;
    cont->container.layout.type = 0;
    return cont;
}

void JGAddChild(JGCOMPONENT cont, JGCOMPONENT comp)
{
    int cnt = cont->container.childCnt++;
    cont->container.children = realloc(cont->container.children, (cnt + 1) * sizeof(JGCOMPONENT));
    cont->parent = cont->container.children;
    *(cont->container.children + cnt) = comp;
}

void JGAddChildren(JGCOMPONENT cont, int addCnt, ...)
{
    if(addCnt <= 0)
        return;
    va_list l;
    va_start(l, addCnt);

    int cnt = cont->container.childCnt;
    int newCnt = cnt + addCnt;
    cont->container.children = realloc(cont->container.children, newCnt * sizeof(JGCOMPONENT));

    JGCOMPONENT *chn = cont->container.children + cnt;
    JGCOMPONENT comp;
    while(addCnt--)
    {
        comp = va_arg(l, JGCOMPONENT);
        cont->parent = cont->container.children;
        *(chn++) = comp;
    }
    va_end(l);

    cont->container.childCnt = newCnt;
}

bool JGRemoveChild(JGCOMPONENT cont, int index)
{
    if(cont->container.childCnt == 0)
        return 0;
    int cnt = --cont->container.childCnt - index;
    while(cnt--) *(cont->container.children + cnt) = *(cont->container.children + cnt + 1);
    return 1;
}

int JGIndexOfChild(JGCOMPONENT cont, JGCOMPONENT comp)
{
    int cnt = cont->container.childCnt;
    JGCOMPONENT *chn = cont->container.children;
    while(cnt--)
    {
        if(*chn == comp)
            return cont->container.childCnt - cnt - 1;
        chn++;
    }
    return -1;
}
