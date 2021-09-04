#include "jggraphics.h"
#include "jgbase.h"
#include "jggeom.h"

JGGRAPHICS JGCreateGraphics(JGIMAGE *image, JGCOLORPALETTE *palette)
{
    JGGRAPHICS g = malloc(sizeof(JGGRAPHICS__));
    g->image = *image;
    g->palette = *palette;
    g->tm[0] = 1.0f; g->tm[1] = 0.0f; g->tm[2] = 0.0f;
    g->tm[3] = 0.0f; g->tm[4] = 1.0f; g->tm[5] = 0.0f;
    g->tm[6] = 0.0f; // 7 unused   // 8 unused
    return g;
}

bool JGDestroyGraphics(JGGRAPHICS g)
{
    if(g == NULL)
        return 0;
    free(g->image.pixels);
    free(g);
    return 1;
}


// some utility
static void colset(color_t *dst, color_t color, int n)
{
    while(n--)
        *(dst++) = color;
}

static void colsetinc(color_t *dst, int inc, color_t color, int n)
{
    while(n--)
    {
        *dst = color;
        dst += inc;
    }
}

static void JGFastDrawStroke(color_t *pixel, unit_t width, color_t stroke, int strokeWidth)
{
    if(strokeWidth == 1)
    {
        *pixel = stroke;
        return;
    }
    for(unit_t x = 1; x <= strokeWidth; x++)
    {
        *(pixel + x) = stroke;
        *(pixel - x) = stroke;
        *(pixel + x * width) = stroke;
        *(pixel - x * width) = stroke;
        for(unit_t y = 1; y <= strokeWidth - x; y++)
        {
            *(pixel + x + y * width) = stroke;
            *(pixel - x + y * width) = stroke;
            *(pixel + x - y * width) = stroke;
            *(pixel - x - y * width) = stroke;
        }
    }
}

void JGCopyImage(JGIMAGE *imgDest, unit_t x1, unit_t y1, unit_t w1, unit_t h1, CLP(JGIMAGE) imgSrc, unit_t x, unit_t y)
{
    JGRECT rect = { .x = 0, .y = 0, .width = w1, .height = h1 };
    JGRECT rect2 = { .x = x, .y = y, .width = imgSrc->width - x, imgSrc->height - y };
    JGRECT draw;
    if(!JGRectIntersection(&rect, &rect2, &draw))
        return;
    color_t *destPixels = imgDest->pixels + x1 + y1 * imgDest->width;
    color_t *srcPixels = imgSrc->pixels + x + y * imgSrc->width;
    while(draw.height--)
    {
        memmove(destPixels, srcPixels, draw.width * sizeof(color_t));
        destPixels += imgDest->width;
        srcPixels += imgSrc->width;
    }
}

void JGFillRect(JGGRAPHICS g, CLP(JGRECT) rect)
{
    const int imageWidth = g->image.width;
    const int imageHeight = g->image.height;
    JGRECT bounds = { .x = 0, .y = 0, .width = imageWidth, .height = imageHeight };
    JGRECT draw;
    if(!JGRectIntersection(&bounds, rect, &draw))
        return;
    color_t *pixels = g->image.pixels + draw.x + draw.y * imageWidth;
    const color_t fill = g->fill;
    while(draw.height--)
    {
        colset(pixels, fill, draw.width);
        pixels += imageWidth;
    }
}


static inline void JGDrawRect0(JGGRAPHICS g, int left, int top, int right, int bottom)
{
    int imageWidth = g->image.width;
    int imageHeight = g->image.height;
    if(right < 0 || bottom < 0 || top >= imageHeight || left >= imageWidth)
        return;
    color_t *pixels = g->image.pixels;
    color_t stroke = g->stroke;
    colsetinc(pixels + left + top * imageWidth, imageWidth, stroke, bottom - top);
    colsetinc(pixels + right + top * imageWidth, imageWidth, stroke, bottom - top);
    colset(pixels + left + top * imageWidth, stroke, right - left);
    colset(pixels + left + bottom * imageWidth, stroke, right - left);
}

void JGDrawRect(JGGRAPHICS g, int x, int y, int w, int h)
{
    JGDrawRect0(g, x, y, x + w, y + h);
}

void JGDrawEllipse(JGGRAPHICS g, int x, int y, int w, int h)
{

}

// nice dash shape (not used)
/*
void JGFillDash(JGGRAPHICS g, int x, int y, int r)
{
    const int rr = r * r;

    const int imgWidth = g->image.width;
    color_t *pixels = g->image.pixels + x + y * imgWidth;
    int centerOffset = 0;
    const color_t fill = g->fill;

    int xi;
    int yi = 0;
    const int r2 = 2 * r;
    while(yi < r)
    {
        for(xi = -r; xi; xi++)
            if(xi * xi + yi * yi <= rr)
                break;
        colset(pixels + xi - centerOffset, fill, r);
        colset(pixels + xi + centerOffset, fill, r);
        centerOffset += imgWidth;
        yi++;
    }
}
*/

// TODO: Add bounds check
void JGDrawCircle(JGGRAPHICS g, int x, int y, int r)
{
    x += r;
    y += r;

    int xi = r;
    int yi = 0;

    int imageWidth = g->image.width;
    color_t *pixels = g->image.pixels + x + y *imageWidth;
    color_t stroke = g->stroke;

    *(pixels + r) = stroke;

    if(r > 0)
    {
        *(pixels - r * imageWidth) = stroke;
        *(pixels + r * imageWidth) = stroke;
        *(pixels - r) = stroke;
    }

    int P = 1 - r;
    while(xi > yi)
    {

        yi++;

        if(P <= 0)
        {
            P += 2 * yi + 1;
        }
        else
        {
            xi--;
            P += 2 * (yi - xi) + 1;
        }

        if(xi < yi)
            break;


        *(pixels + xi + yi * imageWidth) = stroke;
        *(pixels - xi + yi * imageWidth) = stroke;
        *(pixels + xi - yi * imageWidth) = stroke;
        *(pixels - xi - yi * imageWidth) = stroke;

        if(xi != yi)
        {
            *(pixels + yi + xi * imageWidth) = stroke;
            *(pixels - yi + xi * imageWidth) = stroke;
            *(pixels + yi - xi * imageWidth) = stroke;
            *(pixels - yi - xi * imageWidth) = stroke;
        }
    }
}

void JGFillCircle(JGGRAPHICS g, int x, int y, int r)
{
    if(r <= 0)
        return;
    x += r;
    y += r;

    int xi = r;
    int yi = 0;

    int imageWidth = g->image.width;
    color_t *pixels = g->image.pixels + x + y *imageWidth;
    color_t fill = g->fill;

    colset(pixels - r, fill, 2 * r);

    int P = 1 - r;
    while(xi > yi)
    {

        yi++;

        if(P <= 0)
        {
            P += 2 * yi + 1;
        }
        else
        {
            xi--;
            P += 2 * (yi - xi) + 1;
        }

        if(xi < yi)
            break;

        colset(pixels - xi + yi * imageWidth, fill, 2 * xi);
        colset(pixels - xi - yi * imageWidth, fill, 2 * xi);

        if(xi != yi)
        {
            colset(pixels - yi + xi * imageWidth, fill, 2 * yi);
            colset(pixels - yi - xi * imageWidth, fill, 2 * yi);
        }
    }
}

void JGDrawLine_(JGGRAPHICS g, unit_t x1, unit_t y1, unit_t x2, unit_t y2)
{
    JGLINE line = { .x1 = x1, .y1 = y1, .x2 = x2, .y2 = y2 };
    JGDrawLine(g, line);
}

void JGDrawLine(JGGRAPHICS g, JGLINE line)
{
    JGIMAGE image = g->image;
    unit_t imageWidth = image.width;
    unit_t imageHeight = image.height;

    JGLINE draw;
    JGRECT rect = { .width = imageWidth, .height = imageHeight };
    if(!JGClipLineRect(&line, &rect, &draw))
        return;

    unit_t dx = iabs(draw.x2 - draw.x1);
    unit_t sx = draw.x1 < draw.x2 ? 1 : -1;
    unit_t dy = -iabs(draw.y2 - draw.y1);
    unit_t sy = draw.y1 < draw.y2 ? 1 : -1;

    unit_t err = dx + dy;
    unit_t err2;

    color_t *pixels = image.pixels + draw.x1 + draw.y1 * imageWidth;
    color_t stroke = g->stroke;

    imageWidth *= sy;
    while(JGFastDrawStroke(pixels, imageWidth, stroke, 1), draw.x1 != draw.x2 || draw.y1 != draw.y2)
    {
        err2 = 2 * err;
        if(err2 >= dy) /* e_xy + e_x > 0 */
        {
            err += dy;
            draw.x1 += sx;
            pixels += sx;
        }
        if(err2 <= dx) /* e_xy + e_y < 0 */
        {
            err += dx;
            draw.y1 += sy;
            pixels += imageWidth;
        }
    }
}

static inline void JGFillEllipse0(JGGRAPHICS g, int left, int top, int right, int bottom)
{
    int w = g->image.width;
    int h = g->image.height;
    if(left < 0)
        left = 0;
    if(top < 0)
        top = 0;
    if(right > w)
        right = w;
    if(bottom > h)
        bottom = h;
    color_t *pixels = g->image.pixels;
    color_t fill = g->fill;
    int ww = w * w;
    int hh = h * h;
    int ww_hh = ww + hh;
    int px, py;
    for(int y = top; y < bottom; y++)
    for(int x = left; x < right; x++)
    {
        px = x - left;
        py = y - left;
        if(px * px * ww + py * py * hh <= ww_hh)
        {
            int i = x + y * w;
            *(pixels + i) = fill;
        }
    }
}

void JGFillEllipse(JGGRAPHICS g, int x, int y, int w, int h)
{
    const int w_2 = w >> 1;
    const int h_2 = h >> 1;

    //const int cx = x + w_2;

    const int w_2w_2 = w_2 * w_2;
    const int h_2h_2 = h_2 * h_2;

    const int w_2w_2h_2h_2 = w_2w_2 * h_2h_2;

    const int imgWidth = g->image.width;
    color_t *pixels = g->image.pixels + x + y * imgWidth;
    const color_t fill = g->fill;

    int xi;
    int yp;
    yp = (h - 1) * (h - 1) * w_2w_2;
    while(h--)
    {
        for(xi = 0; xi < w_2; xi++)
            if(xi * xi * h_2h_2 + h * h * w_2w_2 <= w_2w_2h_2h_2)
                break;
        colset(pixels + xi, fill, w - xi);
        pixels += imgWidth;
        yp -= w_2w_2 * h + 2;
    }

    /*for(int yi = 0; yi <= h; yi++)
    for(int xi = 0; xi <= w; xi++)
        if(xi * xi * h2 + yi * yi * w2 <= w2h2)
        {
            *(pixels + x + xi + (y + yi) * imgW) = fill;
            *(pixels + x - xi + (y + yi) * imgW) = fill;
            *(pixels + x + xi + (y - yi) * imgW) = fill;
            *(pixels + x - xi + (y - yi) * imgW) = fill;
        }*/
}

