#pragma once
#include "raylib.h"

// ---------------------------
// Layout constants
// ---------------------------
constexpr float GUI_MARGIN          = 20.0f;

constexpr float WAND_SLOT_BORDER    = 2.0f;
constexpr float WAND_SLOT_SIZE      = 80.0f;
constexpr float WAND_SLOT_SPACING   = 15.0f;
constexpr float WAND_SLOT_PADDING   = 6.0f;
constexpr Color WAND_SLOT_COLOR             = {40, 40, 40, 255};
constexpr Color WAND_SLOT_OUTLINE_COLOR     = {255, 255, 255, 255};
constexpr Color WAND_SLOT_SELECTED_COLOR    = {120, 170, 255, 255};
constexpr Color WAND_SLOT_OCCUPIED_COLOR    = {90, 90, 90, 255};

constexpr float SPELL_SLOT_SIZE     = 60.0f;
constexpr float SPELL_SLOT_SPACING  = 10.0f;
constexpr float SPELL_SLOT_BORDER   = 2.0f;
constexpr Color SPELL_SLOT_COLOR            = {30, 30, 30, 255};
constexpr Color SPELL_SLOT_OUTLINE_COLOR    = {230, 230, 230, 255};
constexpr Color SPELL_SLOT_OCCUPIED_COLOR   = {70, 70, 70, 255};

// ---------------------------
// Inventory overlays
// ---------------------------
constexpr Color INVENTORY_SHADE           = {0, 0, 0, 120};
constexpr float HELD_WAND_PANEL_PADDING   = 10.0f;
constexpr float HELD_WAND_STATS_SPACING   = 6.0f;
constexpr float HELD_WAND_VALUE_SPACING   = 10.0f;
constexpr float HELD_WAND_BETWEEN_ROWS    = 14.0f;
constexpr float HELD_WAND_TEXTURE_SIZE    = 96.0f;
constexpr float HELD_WAND_TEXTURE_GAP     = 12.0f;
constexpr float HELD_WAND_PANEL_OFFSET_Y  = 40.0f;
constexpr float HELD_WAND_PANEL_BORDER    = 2.0f;
constexpr Color HELD_WAND_PANEL_BG        = {0, 0, 0, 200};
constexpr Color HELD_WAND_PANEL_OUTLINE   = {255, 255, 255, 255};
constexpr int   HELD_WAND_FONT_SIZE       = 18;
constexpr int   HELD_WAND_PREVIEW_FONT_SIZE = 16;
constexpr Color HELD_WAND_FONT_COLOR      = {255, 255, 255, 255};
constexpr float HELD_WAND_PREVIEW_OFFSET_X = 20.0f;
constexpr float HELD_WAND_PREVIEW_WIDTH   = 420.0f;
constexpr float HELD_WAND_PREVIEW_HEIGHT  = 260.0f;
constexpr float HELD_WAND_PREVIEW_SPELL_GAP = 10.0f;
constexpr float HELD_WAND_PREVIEW_SLOT_SCALE = 0.8f;
constexpr int   HELD_WAND_PREVIEW_TITLE_SIZE = 22;
constexpr float HELD_WAND_PREVIEW_NAME_GAP = 10.0f;

// ---------------------------
// Window size (temporary)
// ---------------------------
constexpr int SCREEN_W              = 1280;
constexpr int SCREEN_H              = 720;

// ---------------------------
// Debug panel
// ---------------------------
constexpr float DEBUG_PANEL_WIDTH   = 300.0f;
constexpr float DEBUG_PANEL_HEIGHT  = 180.0f;

// ---------------------------
// Status bars
// ---------------------------
constexpr float BAR_WIDTH  = 180;
constexpr float BAR_HEIGHT = 16;
constexpr float BAR_SPACING = 12;