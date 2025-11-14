/*
 * Split-Field Game Header
 * Two-player cooperative puzzle game for PSP
 */

#ifndef GAME_H
#define GAME_H

#include <pspkernel.h>
#include <pspctrl.h>

/* Game constants */
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272
#define FIELD_WIDTH 20
#define FIELD_HEIGHT 14
#define TILE_SIZE 16
#define FIELD_OFFSET_X ((SCREEN_WIDTH - (FIELD_WIDTH * TILE_SIZE)) / 2)
#define FIELD_OFFSET_Y ((SCREEN_HEIGHT - (FIELD_HEIGHT * TILE_SIZE)) / 2)

/* Game states */
typedef enum {
    GAME_RUNNING,
    GAME_WIN,
    GAME_LOSE,
    GAME_QUIT
} GameState;

/* Entity types */
typedef enum {
    TILE_EMPTY = 0,
    TILE_WALL,
    TILE_BOX,
    TILE_GHOST_BOX,  /* Box that only one player can move */
    TILE_ENEMY,
    TILE_GOAL,
    TILE_BARRIER     /* Vertical barrier in middle */
} TileType;

/* Player structure */
typedef struct {
    int x;
    int y;
    int color;  /* For visual distinction */
} Player;

/* Enemy structure */
typedef struct {
    int x;
    int y;
    int target_player;  /* 1 or 2 */
    int active;
} Enemy;

/* Mirror Box structure */
typedef struct {
    int x;
    int y;
    int owner;  /* 1 or 2 - which player controls it */
} MirrorBox;

#define MAX_ENEMIES 4
#define MAX_MIRROR_BOXES 4

/* Game context */
typedef struct {
    Player player1;  /* Controlled by D-pad */
    Player player2;  /* Controlled by ABXO buttons */
    TileType field[FIELD_HEIGHT][FIELD_WIDTH];
    Enemy enemies[MAX_ENEMIES];
    MirrorBox mirror_boxes[MAX_MIRROR_BOXES];
    GameState state;
    int level;
    int boxes_in_goal;
    int total_boxes;
    int enemy_move_counter;  /* For slow enemy movement */
} GameContext;

/* Function prototypes */
void game_init(GameContext* ctx);
void game_run(GameContext* ctx);
void game_update(GameContext* ctx, SceCtrlData* pad);
void game_render(GameContext* ctx);
void game_cleanup(GameContext* ctx);

#endif /* GAME_H */
