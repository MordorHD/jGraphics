#include "jggeom.h"

unit_t iabs(unit_t i)
{
    return i < 0 ? -i : i;
}

unit_t isgn(unit_t i)
{
    return i < 0 ? -1 : 1;
}

unit_t imin(unit_t i1, unit_t i2)
{
    return i1 < i2 ? i1 : i2;
}

unit_t imax(unit_t i1, unit_t i2)
{
    return i1 > i2 ? i1 : i2;
}

bool JGRectContains(CLP(JGRECT) rect, CLP(JGPOINT) point)
{
    return point->x > rect->x && point->x < rect->x + rect->width
        && point->y > rect->y && point->y < rect->y + rect->height;
}

bool JGRectsInstersect(CLP(JGRECT) rect1, CLP(JGRECT) rect2)
{
    return rect1->x < rect2->x + rect2->width  && rect1->x + rect1->width  > rect2->x
        && rect1->y < rect2->y + rect2->height && rect1->y + rect1->height > rect2->y;
}

bool JGRectIntersection(CLP(JGRECT) rect1, CLP(JGRECT) rect2, JGRECT *rectDest)
{
    unit_t x = imax(rect1->x, rect2->x);
    unit_t y = imax(rect1->y, rect2->y);
    unit_t width  = imin(rect1->x + rect1->width, rect2->x + rect2->width) - x;
    unit_t height = imin(rect1->y + rect1->height, rect2->y + rect2->height) - y;
    if(width > 0 && height > 0)
    {
        rectDest->x = x;
        rectDest->y = y;
        rectDest->width = width;
        rectDest->height = height;
        return 1;
    }
    return 0;
}

bool JGLinesIntersect(CLP(JGLINE) line1, CLP(JGLINE) line2)
{
		unit_t s1_x = line1->x2 - line1->x1;
		unit_t s1_y = line1->y2 - line1->y1;
		unit_t s2_x = line2->x2 - line2->x1;
		unit_t s2_y = line2->y2 - line2->y1;

		unit_t s = -s1_y * (line1->x1 - line2->x1) + s1_x * (line1->y1 - line2->y1);
		unit_t t =  s2_x * (line1->y1 - line2->y1) - s2_y * (line1->x1 - line2->x1);

		return s >= 0 && s <= (-s2_x * s1_y + s1_x * s2_y)
            && t >= 0 && t <= (-s2_x * s1_y + s1_x * s2_y);
}

bool JGLineIntersection(CLP(JGLINE) line1, CLP(JGLINE) line2, JGPOINT *pointDest)
{
		unit_t s1_x = line1->x2 - line1->x1;
		unit_t s1_y = line1->y2 - line1->y1;
		unit_t s2_x = line2->x2 - line2->x1;
		unit_t s2_y = line2->y2 - line2->y1;

		unit_t s = -s1_y * (line1->x1 - line2->x1) + s1_x * (line1->y1 - line2->y1);
        unit_t t =  s2_x * (line1->y1 - line2->y1) - s2_y * (line1->x1 - line2->x1);
		unit_t s_ = (-s2_x * s1_y + s1_x * s2_y);
		unit_t t_ = (-s2_x * s1_y + s1_x * s2_y);

		if(s >= 0 && s <= s_
        && t >= 0 && t <= t_)
        {
            pointDest->x = line1->x1 + (t * s1_x) / t_;
            pointDest->y = line1->y1 + (t * s1_y) / t_;
            return 1;
        }
        return 0;
}

#define INSIDE 0x0
#define LEFT 0x1
#define RIGHT 0x2
#define TOP 0x4
#define BOTTOM 0x8

static char ComputeCode(CLP(JGRECT) rect, unit_t x, unit_t y)
{
    char code = INSIDE;

    if(x < rect->x)
        code |= LEFT;
    else if(x > rect->x + rect->width)
        code |= RIGHT;
    if(y < rect->y)
        code |= BOTTOM;
    else if(y > rect->y + rect->height)
        code |= TOP;

    return code;
}

bool JGClipLineRect(CLP(JGLINE) src, CLP(JGRECT) rect, JGLINE *dest)
{
    unit_t x1 = src->x1;
    unit_t y1 = src->y1;
    unit_t x2 = src->x2;
    unit_t y2 = src->y2;
    char code1 = ComputeCode(rect, x1, y1);
    char code2 = ComputeCode(rect, x2, y2);
    char code_out;
    unit_t x, y;
    unit_t side;

    while(1)
    {
        if(!(code1 | code2))
        {
            dest->x1 = x1;
            dest->y1 = y1;
            dest->x2 = x2;
            dest->y2 = y2;
            return 1;
        }
        if(code1 & code2)
            return 0;

        code_out = code1 ? code1 : code2;
        if(code_out & TOP)
        {
            side = rect->y + rect->height;
            x = x1 + (x2 - x1) * (side - y1) / (y2 - y1);
            y = side;
        }
        else if(code_out & BOTTOM)
        {
            side = rect->y;
            x = x1 + (x2 - x1) * (side - y1) / (y2 - y1);
            y = side;
        }
        else if(code_out & RIGHT)
        {
            side = rect->x + rect->width;
            y = y1 + (y2 - y1) * (side - x1) / (x2 - x1);
            x = side;
        }
        else if(code_out & LEFT)
        {
            side = rect->x;
            y = y1 + (y2 - y1) * (side - x1) / (x2 - x1);
            x = side;
        }
        if(code1)
        {
            x1 = x;
            y1 = y;
            code1 = ComputeCode(rect, x1, y1);
        }
        else
        {
            x2 = x;
            y2 = y;
            code2 = ComputeCode(rect, x2, y2);
        }
    }
}

void JGVecAddf(VECTOR2F *vec, unitf_t x, unitf_t y)
{
    vec->x += x;
    vec->y += y;
}

void JGVecSubf(VECTOR2F *vec, unitf_t x, unitf_t y)
{
    vec->x -= x;
    vec->y -= y;
}

void JGVecMultf(VECTOR2F *vec, unitf_t x, unitf_t y)
{
    vec->x *= x;
    vec->y *= y;
}

void JGVecDivf(VECTOR2F *vec, unitf_t x, unitf_t y)
{
    vec->x /= x;
    vec->y /= y;
}

void JGVecAdd(CLP(VECTOR2F) vec1, CLP(VECTOR2F) vec2, VECTOR2F *vecDest)
{
    vecDest->x = vec1->x + vec2->x;
    vecDest->y = vec2->y + vec2->y;
}

void JGVecSub(CLP(VECTOR2F) vec1, CLP(VECTOR2F) vec2, VECTOR2F *vecDest)
{
    vecDest->x = vec1->x - vec2->x;
    vecDest->y = vec2->y - vec2->y;
}

void JGVecMult(CLP(VECTOR2F) vec1, CLP(VECTOR2F) vec2, VECTOR2F *vecDest)
{
    vecDest->x = vec1->x * vec2->x;
    vecDest->y = vec2->y * vec2->y;
}

void JGVecDiv(CLP(VECTOR2F) vec1, CLP(VECTOR2F) vec2, VECTOR2F *vecDest)
{
    vecDest->x = vec1->x / vec2->x;
    vecDest->y = vec2->y / vec2->y;
}
void JGVecSetAngle(VECTOR2F *vec, unitf_t a)
{
    double s = sqrt((double) (vec->x * vec->x + vec->y + vec->y));
    vec->x = (unitf_t) (cos((double) a) * s);
    vec->y = (unitf_t) (sin((double) a) * s);
}

unitf_t JGVecSetMagnitude(VECTOR2F *vec, unitf_t m)
{
    unitf_t s = (unitf_t) ((double) m / sqrt((double) (vec->x * vec->x + vec->y + vec->y)));
    vec->x *= s;
    vec->y *= s;
    return s;
}
