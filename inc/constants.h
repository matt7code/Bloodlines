#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <cmath>

enum {PAUSE_MODE_NONE, PAUSE_MODE_STAIRS, PAUSE_MODE_SLEEP, PAUSE_MODE_CUTSCENE, PAUSE_MODE_DIALOG};

enum {
    DIR_SOUTH,
    DIR_WEST,
    DIR_EAST,
    DIR_NORTH
};

enum {
    CLASS_NONE,
    CLASS_WARRIOR,
    CLASS_ROGUE,
    CLASS_WIZARD,
    CLASS_CLERIC
};

#ifdef DEBUG
const unsigned int PARTY_SIZE = 7;
const unsigned int BASE_WALKING_SPEED = 150;
const unsigned int PARTY_LAG_STEPS = 12;
#else
//const unsigned int PARTY_SIZE = 5;
const unsigned int BASE_WALKING_SPEED = 150;
const unsigned int PARTY_LAG_STEPS = 12;
#endif // DEBUG

const int SCREEN_X_DEBUG = 1600;
const int SCREEN_Y_DEBUG = 900;
//const int SCREEN_X_PROFILE = 1920;
//const int SCREEN_Y_PROFILE = 1080;
//const int SCREEN_X_RELEASE = 1600;
//const int SCREEN_Y_RELEASE = 900;
const int SCREEN_X_FINAL = 1920;
const int SCREEN_Y_FINAL = 1080;
const int BPP = 32;
const int SOLID_TEXTURE_START = 121;
const int ANIMATED_TILE_TICKS_PER_SECOND = 10;
const double PI = std::acos(-1);
const float RADIANS  = PI / 180.0f;

//// Zoom 1.0 Culling
//const unsigned int VIEW_LEFT    = 992;
//const unsigned int VIEW_RIGHT   = 960;
//const unsigned int VIEW_UP      = 572;
//const unsigned int VIEW_DOWN    = 540;
//// Zoom 0.5 Culling
const unsigned int VIEW_LEFT    = 512;
const unsigned int VIEW_RIGHT   = 480;
const unsigned int VIEW_TOP     = 302;
const unsigned int VIEW_BOTTOM  = 270;

// Zoom Test Culling
//const unsigned int VIEW_LEFT    = 5000;
//const unsigned int VIEW_RIGHT   = 5000;
//const unsigned int VIEW_UP      = 5000;
//const unsigned int VIEW_DOWN    = 5000;
const float NPC_TALK_DISTANCE   = 50.0f;
const int   CLIP_DISTANCE_NPCS  = 1500;

const int ZONE_SIZE_X = 32;
const int ZONE_SIZE_Y = 20;

const int TILE_SIZE_X = 32;
const int TILE_SIZE_Y = 32;

const int CHARACTER_TILE_X = 32;
const int CHARACTER_TILE_Y = 32;
const int CHARACTER_CELLS_X = 3;
const int CHARACTER_CELLS_Y = 4;

const int NUMER_OF_CHARACTERS_PER_ROW = 4;

const int OFFSET_TO_AVOID_NEGATIVES_X = ZONE_SIZE_X * TILE_SIZE_X * 100;
const int OFFSET_TO_AVOID_NEGATIVES_Y = ZONE_SIZE_Y * TILE_SIZE_Y * 100;
const sf::Vector2f NEW_GAME_START_POSITION = sf::Vector2f(102206, 64382);

const int FPS_SAMPLE_SIZE = 60;
const int STEP_DISTANCE = 32;
const int MAP_EDIT_MOVE_SPEED = 1500;

const int NPC_SPEED_DEFAULT = 50;
const float DOUBLE_CLICK_TIME = 0.25;

const bool DEBUG_LOAD_NPC = false;


#endif // CONSTANTS_H_INCLUDED
