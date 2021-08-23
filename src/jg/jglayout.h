#ifndef __JGLAYOUT_H__
#define __JGLAYOUT_H__

// forward declaration needed, because including "jgcomps.h" would lead to weird nesting
struct ComponentTag;
struct ContainerTag;

typedef void (*JGLAYOUTCOMPONENTS)(struct ContainerTag*);

#define JGLAYOUTT_FLOW 0x1

typedef struct FlowLayoutTag {
    void *ptr;
} JGFLOWLAYOUT__, *JGFLOWLAYOUT;

#define JGLAYOUTT_STACK 0x2

typedef struct StackLayoutTag {
    void *ptr;
} JGSTACKLAYOUT__, *JGSTACKLAYOUT;

#define JGLAYOUTT_GRIDBAG 0x3

typedef struct GridBagLayoutTag {
    void *ptr;
} JGGRIDBAGLAYOUT__, *JGGRIDBAGLAYOUT;


// a layout positions components of a container in a specific way
// every layout struct must have a layout function pointer as first value
typedef struct LayoutTag {
    int type;
    union {
        JGLAYOUTCOMPONENTS layoutFunc;
        JGFLOWLAYOUT flow;
        JGSTACKLAYOUT stack;
        JGGRIDBAGLAYOUT gbg;
    };
} JGLAYOUT__, *JGLAYOUT;

#endif // __JGLAYOUT_H__
