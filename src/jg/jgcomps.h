#ifndef __JGCOMP_STATE_H__
#define __JGCOMP_STATE_H__

#include "jgevent.h"
#include "jgtext.h"
#include "jglayout.h"
#include "jggraphics.h"
#include "typedefs.h"
#include "jggeom.h"

// forward declaration needed, because component needs container, but container also needs component
struct ComponentTag;
struct ApplicationTag;

typedef int (*JGLISTENER)(struct ComponentTag*, const JGEVENT*);
typedef void (*JGPAINTER)(struct ComponentTag*, JGGRAPHICS);

#define JGCOMP_TYPE_CONTAINER 0x1

typedef struct ContainerTag {
    struct ComponentTag **children;
    int childCnt;
    JGLAYOUT layout;
} JGCONTAINER;

struct ComponentTag *JGCreateContainer(void);
// zero naming convention because functions take in a container, but you usually want to say
// JGCOMPONENT comp = JGCreateContainer();
// JGAddChild(comp, child); instead of JGAddChild(comp->container, child);
// matching macros are below
void JGAddChild0(JGCONTAINER*, struct ComponentTag*);
bool JGRemoveChild0(JGCONTAINER*, int);
int JGIndexOfChild0(JGCONTAINER*, struct ComponentTag*);

#define JGAddChild(parent, child) JGAddChild0(&(parent)->container, child)
#define JGRemoveChild(parent, index) JGRemoveChild0(&(parent)->container, index)
#define JGIndexOfChild(parent, child) JGIndexOfChild0(&(parent)->container, child)
#define JGContainsChild(parent, child) (JGIndexOfChild0(&(parent)->container, child)!=-1)

#define JGSetLayout(comp, layout_) ((comp)->container.layout=layout_)
#define JGRelayout(comp) (comp)->layout\
    { JGCONTAINER cont_macro_ = (comp)->container; \
      JGLAYOUT layout_macro_ = cont_macro_.layout; \
      if(layout_macro_ != NULL) \
          layout_macro_.layoutFunc(layout_macro_, cont_macro_.children, cont_macro_.childCnt, (comp)->x, (comp)->y, (comp)->width, (comp)->height); }

#define JGCOMP_TYPE_BUTTON 0x2
// no struct needed, only uses text
struct ComponentTag *JGCreateButton(string_t);

#define JGSetButtonText(comp, str) JGSetText(comp, str)
#define JGGetButtonText(comp, buf, bufLen) JGGetText(comp, buf, bufLen)

#define JGCOMP_TYPE_SLIDER 0x3

typedef struct SliderTag {
    int minValue;
    int value;
    int maxValue;
} JGSLIDER;

struct ComponentTag *JGCreateSlider(int, int, int);

#define JGCOMP_TYPE_TEXTAREA 0x4
#define JGCOMP_TYPE_CANVAS 0x5

#define JGCANVAS_BUFFER JGCOMP_STATE_UNUSED1

typedef struct CanvasTag {
    JGGRAPHICS buffer;
    JGPAINTER painter;
} JGCANVAS;

struct ComponentTag *JGCreateCanvas(bool, JGPAINTER);

#define JGCOMP_TYPE_CHECKBOX 0x6
// no struct needed, has no additional values
struct ComponentTag *JGCreateCheckBox(void);

#define JGCOMP_TYPE_CHOICEMENU 0x7
#define JGCOMP_TYPE_LABEL 0x8
// no struct needed, only uses text
struct ComponentTag *JGCreateLabel(string_t);

#define JGCOMP_TYPE_RADIOBUTTON 0x9
#define JGCOMP_TYPE_SCROLLPANE 0xA
#define JGCOMP_TYPE_TABBEDPANE 0xB
#define JGCOMP_TYPE_TEXTINPUT 0xC
#define JGCOMP_TYPE_MENUBAR 0xD

#define JGCOMP_STATE_MOUSEIN 0x1
#define JGCOMP_STATE_PRESSED 0x2
#define JGCOMP_STATE_TOGGLED 0x4
#define JGCOMP_STATE_REDRAW 0x8
#define JGCOMP_STATE_NOTDRAWBG 0x10
#define JGCOMP_STATE_FORWARD 0x20
#define JGCOMP_STATE_LAYOUT 0x40
#define JGCOMP_STATE_MIEVENT 0x80
#define JGCOMP_STATE_MOEVENT 0x100
#define JGCOMP_STATE_PSEVENT 0x200
#define JGCOMP_STATE_TGEVENT 0x400
#define JGCOMP_STATE_FIXEDSIZE 0x800

// These bits are used by component bases
#define JGCOMP_STATE_UNUSED1 0x1000
#define JGCOMP_STATE_UNUSED2 0x2000
#define JGCOMP_STATE_UNUSED3 0x4000
#define JGCOMP_STATE_UNUSED4 0x8000

typedef struct ComponentTag {
    short type;
    // General state of component
    // Bit 1 - Mouse in
    // Bit 2 - Mouse was pressed inside and is still being held down
    // Bit 3 - Every time this component is pressed, this bit is toggled
    //
    // Bit 4 - If this component should get redrawn
    // Bit 5 - If this component should not draw its background
    // Bit 6 - If this component forwards forwardable (events width type between JGEVENT_FORWARD_MIN and JGEVENT_FORWARD_MAX) events
    //         If this bit is toggled, the component base struct must have JGCOMPONENT* (children) as their first value and int as the 2nd
    // Bit 7 - If this component should layout its children
    // Bit 8 - If this component redraws when the mouse enters it
    // Bit 9 - If this component redraws when the mouse leaves it
    // Bit 10 - If this component redraws when it is pressed
    // Bit 11 - If this component redraws when it is toggled
    // Bit 12 - If this components size should get affected by a layout
    // Bit 13 - 16 - Base component defined
    short state;
    // all children from the parent
    struct ComponentTag **parent;
    union {
        JGRECT rect;
        struct {
            unit_t x;
            unit_t y;
            unit_t width;
            unit_t height;
        };
    };
    // listeners
    JGLISTENER *listener;
    int listenerCnt;
    // paint procedure of this component
    JGPAINTER painter;
    // base of this component
    union {
        JGTEXT text;
        JGCONTAINER container;
        //JGSLIDER slider;
        //JGTEXTAREA textArea;
        JGCANVAS canvas;
        //JGCHECKBOX checkBox;
        //JGCHOICEMENU choiceMenu;
        //JGRADIOBUTTON radioButton;
        //JGSCROLLPANE scrollPane;
        //JGTABBEDPANE tabbedPane;
        //JGTEXTINPUT textInput;
        //JGMENUBAR menuBar;
    };
} JGCOMPONENT__, *JGCOMPONENT;

JGCOMPONENT JGCreateComponent(int, int, JGLISTENER, JGPAINTER);

short JGDispatchEvent(JGCOMPONENT, const JGEVENT*);
short JGDispatchEventAndForward(JGCOMPONENT, const JGEVENT*);
bool JGDestroyComponent(JGCOMPONENT);

#define JGSetState(comp, stFlag, bool) ((bool)?((comp)->state|=stFlag):((comp)->state&=~stFlag))
#define JGSetFixedSize(comp, bool) JGSetState(comp, JGCOMP_STATE_FIXEDSIZE, bool)
#define JGSetText(comp, str) JGSetString((comp)->text, str)
#define JGGetText(comp, buf, bufLen) JGGetString((comp)->text, buf, bufLen)

#define JGRC_NORMAL 0x0
#define JGRC_FORCE 0x1

void JGRedrawComponent(JGCOMPONENT, JGGRAPHICS, int);
void JGAddListener(JGCOMPONENT, JGLISTENER);

void JGSetBounds(JGCOMPONENT, JGRECT);
void JGSetPos(JGCOMPONENT, JGPOINT);
void JGSetSize(JGCOMPONENT, JGSIZE);

#define JGSetX(comp, posX) ((comp->x)=posX)
#define JGSetY(comp, posY) ((comp->y)=posY)
#define JGSetWidth(comp, sizeX) ((comp)->width=sizeX)
#define JGSetHeight(comp, sizeY) ((comp)->height=sizeY)

#endif // __JGCOMP_STATE_H__
