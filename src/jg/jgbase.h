/* JONAS GRAPHICS V.0
   The cross platform graphics library. Use it to create any GUI application or game in C.
   Includes:
   - Component system (IN PROGRESS)
   - Sound system (NOT YET)
   - Image system (NOT YET)

   When using any function or macro in this library, only use NULL arguments when it is explicitly allowed.
   Naming conventions:
   - Every macro function or struct has the prefix JG
   - All structs have (excluding JGEVENT) have two typedef version
     - JGSTRUCT__
     - *JGSTRUCT
   - Usually, there are functions given to create and destroy the struct pointers; in this pattern:
     - JGSTRUCT JGCreateStruct(void)
     - bool JGDestroyStruct(JGSTRUCT)

*/

#ifndef __JGBASE_H__
#define __JGBASE_H__

#include "typedefs.h"
#include "jgcomps.h"
#include <time.h>

#define JGIsFullScreen(app) ((app)->fullScreen!=NULL)

#ifdef _WIN32 // windows 32/64 bit

    #include <windows.h>
    #include <windowsx.h>
    #include <processthreadsapi.h>

    #define JGRGB(r, g, b) RGB(r, g, b)

    typedef struct ApplicationTag {
        HWND root;
        HWND fullScreen;
        JGCOMPONENT focus;
        JGCONTAINER container;
        HANDLE thread;
        JGCOLORPALETTE palette;
        JGGRAPHICS buffer;
        char keyStates[0x100];
        int pmx;
        int pmy;
        int mx;
        int my;
    } JGAPPLICATION__, *JGAPPLICATION;

    //#include "jgcomp.h"

    LRESULT CALLBACK JGApplicationProc__(HWND, UINT, WPARAM, LPARAM);

    LRESULT CALLBACK JGFullScreenProc__(HWND, UINT, WPARAM, LPARAM);

    #define JGSetAppBounds(app, x, y, w, h) SetWindowPos((app)->root, HWND_TOP, x, y, w, h, 0)
    #define JGGetAppBounds(app, lpr) GetClientRect((app)->root, lpr)

    #define JGSetAppTitle(app, title) SetWindowText((app)->root, title)
    #define JGGetAppTitle(app, title, max_n) GetWindowText((app)->root, title, max_n)

    #define JGIsAppFocused(app) (GetFocus()==(app)->root)

    struct ClockTag;

    // a clock is destroyed by returning 0 in this function
    typedef int (*JGCLOCKFUNC)(struct ClockTag*, clock_t);

    #define JGCLOCKS_STOPPED 0x0
    #define JGCLOCKS_RUNNING 0x1
    #define JGCLOCKS_INTERRUPTED 0x2

    typedef struct ClockTag {
        HANDLE clockThread;
        JGCLOCKFUNC clockFunc;
        clock_t runtime;
        clock_t tickDelay;
    } JGCLOCK__, *JGCLOCK;

    void JGInit(int, char**);

#elif defined(__unix__) // unix systems
    #include <X11/X.h>
    #include <X11/Xlib.h>
    #include <pthread.h>
    #include <unistd.h>

    typedef struct ApplicationTag {
        Display *display;
        Window root;
        JGCONTAINER container;
        pthread_t thread;
        char keyStates[0x100];
    } JGAPPLICATION__, *JGAPPLICATION;

    #define JG_INIT
    #define JG_DISPOSE

#endif // _WIN32

#define JGIsKeyDown(app, vk) ((app)->keyStates[vk] & 0x2)
#define JGIsKeyUp(app, vk) (!((app)->keyStates[vk] & 0x2))
#define JGIsKeyToggled(app, vk) ((app)->keyStates[vk] & 0x1)

JGAPPLICATION JGCreateApplication(void);
bool JGDestroyApplication(JGAPPLICATION);

#define JGRA_WAIT 0x0
#define JGRA_CTHREAD 0x1

bool JGRunApplication(JGAPPLICATION, int);
bool JGToggleFullScreen(JGAPPLICATION);

/**
 * Creates a new thread with a clock running that calls given JGCLOCKFUNC every cycle.
 *
 * @param CLOCKFUNC - function to execute every clock cycle
 * @param clock_t   - Clock cycle duration, in milliseconds
 * @return Constructed clock
 **/
JGCLOCK JGCreateClock(JGCLOCKFUNC, clock_t);
bool JGDestroyClock(JGCLOCK);
bool JGStartClock(JGCLOCK);
bool JGInterruptClock(JGCLOCK);

#endif // __JGBASE_H__
