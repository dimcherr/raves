#pragma once
#include "tun/tstring.h"
#include "tun/tmath.h"
#include "comp/ccore.h"

namespace work {

void DrawAmbience();
void DrawBuffer();
void DrawTextBuffer();

void DrawLighting(const Vec& lightPos, const Quat& lightRotation, const Vec4& lightColor);

void DrawGrid();
void DrawLights();
void DrawColliders();
void DrawGColor();
void DrawInventory();
void DrawBoundingBoxes();
void DrawRaycasts();

}