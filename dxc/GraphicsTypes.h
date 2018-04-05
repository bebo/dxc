#pragma once

#include "stdafx.h"

typedef struct COLOR { FLOAT r, g, b, a; } COLOR;
typedef struct TEXTURECOORD { FLOAT u, v; } TEXTURECOORD;
typedef struct VERTEX { FLOAT X, Y, Z; TEXTURECOORD texCoord; } VERTEX;