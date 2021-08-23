#include <stdio.h>
#include <stdlib.h>

#include "jg/jgbase.h"

int clockfunc(JGCLOCK cl, clock_t delay)
{
    printf("TICK: %ld, %ld\n", cl->runtime, delay);
    if(cl->runtime >= 1000)
        return JGCLOCKS_STOPPED;
    return JGCLOCKS_RUNNING;
}

int main()
{
    JGCLOCK c = JGCreateClock(clockfunc, 1l);
    //JGStartClock(c);
    JGDestroyClock(c);

    JGTEXT text = JGCreateText("Hello");

    char buf[512];
    JGGetString(text, buf, 512);
    printf("%s\n", buf);

    JGAppendString(text, "What is going on here?");
    JGGetString(text, buf, 512);
    printf("%s\n", buf);

    JGRemoveRangeAt(text, 10, 8);
    JGGetString(text, buf, 512);
    printf("%s\n", buf);

    int index = JGFindString(text, "here", 0);
    printf("INDEX: %d, %c\n", index, *(text->text + index));

    JGReplaceString(text, "on", 0, "off");
    JGGetString(text, buf, 512);
    printf("%s\n", buf);

    index = JGIndexOf(text, '?', 0);
    printf("INDEX: %d, %c\n", index, *(text->text + index));

    JG_INIT;

    JGCOLORPALETTE palette = JGCreateColorPalette(JGCCP_SKY);
    JGAPPLICATION app = JGCreateApplication(palette);

    JGCOMPONENT cont = JGCreateContainer();
    JGSetBounds(cont, 0, 0, 500, 500);

    JGCOMPONENT button = JGCreateButton();
    JGSetButtonText(button, "Hello");

    JGAddChild(cont, button);

    JGAddAppChild(app, cont);
    //JGToggleFullScreen(app);

    JGRunApplication(app, JGRA_WAIT);

    JGDestroyApplication(app);
    JGDestroyColorPalette(palette);

    JG_DISPOSE;
    return 0;
}
