#ifndef DRAW_H
#define DRAW_H

#include "../raylib-5.5_linux_amd64/include/raylib.h"
#include "types.h"

void DrawWireframe(const int Edges[][2], int EdgeCount, const struct Point2 *ScreenPos, Color color);

#endif
