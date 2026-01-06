#ifndef DRAW_H
#define DRAW_H

#include "../raylib-5.5_linux_amd64/include/raylib.h"
#include "types.h"
#include "object.h"

void DrawWireframe(struct Object *object, struct Point2 *ScreenPos, Color color);

#endif
