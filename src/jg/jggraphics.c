#include "jggraphics.h"

#include "jgbase.h"

#ifdef __WIN32 // Windows

    JGGRAPHICS JGCreateGraphics(struct ApplicationTag *app)
    {
        HDC dc = GetDC(app->root);
        JGGRAPHICS g = malloc(sizeof(JGGRAPHICS__));
        g->dc = dc;
        g->transformMatrix[0] = 1; g->transformMatrix[1] = 0; g->transformMatrix[2] = 0;
        g->transformMatrix[3] = 0; g->transformMatrix[4] = 1; g->transformMatrix[5] = 0;
        g->transformMatrix[6] = 0; // 7 unused                // 8 unused
        return g;
    }

    bool JGDestroyGraphics(struct ApplicationTag *app, JGGRAPHICS g)
    {
        if(g == NULL)
            return 0;
        ReleaseDC(app->root, g->dc);
        free(g);
        return 1;
    }

#elif defined(__unix__) // Unix systems

#elif // Mac OS

#endif
