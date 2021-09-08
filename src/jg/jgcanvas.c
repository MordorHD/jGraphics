#include "jgcomps.h"
#include "jgbase.h"

//////////////////////////////////
//          CANVAS              //
//////////////////////////////////

static int JGCanvasListener(JGCOMPONENT comp, const JGEVENT *event)
{
    JGCANVAS canvas = comp->canvas;
    if(comp->state & JGCANVAS_STATE_BUFFER)
    switch(event->id)
    {
    case JGEVENT_ID_SIZE:
    {
        unit_t newWidth = comp->width;
        unit_t newHeight = comp->height;
        JGGRAPHICS buffer = canvas.buffer;
        JGIMAGE img = buffer->image;
        // the original buffer is kept if the new image size is smaller than or equal to the old image size
        if(newWidth <= img.width && newHeight <= img.height)
            return 0;
        if(newWidth < img.width)
            newWidth = img.width;
        if(newHeight < img.height)
            newHeight = img.height;
        JGIMAGE newImage;
        newImage.width = newWidth;
        newImage.height = newHeight;
        newImage.pixels = malloc(sizeof(color_t) * newWidth * newHeight);
        JGCopyImage(&newImage, 0, 0, newWidth, newHeight, &img, 0, 0);
        free(img.pixels);
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
    if(comp->state & JGCANVAS_STATE_BUFFER)
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
    JGCOMPONENT comp = JGCreateComponent(JGCOMP_TYPE_CANVAS, (useBuffer ? JGCANVAS_STATE_BUFFER : 0), JGCanvasListener, JGCanvasPainter);
    if(useBuffer)
    {
        JGIMAGE img = {0};
        JGCOLORPALETTE pal;
        comp->canvas.buffer = JGCreateGraphics(&img, &pal);
    }
    comp->canvas.painter = painter;
    return comp;
}
