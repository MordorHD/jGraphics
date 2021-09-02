#ifndef __JGLAYOUT_H__
#define __JGLAYOUT_H__

#include "typedefs.h"

// forward declaration needed, because including "jgcomps.h" would lead to weird nesting
struct ComponentTag;
struct ContainerTag;
struct LayoutTag;

typedef void (*JGLAYOUTCOMPONENTS)(struct LayoutTag*, struct ComponentTag**, int, unit_t, unit_t, unit_t, unit_t);

// these layouts do not get an extra struct because they just have the layoutFunc
#define JGLAYOUTT_FLOW 0x1
#define JGLAYOUTT_STACK 0x2

#define JGLAYOUTT_GRIDBAG 0x3

typedef struct GridBagLayoutTag {
    JGLAYOUTCOMPONENTS layoutFunc;
} JGGRIDBAGLAYOUT__, *JGGRIDBAGLAYOUT;


// a layout positions components of a container in a specific way
// every layout struct must have a layout function pointer as first value
typedef struct LayoutTag {
    int type;
    union {
        JGLAYOUTCOMPONENTS layoutFunc;
        JGGRIDBAGLAYOUT gbg;
    };
} JGLAYOUT__, *JGLAYOUT;

JGLAYOUT JGCreateLayout(int, JGLAYOUTCOMPONENTS);
JGLAYOUT JGCreateFlowLayout(void);
JGLAYOUT JGCreateStackLayout(void);
bool JGDestroyLayout(JGLAYOUT);

#endif // __JGLAYOUT_H__
