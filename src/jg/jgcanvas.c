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
        JGGRAPHICS buffer = canvas.buffer;
        JGIMAGE newImage;
        newImage.width = comp->width;
        newImage.height = comp->height;
        newImage.pixels = malloc(sizeof(color_t) * newImage.width * newImage.height);
        JGCopyImage(&newImage, 0, 0, buffer->image.width, buffer->image.height, &buffer->image, 0, 0);
        free(buffer->image.pixels);
        buffer->image = newImage;
        break;
    }
    }
    return 0;
}


static void JGCanvasPainter(JGCOMPONENT comp, JGGRAPHICS g)
{
    JGCANVAS canvas = comp->canvas;
    if(!canvas.painter)
        return;
    if(comp->state & JGCANVAS_BUFFER)
    {
        canvas.painter(comp, canvas.buffer);
        JGCopyImage(&g->image, comp->x, comp->y, comp->width, comp->height, &comp->canvas.buffer->image, 0, 0);
    }
    else
    {
        canvas.painter(comp, g);
    }
}

JGCOMPONENT JGCreateCanvas(bool useBuffer, JGPAINTER painter)
{
    JGCOMPONENT comp = JGCreateComponent(JGCOMP_TYPE_CANVAS, (useBuffer ? JGCANVAS_BUFFER : 0), JGCanvasListener, JGCanvasPainter);
    if(useBuffer)
    {
        JGIMAGE img = {0};
        JGCOLORPALETTE pal;
        comp->canvas.buffer = JGCreateGraphics(&img, &pal);
    }
    comp->canvas.painter = painter;
    return comp;
}
