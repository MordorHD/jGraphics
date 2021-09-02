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

/*
bool JGLineRectIntersection(CLP(JGLINE) line, CLP(JGRECT) rect, JGLINE* lineDst)
{
    JGPOINT unit_tersection;
    unit_t min_x = rect->x;
    unit_t min_y = rect->y;
    unit_t max_x = min_x + rect->width;
    unit_t max_y = min_y + rect->height;

    if(line->x2 < min_x)
    {
        if(line->y2 > min_y && line->y2 < max_y)
        {
            JGLINE line2 = { min_x, min_y, min_x, max_y};
            return JGLineIntersection(line, &line2);
        }
        else if(line->y2 < min_y)
        {
            JGLINE tl = { min_x, min_y, max_x, min_y};
            JGLINE tl = { min_x, min_x, min_x, max_y};
            if(!JGLineIntersection(line, &line2, &lineDst->pounit_t1) && !JGLineIntersection(line, new Vector2(min_x, min_y), new Vector2(min_x, max_y))
               return 0;
            return 1;
        }
        else
        {
            unit_tersection = JGLineIntersection(line, new Vector2(min_x, max_y), new Vector2(max_x, max_y));
            if(unit_tersection == null)
                unit_tersection = JGLineIntersection(line, new Vector2(min_x, min_y), new Vector2(min_x, max_y));
            return unit_tersection;
        }
    }
    else if(line->x2 > max_x)
    {
        if(line->y2 > min_y &&line->y2 < max_y)
        {
            return JGLineIntersection(line, new Vector2(max_x, min_y), new Vector2(max_x, max_y));
        }
        else if(line->y2 < min_y)
        {
            unit_tersection = JGLineIntersection(line, new Vector2(min_x, min_y), new Vector2(max_x, min_y));
            if (unit_tersection == null)
                unit_tersection = JGLineIntersection(line, new Vector2(max_x, min_y), new Vector2(max_x, max_y));
            return unit_tersection;
        }
        else
        {
            unit_tersection = JGLineIntersection(line, new Vector2(min_x, max_y), new Vector2(max_x, max_y));
            if (unit_tersection == null)
                unit_tersection = JGLineIntersection(line, new Vector2(max_x, min_y), new Vector2(max_x, max_y));
            return unit_tersection;
        }
    }
    else
    {
        if(p2->y < min_y)
            return LSegsIntersectionPounit_t(line, new Vector2(min_x, min_y), new Vector2(max_x, min_y));
        if(p2->y > max_y)
            return LSegsIntersectionPounit_t(line, new Vector2(min_x, max_y), new Vector2(max_x, max_y));
    }
    return null;

}*/

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
