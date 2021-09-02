#include "jggraphics.h"
#include "jgbase.h"
#include "jggeom.h"

JGGRAPHICS JGCreateGraphics(JGIMAGE image, JGCOLORPALETTE palette)
{
    JGGRAPHICS g = malloc(sizeof(JGGRAPHICS__));
    g->image = *image;
    g->palette = palette;
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

static void JGSetPixel(color_t *dst, int x, int y, int w, int h, color_t c)
{
    if(x < 0 || y < 0 || x >= w || y >= h)
        return;
    *dst = c;
}

void JGCopyImage(JGIMAGE imgDest, int x1, int y1, const JGIMAGE imgSrc)
{
    int w1 = imgDest->width;
    int h1 = imgDest->height;
    int dwx = w1 - x1;
    int dwy = h1 - y1;
    if(dwx <= 0 || dwy <= 0)
        return;
    int w = imgSrc->width;
    int w2 = w;
    int h = imgSrc->height;
    if(w > dwx)
        w = dwx;
    if(h > dwy)
        h = dwy;
    color_t *destPixels = imgDest->pixels + x1 + y1 * w1;
    color_t *srcPixels = imgSrc->pixels;
    while(h--)
    {
        memmove(destPixels, srcPixels, w * sizeof(color_t));
        destPixels += w1;
        srcPixels += w2;
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
        printf("%d, %d\n", xi, h);
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

void JGDrawEllipse(JGGRAPHICS g, int x, int y, int w, int h)
{

}

void JGFillCircle(JGGRAPHICS g, int x, int y, int r)
{
    const int imgWidth = g->image.width;
    const int imgHeight = g->image.height;

    int lOverlap = r - x;
    if(lOverlap < 0)
        lOverlap = 0;
    int tOverlap = r - y;
    if(tOverlap < 0)
        tOverlap = 0;
    int rOverlap = (x + r) - imgWidth;
    if(rOverlap < 0)
        rOverlap = 0;
    int bOverlap = (y + r) - imgHeight;
    if(bOverlap < 0)
        bOverlap = 0;

    color_t *pixels = g->image.pixels + x + y * imgWidth;
    const color_t fill = g->fill;

    int xi;
    int nxi2;
    int centerOffset = tOverlap * imgWidth;
    const int rr = r * r;
    for(int yi = tOverlap, ye = r - bOverlap; yi < ye; yi++)
    {
        for(xi = -r; xi; xi++)
            if(xi * xi + yi * yi <= rr)
                break;
        nxi2 = -2 * xi;
        colset(pixels + (xi - centerOffset), fill, nxi2);
        colset(pixels + (xi + centerOffset), fill, nxi2);
        centerOffset += imgWidth;
    }
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
    r >>= 1;

    x += r;
    y += r;

    int xi = r;
    int yi = 0;

    int imgW = g->image.width;
    color_t *pixels = g->image.pixels;
    color_t stroke = g->stroke;

    *(pixels + x + r + y * imgW) = stroke;

    if(r > 0)
    {
        *(pixels + x + (y - r) * imgW) = stroke;
        *(pixels + x + (y + r) * imgW) = stroke;
        *(pixels + x - r + y * imgW) = stroke;
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

        *(pixels + x + xi + (y + yi) * imgW) = stroke;
        *(pixels + x - xi + (y + yi) * imgW) = stroke;
        *(pixels + x + xi + (y - yi) * imgW) = stroke;
        *(pixels + x - xi + (y - yi) * imgW) = stroke;

        if(xi != yi)
        {
            *(pixels + x + xi + (y + yi) * imgW) = stroke;
            *(pixels + x - xi + (y + yi) * imgW) = stroke;
            *(pixels + x + xi + (y - yi) * imgW) = stroke;
            *(pixels + x - xi + (y - yi) * imgW) = stroke;
        }
    }
}

void JGDrawLine(JGGRAPHICS g, int x1, int y1, int x2, int y2)
{
    JGIMAGE__ image = g->image;
    int imageWidth = image.width;
   // int imageHeight = image.height;

    int dx = iabs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -iabs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;

    int err = dx + dy;  /* error value e_xy */
    int err2;
    color_t *pixels = image.pixels + x1 + y1 * imageWidth;
    imageWidth *= sy;
    while(*pixels = g->stroke, x1 != x2 || y1 != y2)
    {
        err2 = 2 * err;
        if(err2 >= dy) /* e_xy + e_x > 0 */
        {
            err += dy;
            x1 += sx;
            pixels += sx;
        }
        if(err2 <= dx) /* e_xy + e_y < 0 */
        {
            err += dx;
            y1 += sy;
            pixels += imageWidth;
        }
    }
}

