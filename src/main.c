#include <stdio.h>
#include <stdlib.h>

#define JGSETTING_UNIT_FLOAT_TYPE float
#define JGSETTING_UNIT_INT_TYPE int

#include "jg/jgbase.h"
#include "jg/jglayout.h"

int clockfunc(JGCLOCK cl, clock_t delay)
{
    printf("TICK: %ld, %ld\n", cl->runtime, delay);
    if(cl->runtime >= 1000)
        return JGCLOCKS_STOPPED;
    return JGCLOCKS_RUNNING;
}

int ButtonListener(JGCOMPONENT comp, const JGEVENT *event)
{
    switch(event->type)
    {
    case JGEVENT_ID_TOGGLE:
        printf("TOGGLED\n");
        break;
    }
    return 0;
}

int CanvasListener(JGCOMPONENT comp, const JGEVENT *event)
{
    //static int x1 = -1;
    //static int y1 = 0;
    switch(event->type)
    {
    case JGEVENT_ID_MOUSEPRESSED:
        /*if(x1 != -1)
        {
            JGGRAPHICS g = comp->canvas->buffer;
            JGSetStrokeColor(g, 0xFF00FF);
            JGDrawLine(g, event->x - comp->x, event->y - comp->y,
                          x1 - comp->x, y1 - comp->y);
            comp->state |= JGCOMP_STATE_REDRAW;
        }
        x1 = event->x;
        y1 = event->y;*/
        break;
    case JGEVENT_ID_MOUSEMOVED:
    if(event->mouseButton)
    {
        JGPOINT mp = event->mousePos;
        JGGRAPHICS g = comp->canvas.buffer;
        JGFillCircle(g, 40, 40, 30);
        if(event->mouseButton & JGEVENT_MB_LEFT)
        {
            JGSetStrokeColor(g, 0xFF00FF);
            JGDrawLine_(g, mp.x - event->dx - comp->x, mp.y - event->dy - comp->y,
                          mp.x - comp->x, mp.y - comp->y);
        }
        else if(event->mouseButton & JGEVENT_MB_RIGHT)
        {
            JGSetFillColor(g, 0x000000);
            JGRECT r;
            r.x = mp.x - comp->x - 15;
            r.y = mp.y - comp->y - 15;
            r.width = 30;
            r.height = 30;
            JGFillRect(g, &r);
        }
        comp->state |= JGCOMP_STATE_REDRAW;
    }
    break;
    }
    return 0;
}

void CanvasPainter(JGCOMPONENT comp, JGGRAPHICS g)
{
}

static int time__ = 0;

JGRECT someRect = { .x = 20, .y = -20, .width = 3000, .height = 1000 };
JGLINE someLine = { .x1 = 220, .y1 = -320, .x2 = 9800, .y2 = 2000 };
JGLINE dest;
void tests(JGGRAPHICS g)
{
    clock_t t = clock();
    //JGFillEllipse(g, 0, 0, 1919, 1079);
    //CLOCKS_PER_SEC
    //JGCopyImage(&g->image, 0, 0, &g->image);
    //JGClipLineRect(&someLine, &someRect, &dest);
    JGDrawLine(g, someLine);
    clock_t delta = clock() - t;
    time__ += delta;
}

void wintests(HDC dc)
{
    clock_t t = clock();
    RECT r = { -10, 10, 290, 3010 };
    FillRect(dc, &r, GetStockObject(WHITE_BRUSH));
    clock_t delta = clock() - t;
    time__ += delta;
}

//#define __TEST_CASE__

int main(int argc, char **argv)
{
    JGLINE line;
    line.x1 = 200;
    line.y1 = -20;
    line.x2 = 500;
    line.y2 = 50;
    JGRECT rect;
    rect.x = 10;
    rect.y = 10;
    rect.width = 200;
    rect.height = 200;
    JGClipLineRect(&line, &rect, &line);

    //TESTS
#ifdef __TEST_CASE__
    JGIMAGE image;
    image.width = 1920;
    image.height = 1080;
    image.pixels = malloc(sizeof(color_t) * 1920 * 1080);
    JGGRAPHICS g = JGCreateGraphics(&image, NULL);

    HBITMAP hbm = CreateBitmap(image.width, image.height, 1, sizeof(color_t) * 8, image.pixels);
    HDC dc = GetDC(NULL);
    HBITMAP hbmOld = SelectObject(dc, hbm);

    const int testCases = 1000000;
    int i = testCases;
    while(i--)
        tests(g);
    printf("Average: %f\n", (float) time__ / testCases);

    SelectObject(dc, hbmOld);
    ReleaseDC(NULL, dc);

    DeleteObject(hbm);

    JGDestroyGraphics(g);
    return 0;
#endif // __TEST_CASE__

    JGCLOCK c = JGCreateClock(clockfunc, 1l);
    //JGStartClock(c);
    JGDestroyClock(c);

    JGTEXT text;
    JGSetString(&text, "Hello");

    char buf[512];
    JGGetString(&text, buf, 512);
    printf("%s\n", buf);

    JGAppendString(&text, "What is going on here?");
    JGGetString(&text, buf, 512);
    printf("%s\n", buf);

    JGRemoveRangeAt(&text, 10, 8);
    JGGetString(&text, buf, 512);
    printf("%s\n", buf);

    int index = JGFindString(&text, "here", 0);
    printf("INDEX: %d, %c\n", index, *(text.text + index));

    JGReplaceString(&text, "on", 0, "off");
    JGGetString(&text, buf, 512);
    printf("%s\n", buf);

    index = JGFindChar(&text, '?', 0);
    printf("INDEX: %d, %c\n", index, *(text.text + index));

    JGInit(argc, argv);

    JGLAYOUT stacklayout;
    JGGetStockLayout(&stacklayout, JGLAYOUT_STACK);
    JGLAYOUT flowlayout;
    JGGetStockLayout(&flowlayout, JGLAYOUT_FLOW);

    JGAPPLICATION app = JGCreateApplication();
    JGSetLayout(app, stacklayout);

    JGCOMPONENT topLabel = JGCreateLabel("JG Demo");
    JGAddChild(app, topLabel);

    // bottom container //
    JGCOMPONENT bottomCont = JGCreateContainer();
    JGSetLayout(bottomCont, flowlayout);

    JGCOMPONENT button = JGCreateButton("Hello");
    JGSIZE s = { 200, 40 };
    JGSetSize(button, s);
    JGSetFixedSize(button, 1); // alternative: JGSetState(button, JGCOMPS_FIXEDSIZE, 1);
    JGAddListener(button, ButtonListener);
    JGAddChild(bottomCont, button);

    JGCOMPONENT canvas = JGCreateCanvas(1, CanvasPainter);
    JGAddChild(bottomCont, canvas);
    JGAddListener(canvas, CanvasListener);
    //////////////////////

    JGAddChild(app, bottomCont);

    //JGToggleFullScreen(app);

    JGRunApplication(app, JGRA_WAIT);
    return 0;
}
