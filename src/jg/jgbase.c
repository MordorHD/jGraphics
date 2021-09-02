#include "jgbase.h"

#ifdef __WIN32

    ///////////////////////////////
    //        APPLICATION        //
    ///////////////////////////////
    JGAPPLICATION JGCreateApplication(JGCOLORPALETTE palette)
    {
        JGAPPLICATION app = malloc(sizeof(JGAPPLICATION__));
        app->root = CreateWindow("JG_APP_WINDOW", "Title", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), app);
        app->fullScreen = NULL;
        app->container = malloc(sizeof(JGCONTAINER__));
        app->container->childCnt = 0;
        app->container->children = NULL;
        app->palette = palette;
        JGIMAGE__ image;
        image.width = 0;
        image.height = 0;
        image.pixels = NULL;
        app->buffer = JGCreateGraphics(&image, palette);
        return app;
    }

    bool JGDestroyApplication(JGAPPLICATION app)
    {
        if(app == NULL)
            return 0;
        JGDestroyGraphics(app->buffer);
        DestroyWindow(app->root);
        DestroyWindow(app->fullScreen);
        free(app);
        return 1;
    }

    static inline void WinTranslateProc(JGEVENT *event, JGAPPLICATION app, UINT msg, WPARAM wParam, LPARAM lParam, DWORD time)
    {
        event->time = time;
        switch(msg)
        {
        case WM_SETFOCUS:
            event->type = JGEVENT_ID_FOCUSGAINED;
            break;
        case WM_KILLFOCUS:
            event->type = JGEVENT_ID_FOCUSLOST;
            break;
        case WM_KEYDOWN:
        {
            event->type = JGEVENT_ID_KEYPRESSED;
            event->vKey = wParam;
            event->keyChar = MapVirtualKey(wParam, MAPVK_VK_TO_CHAR);
            event->flags = lParam;
            break;
        }
        case WM_KEYUP:
        {
            event->type = JGEVENT_ID_KEYRELEASED;
            event->vKey = wParam;
            event->keyChar = MapVirtualKey(wParam, MAPVK_VK_TO_CHAR);
            event->flags = lParam;
            break;
        }
        case WM_CHAR:
        {
            event->type = JGEVENT_ID_KEYTYPED;
            event->vKey = wParam;
            event->keyChar = wParam;
            event->flags = lParam;
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        {
            event->type = JGEVENT_ID_MOUSEPRESSED;
            event->mousePos.x = GET_X_LPARAM(lParam);
            event->mousePos.y = GET_Y_LPARAM(lParam);
            event->mouseButton = LOWORD(wParam);
            event->pressedButton = msg;
            break;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            event->type = JGEVENT_ID_MOUSERELEASED;
            event->mousePos.x = GET_X_LPARAM(lParam);
            event->mousePos.y = GET_Y_LPARAM(lParam);
            event->mouseButton = LOWORD(wParam);
            event->pressedButton = msg;
            break;
        }
        case WM_MOUSEWHEEL:
        {
            event->type = JGEVENT_ID_MOUSEWHEEL;
            event->mousePos.x = GET_X_LPARAM(lParam);
            event->mousePos.y = GET_Y_LPARAM(lParam);
            event->mouseButton = LOWORD(wParam);
            event->deltaWheel = GET_WHEEL_DELTA_WPARAM(wParam);
            break;
        }
        case WM_MOUSEMOVE:
        {
            event->type = JGEVENT_ID_MOUSEMOVED;
            app->pmx = app->mx;
            app->pmy = app->my;
            event->mousePos.x = app->mx = GET_X_LPARAM(lParam);
            event->mousePos.y = app->my = GET_Y_LPARAM(lParam);
            event->dx = app->mx - app->pmx;
            event->dy = app->my - app->pmy;
            event->mouseButton = LOWORD(wParam);
            break;
        }
        default:
            event->type = 0;
        }
    }

    static DWORD WINAPI JGThreadProc__(_In_ LPVOID lpParameter)
    {
        JGAPPLICATION app = (JGAPPLICATION) lpParameter;
        MSG msg;
        while(GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        TerminateThread(app->thread, 0);
        return 0;
    }

    bool JGRunApplication(JGAPPLICATION app, int flags)
    {
        if(flags & JGRA_CTHREAD)
        {
            DWORD id;
            if(!(app->thread = CreateThread(NULL, 0, JGThreadProc__, app, 0, &id)))
                return 0;
        }
        else
        {
            ShowWindow(app->root, SW_NORMAL);
            UpdateWindow(app->root);
            MSG msg;
            JGEVENT event;
            // win message loop
            while(GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                // translate to jg event
                WinTranslateProc(&event, app, msg.message, msg.wParam, msg.lParam, msg.time);
                // if type == 0, then there is no need to forward it
                if(!event.type)
                    continue;
                // forward event to all children
                int cnt = app->container->childCnt;
                JGCOMPONENT *chn = app->container->children;
                short st = 0;
                while(cnt--)
                {
                    st |= event.type >= JGEVENT_FORWARD_MIN && event.type <= JGEVENT_FORWARD_MAX ? JGDispatchEventAndForward(*chn, &event) :
                                                                                                   JGDispatchEvent(*chn, &event);
                    chn++;
                }
                if(st & JGCOMP_STATE_REDRAW)
                    RedrawWindow(app->root, NULL, NULL, RDW_INVALIDATE);
            }
        }
        return 1;
    }

    LRESULT CALLBACK JGApplicationProc__(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        JGAPPLICATION app = (JGAPPLICATION) GetWindowLongPtr(hWnd, GWLP_USERDATA);
        if(msg == WM_CREATE)
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) ((CREATESTRUCT*) lParam)->lpCreateParams);
        if(app == NULL)
            return DefWindowProc(hWnd, msg, wParam, lParam);
        switch(msg)
        {
        case WM_SIZE:
        {
            JGCONTAINER cont = app->container;
            JGLAYOUT layout = cont->layout;
            JGGRAPHICS buffer = app->buffer;
            RECT r;
            GetClientRect(hWnd, &r);
            buffer->image.width = r.right;
            buffer->image.height = r.bottom;
            buffer->image.pixels = realloc(buffer->image.pixels, sizeof(int) * (r.right * r.bottom));
            if(layout == NULL)
                break;
            layout->layoutFunc(layout, cont->children, cont->childCnt, 0, 0, r.right, r.bottom);
            break;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            JGGRAPHICS g = app->buffer;
            JGSetFillColor(g, app->palette->bgC0Color);
            JGSetStrokeColor(g, app->palette->fgColor);

            int cnt = app->container->childCnt;
            JGCOMPONENT *chn = app->container->children;
            while(cnt--)
            {
                JGRECT rect = { .x = ps.rcPaint.left, .y = ps.rcPaint.top, .width = ps.rcPaint.right - ps.rcPaint.left, .height = ps.rcPaint.bottom - ps.rcPaint.top };
                if(JGRectsInstersect(&(*chn)->rect, &rect))
                    JGRedrawComponent(*chn, g, 0);
                chn++;
            }

            JGIMAGE__ image = g->image;
            HBITMAP hbmp = CreateBitmap(image.width, image.height, 1, sizeof(color_t) * 8, image.pixels);

            HDC dc = CreateCompatibleDC(hdc);
            HBITMAP oldBmp = SelectObject(dc, hbmp);
            BitBlt(hdc, 0, 0, image.width, image.height, dc, 0, 0, SRCCOPY);
            SelectObject(dc, oldBmp);
            DeleteDC(dc);

            DeleteObject(hbmp);

            EndPaint(hWnd, &ps);
            break;
        }
        case WM_KEYDOWN:
            app->keyStates[wParam] |= 0x2;
            break;
        case WM_KEYUP:
            app->keyStates[wParam] ^= 0x3;
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        return 0;
    }

    LRESULT CALLBACK JGFullScreenProc__(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        JGAPPLICATION app = (JGAPPLICATION) GetWindowLongPtr(hWnd, GWLP_USERDATA);
        if(msg == WM_CREATE)
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) ((CREATESTRUCT*) lParam)->lpCreateParams);
        if(app == NULL)
            return DefWindowProc(hWnd, msg, wParam, lParam);
        switch(msg)
        {
        case WM_NCDESTROY:
        {

            break;
        }
        case WM_KEYDOWN:
        {
            if(wParam == VK_F11)
                JGToggleFullScreen(app);
            break;
        }
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        return 0;
    }

    bool JGToggleFullScreen(JGAPPLICATION app)
    {
        if(!app->fullScreen)
        {
            HMONITOR hmon = MonitorFromWindow(app->root, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(MONITORINFO) };
            if(!GetMonitorInfo(hmon, &mi))
                return 0;
            app->fullScreen = CreateWindow("JG_APP_FULL_SCREEN_WINDOW", "", WS_POPUP | WS_VISIBLE,
                mi.rcMonitor.left,
                mi.rcMonitor.top,
                mi.rcMonitor.right - mi.rcMonitor.left,
                mi.rcMonitor.bottom - mi.rcMonitor.top, app->root, NULL, GetModuleHandle(NULL), app);
        }
        else
        {
            DestroyWindow(app->fullScreen);
            app->fullScreen = NULL;
        }
        return 1;
    }

    /////////////////////////
    //        CLOCK        //
    /////////////////////////

    static DWORD WINAPI JGClockProc__(_In_ LPVOID lpParameter)
    {
        JGCLOCK cl = (JGCLOCK) lpParameter;
        clock_t creationTime = clock();
        clock_t timeStamp = creationTime;
        clock_t lastTimeStamp;
        while(1)
        {
            lastTimeStamp = timeStamp;
            timeStamp = clock();
            cl->runtime = timeStamp - creationTime;
            if(!cl->clockFunc(cl, timeStamp - lastTimeStamp))
            {
                free(cl);
                return 0;
            }
            while(clock() - timeStamp < cl->tickDelay)
                Sleep(1);
        }
        return 0;
    }

    JGCLOCK JGCreateClock(JGCLOCKFUNC clockFunc, clock_t tickDelay)
    {
        JGCLOCK cl = malloc(sizeof(JGCLOCK__));
        DWORD id;
        if(!(cl->clockThread = CreateThread(NULL, 0, JGClockProc__, cl, CREATE_SUSPENDED, &id)))
        {
            free(cl);
            return NULL;
        }
        cl->clockFunc = clockFunc;
        cl->tickDelay = tickDelay * CLOCKS_PER_SEC / 1000;
        return cl;
    }

    bool JGStartClock(JGCLOCK clock)
    {
        if(clock == NULL)
            return 0;
        ResumeThread(clock->clockThread);
        return 1;
    }

    bool JGInterruptClock(JGCLOCK clock)
    {
        if(clock == NULL)
            return 0;
        SuspendThread(clock->clockThread);
        return 1;
    }

    bool JGDestroyClock(JGCLOCK clock)
    {
        if(clock == NULL)
            return 0;
        WINBOOL b = TerminateThread(clock->clockThread, 0);
        free(clock);
        return b;
    }


#endif // __WIN32
