#include "../raylib-5.5_linux_amd64/include/raylib.h"
#include "types.h"

void DrawWireframe(const int Edges[][2], int EdgeCount, const struct Point2 *ScreenPos, Color color) {
    for (int e = 0; e < EdgeCount; e++) {
        int a = Edges[e][0];
        int b = Edges[e][1];
        DrawLine(
                (int)ScreenPos[a].x,
                (int)ScreenPos[a].y,
                (int)ScreenPos[b].x,
                (int)ScreenPos[b].y,
                color
                );
    }
}
