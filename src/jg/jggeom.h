#ifndef __JGGEOM_H__
#define __JGGEOM_H__

// also includes some math

#include "typedefs.h"

typedef struct PointTag {
    unit_t x;
    unit_t y;
} JGPOINT;

typedef struct SizeTag {
    unit_t width;
    unit_t height;
} JGSIZE;

typedef struct RectTag {
    union {
        struct {
            unit_t x;
            unit_t y;
        };
        JGPOINT point;
    };
    union {
        struct {
            unit_t width;
            unit_t height;
        };
        JGSIZE size;
    };
} JGRECT;

typedef struct LineTag {
    union {
        struct {
            unit_t x1;
            unit_t y1;
        };
        JGPOINT point1;
    };
    union {
        struct {
            unit_t x2;
            unit_t y2;
        };
        JGPOINT point2;
    };
} JGLINE;

// exclusive
bool JGRectContains(CLP(JGRECT), CLP(JGPOINT));
bool JGRectsInstersect(CLP(JGRECT), CLP(JGRECT));
bool JGRectIntersection(CLP(JGRECT), CLP(JGRECT), JGRECT*);

bool JGLinesIntersect(CLP(JGLINE), CLP(JGLINE));
bool JGLineIntersection(CLP(JGLINE), CLP(JGLINE), JGPOINT*);
bool JGLineRectIntersect(CLP(JGRECT), CLP(JGRECT));
bool JGLineRectIntersection(CLP(JGRECT), CLP(JGLINE), JGLINE*);
bool JGClipLineRect(CLP(JGLINE), CLP(JGRECT), JGLINE*);

unit_t imax(unit_t, unit_t);
unit_t imin(unit_t, unit_t);
unit_t iabs(unit_t);
unit_t isgn(unit_t);

// floating point math
#include <math.h>

typedef struct Vec2fTag {
    unitf_t x;
    unitf_t y;
    unitf_t a;
} VECTOR2F;

void JGVecAddf(VECTOR2F*, unitf_t, unitf_t);
void JGVecSubf(VECTOR2F*, unitf_t, unitf_t);
void JGVecMultf(VECTOR2F*, unitf_t, unitf_t);
void JGVecDivf(VECTOR2F*, unitf_t, unitf_t);

void JGVecAdd(CLP(VECTOR2F), CLP(VECTOR2F), VECTOR2F*);
void JGVecSub(CLP(VECTOR2F), CLP(VECTOR2F), VECTOR2F*);
void JGVecMult(CLP(VECTOR2F), CLP(VECTOR2F), VECTOR2F*);
void JGVecDiv(CLP(VECTOR2F), CLP(VECTOR2F), VECTOR2F*);

void JGVecSetAngle(VECTOR2F*, unitf_t);
#define JGVecRotate(vec, a_) JGVectfSetAngle(vec, (vec)->a += a_)

unitf_t JGVecSetMagnitude(VECTOR2F*, unitf_t);
#define JGVecNormalize(vec) JGVectSetMagnitude(vec, (unitf_t) 1)

#endif // __JGGEOM_H__
