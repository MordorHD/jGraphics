#ifndef __JGCOMPS_H__
#define __JGCOMPS_H__

#include "jgevent.h"
#include "jgtext.h"
#include "jglayout.h"
#include "jggraphics.h"
#include "typedefs.h"

// forward declaration needed, because component needs container, but container also needs component
struct ComponentTag;
struct ApplicationTag;

typedef struct ContainerTag {
    struct ComponentTag *children;
    int childCnt;
    JGLAYOUT layout;
} JGCONTAINER__, *JGCONTAINER;

struct ComponentTag *JGCreateContainer(void);
struct ComponentTag *JGAddChild0(JGCONTAINER, struct ComponentTag*);
bool JGRemoveChild0(JGCONTAINER, int);
int JGIndexOfChild0(JGCONTAINER, struct ComponentTag*);

typedef struct ButtonTag {
    JGTEXT text;
    int state;
} JGBUTTON__, *JGBUTTON;

struct ComponentTag *JGCreateButton(void);

#define JGSetButtonText(button, str) JGSetString((button)->button->text, str)
#define JGGetButtonText(button, buf, bufLen) JGGetString((button)->button->text, buf, bufLen)

typedef int (*JGLISTENER)(struct ComponentTag*, const JGEVENT*);

typedef void (*JGPAINTER)(struct ComponentTag*, JGGRAPHICS);

#define JGCOMPT_CONTAINER 0x1
#define JGCOMPT_BUTTON 0x2
#define JGCOMPT_SLIDER 0x3
#define JGCOMPT_TEXTAREA 0x4
#define JGCOMPT_CANVAS 0x5
#define JGCOMPT_CHECKBOX 0x6
#define JGCOMPT_CHOICEMENU 0x7
#define JGCOMPT_LABEL 0x8
#define JGCOMPT_RADIOBUTTON 0x9
#define JGCOMPT_SCROLLPANE 0xA
#define JGCOMPT_TABBEDPANE 0xB
#define JGCOMPT_TEXTINPUT 0xC
#define JGCOMPT_MENUBAR 0xD

#define JGCOMPS_MOUSEIN 0x1
#define JGCOMPS_PRESSED 0x2
#define JGCOMPS_TOGGLED 0x4
#define JGCOMPS_REDRAW 0x8
#define JGCOMPS_FORWARD 0x10
#define JGCOMPS_MIEVENT 0x20
#define JGCOMPS_MOEVENT 0x40
#define JGCOMPS_PSEVENT 0x80
#define JGCOMPS_TGEVENT 0x100

typedef struct ComponentTag {
    struct ApplicationTag *app;
    short type;
    // General state of component
    // Bit 1 - Mouse in
    // Bit 2 - Mouse was pressed inside and is still being held down
    // Bit 3 - Every time this component is pressed, this bit is toggled
    //
    // Bit 4 - If this component should get redrawn
    // Bit 5 - If this component forwards forwardable (events width type between JGEVENT_FORWARD_MIN and JGEVENT_FORWARD_MAX) events
    //         If this bit is toggled, the component base struct must have JGCOMPONENT* as their first value and int as the 2nd
    //
    // Bit 6 - If this component redraws when the mouse enters it
    // Bit 7 - If this component redraws when the mouse leaves it
    // Bit 8 - If this component redraws when it was pressed
    short state;
    JGCONTAINER parent;
    int x;
    int y;
    int width;
    int height;
    JGLISTENER *listener;
    int listenerCnt;
    JGPAINTER painter;
    union {
        JGCONTAINER container;
        JGBUTTON button;
        //JGSLIDER slider;
        //JGTEXTAREA textArea;
        //JGCANVAS canvas;
        //JGCHECKBOX checkBox;
        //JGCHOICEMENU choiceMenu;
        //JGLABEL label;
        //JGRADIOBUTTON radioButton;
        //JGSCROLLPANE scrollPane;
        //JGTABBEDPANE tabbedPane;
        //JGTEXTINPUT textInput;
        //JGMENUBAR menuBar;
    };
} JGCOMPONENT__, *JGCOMPONENT;

JGCOMPONENT JGCreateComponent(int, void*, int, JGLISTENER, JGPAINTER);

short JGDispatchEvent(JGCOMPONENT, const JGEVENT*);
short JGDispatchEventAndForward(JGCOMPONENT, const JGEVENT*);
bool JGDestroyComponent(JGCOMPONENT);

#define JGRC_NORMAL 0x0
#define JGRC_FORCE 0x1

void JGRedrawComponent(JGCOMPONENT, JGGRAPHICS, int);
void JGAddListener(JGCOMPONENT, JGLISTENER);

// exclusive
bool JGRectContains(JGCOMPONENT, int, int);
bool JGRectInstersects(JGCOMPONENT, int, int, int, int);

void JGSetBounds(JGCOMPONENT, int, int, int, int);
void JGSetPos(JGCOMPONENT, int, int);
void JGSetSize(JGCOMPONENT, int, int);

#define JGAddChild(parent, child) (child)->app = (parent)->app; JGAddChild0((parent)->container, child)
#define JGRemoveChild(parent, index) JGRemoveChild0((parent)->container, index)
#define JGIndexOfChild(parent, child) JGIndexOfChild0((parent)->container, child)
#define JGContainsChild(parent, child) (JGIndexOfChild0((parent)->container, child)!=-1)

#define JGSetX(comp, posX) ((comp->x) = posX)
#define JGSetY(comp, posY) ((comp->y) = posY)
#define JGSetWidth(comp, sizeX) ((comp)->width = sizeX)
#define JGSetHeight(comp, sizeY) ((comp)->height = sizeY)

#endif // __JGCOMPS_H__
