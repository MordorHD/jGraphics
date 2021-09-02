#include "jgcomps.h"
#include "jgbase.h"

//////////////////////////////////
//          CANVAS              //
//////////////////////////////////

static int JGCanvasListener(JGCOMPONENT comp, const JGEVENT *event)
{
    JGCANVAS canvas = comp->canvas;
    if(comp->state & JGCANVAS_BUFFER)
    switch(event->type)
    {
    case JGEVENT_ID_SIZE:
    {
        //JGIMAGE__ newImage;
        JGGRAPHICS buffer = canvas->buffer;
        buffer->image.width = comp->width;
        buffer->image.height = comp->height;
        buffer->image.pixels = realloc(buffer->image.pixels, sizeof(color_t) * comp->width * comp->height);
        /*printf("CPY1\n");
        JGGRAPHICS buffer = canvas->buffer;
        JGCopyImage(&newImage, 0, 0, &buffer->image);
        printf("CPY2\n");
        free(buffer->image.pixels);
        buffer->image = newImage;*/
        break;
    }
    }
    return 0;
}


static void JGCanvasPainter(JGCOMPONENT comp, JGGRAPHICS g)
{
    JGCANVAS canvas = comp->canvas;
    if(!canvas->painter)
        return;
    if(comp->state & JGCANVAS_BUFFER)
    {
        canvas->painter(comp, canvas->buffer);
        JGCopyImage(&g->image, comp->x, comp->y, &comp->canvas->buffer->image);
    }
    else
    {
        canvas->painter(comp, g);
    }
}

JGCOMPONENT JGCreateCanvas(bool useBuffer, JGPAINTER painter)
{
    JGCANVAS canvas = malloc(sizeof(JGCANVAS__));
    if(useBuffer)
    {
        JGIMAGE__ image;
        image.width = 1;
        image.height = 1;
        image.pixels = malloc(sizeof(int));
        canvas->buffer = JGCreateGraphics(&image, NULL);
    }
    canvas->painter = painter;
    return JGCreateComponent(JGCOMP_TYPE_BUTTON, canvas, (useBuffer ? JGCANVAS_BUFFER : 0), JGCanvasListener, JGCanvasPainter);
}
