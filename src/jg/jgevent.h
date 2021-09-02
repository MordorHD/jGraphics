#ifndef __JGEVENT_H__
#define __JGEVENT_H__

#include "typedefs.h"
#include "jggeom.h"

#define JGEVENT_ID_KEYPRESSED 0x11
#define JGEVENT_ID_KEYRELEASED 0x12
#define JGEVENT_ID_KEYTYPED 0x13

// component action events
#define JGEVENT_ID_PRESS 0x15
#define JGEVENT_ID_TOGGLE 0x16
#define JGEVENT_ID_HOVER 0x17
#define JGEVENT_ID_LEAVE 0x18

// forwardable events
#define JGEVENT_FORWARD_MIN 0x21

#define JGEVENT_ID_MOUSEPRESSED 0x21
#define JGEVENT_ID_MOUSERELEASED 0x22
#define JGEVENT_ID_MOUSECLICKED 0x23
#define JGEVENT_ID_MOUSEMOVED 0x24
#define JGEVENT_ID_MOUSEWHEEL 0x25

#define JGEVENT_FORWARD_MAX 0x28

#define JGEVENT_ID_SIZE 0x41
#define JGEVENT_ID_POS 0x42
#define JGEVENT_ID_FOCUSGAINED 0x43
#define JGEVENT_ID_FOCUSLOST 0x44

#define JGEVENT_MB_LEFT 0x1
#define JGEVENT_MB_RIGHT 0x2
#define JGEVENT_MB_MIDDLE 0x10
#define JGEVENT_MB_X1 0x20
#define JGEVENT_MB_X2 0x40

typedef struct EventTag {
    int type; // 4
    time_t time; // 12
    union {
        // mouse event
        struct {
            JGPOINT mousePos;
            int mouseButton; // 24
            union {
                int deltaWheel; // 28
                int pressedButton; // 28
                struct {
                    int dx; // 28
                    int dy; // 32
                };
            };
        };
        // key event
        struct {
            int vKey; // 16
            int keyChar; // 20
            int flags; // 24
        };
        // component event
        struct {
            int oldPosX; // 16
            int oldPosY; // 20
            int posX; // 24
            int posY; // 28
        };
        struct {
            int oldSizeX; // 16
            int oldSizeY; // 20
            int sizeX; // 24
            int sizeY; // 28
        };
    };
} JGEVENT;

#endif // __JGEVENT_H__
