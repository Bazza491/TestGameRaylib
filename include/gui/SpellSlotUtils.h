#pragma once

#include "raylib.h"
#include "gui/GuiConstants.h"

class Spell;

Color GetSpellColor(const Spell* spell);
void DrawSpellLabelFitted(const Spell* spell, const Rectangle& rect, int baseFontSize, Color color);
