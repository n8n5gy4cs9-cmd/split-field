/*
 * Split-Field Game Logic
 * Two-player cooperative puzzle game for PSP
 * Player 1: D-pad controls
 * Player 2: Action buttons (ABXO) as directional controls
 */

#include "game.h"
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <string.h>
#include <stdlib.h>

#define printf pspDebugScreenPrintf

/* Helper function to draw filled rectangle */
static void draw_rect(int x, int y, int w, int h, unsigned int color)
{
    void* vram_base = NULL;
    int buffer_width = 0;
    int pixel_format = 0;
    
    if (sceDisplayGetFrameBuf(&vram_base, &buffer_width, &pixel_format, PSP_DISPLAY_SETBUF_IMMEDIATE) < 0)
        return;
    
    if (!vram_base || buffer_width <= 0) return;
    
    if (pixel_format == PSP_DISPLAY_PIXEL_FORMAT_8888) {
        unsigned int* vram = (unsigned int*)vram_base;
        for (int dy = 0; dy < h; dy++) {
            int py = y + dy;
            if (py < 0 || py >= SCREEN_HEIGHT) continue;
            for (int dx = 0; dx < w; dx++) {
                int px = x + dx;
                if (px < 0 || px >= SCREEN_WIDTH) continue;
                /* Convert ARGB to little-endian ABGR */
                unsigned int a = (color >> 24) & 0xFF;
                unsigned int r = (color >> 16) & 0xFF;
                unsigned int g = (color >> 8) & 0xFF;
                unsigned int b = color & 0xFF;
                vram[py * buffer_width + px] = (a << 24) | (b << 16) | (g << 8) | r;
            }
        }
    } else {
        /* RGB565 format */
        unsigned short* vram = (unsigned short*)vram_base;
        unsigned char r = (color >> 16) & 0xFF;
        unsigned char g = (color >> 8) & 0xFF;
        unsigned char b = color & 0xFF;
        unsigned short rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        
        for (int dy = 0; dy < h; dy++) {
            int py = y + dy;
            if (py < 0 || py >= SCREEN_HEIGHT) continue;
            for (int dx = 0; dx < w; dx++) {
                int px = x + dx;
                if (px < 0 || px >= SCREEN_WIDTH) continue;
                vram[py * buffer_width + px] = rgb565;
            }
        }
    }
}

/* Helper function to draw a tile border */
static void draw_tile_border(int x, int y, int size, unsigned int color)
{
    draw_rect(x, y, size, 1, color);           /* Top */
    draw_rect(x, y + size - 1, size, 1, color); /* Bottom */
    draw_rect(x, y, 1, size, color);           /* Left */
    draw_rect(x + size - 1, y, 1, size, color); /* Right */
}

/* Initialize game state */
void game_init(GameContext* ctx)
{
    memset(ctx, 0, sizeof(GameContext));
    
    /* Set initial player positions - on opposite sides of barrier */
    ctx->player1.x = 3;
    ctx->player1.y = 7;
    ctx->player1.color = 0xFF4444FF; /* Red */
    
    ctx->player2.x = 16;
    ctx->player2.y = 7;
    ctx->player2.color = 0xFF4444FF; /* Blue */
    
    /* Initialize simple level layout */
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            /* Create walls around the border */
            if (x == 0 || x == FIELD_WIDTH - 1 || y == 0 || y == FIELD_HEIGHT - 1) {
                ctx->field[y][x] = TILE_WALL;
            } else {
                ctx->field[y][x] = TILE_EMPTY;
            }
        }
    }
    
    /* Add vertical barrier in the middle */
    int middle_x = FIELD_WIDTH / 2;
    for (int y = 1; y < FIELD_HEIGHT - 1; y++) {
        ctx->field[y][middle_x] = TILE_BARRIER;
    }
    
    /* Add some internal walls */
    for (int i = 2; i < 7; i++) {
        ctx->field[5][i] = TILE_WALL;
        ctx->field[8][FIELD_WIDTH - 1 - i] = TILE_WALL;
    }
    
    /* Initialize mirror boxes - Player 1 controls boxes 1 & 2 */
    ctx->mirror_boxes[0].x = 3;
    ctx->mirror_boxes[0].y = 3;
    ctx->mirror_boxes[0].owner = 1;
    
    ctx->mirror_boxes[1].x = 7;
    ctx->mirror_boxes[1].y = 10;
    ctx->mirror_boxes[1].owner = 1;
    
    /* Player 2 controls boxes 3 & 4 */
    ctx->mirror_boxes[2].x = 16;
    ctx->mirror_boxes[2].y = 3;
    ctx->mirror_boxes[2].owner = 2;
    
    ctx->mirror_boxes[3].x = 12;
    ctx->mirror_boxes[3].y = 10;
    ctx->mirror_boxes[3].owner = 2;
    
    ctx->total_boxes = 4;
    
    /* Place goals */
    ctx->field[3][2] = TILE_GOAL;
    ctx->field[10][7] = TILE_GOAL;
    ctx->field[3][17] = TILE_GOAL;
    ctx->field[10][12] = TILE_GOAL;
    
    /* Initialize enemies - 2 for each player side */
    ctx->enemies[0].x = 5;
    ctx->enemies[0].y = 5;
    ctx->enemies[0].target_player = 1;
    ctx->enemies[0].active = 1;
    
    ctx->enemies[1].x = 5;
    ctx->enemies[1].y = 9;
    ctx->enemies[1].target_player = 1;
    ctx->enemies[1].active = 1;
    
    ctx->enemies[2].x = 14;
    ctx->enemies[2].y = 5;
    ctx->enemies[2].target_player = 2;
    ctx->enemies[2].active = 1;
    
    ctx->enemies[3].x = 14;
    ctx->enemies[3].y = 9;
    ctx->enemies[3].target_player = 2;
    ctx->enemies[3].active = 1;
    
    ctx->state = GAME_RUNNING;
    ctx->level = 1;
    ctx->boxes_in_goal = 0;
    ctx->enemy_move_counter = 0;
}

/* Check if position is valid for player movement */
static int can_move_to(GameContext* ctx, int x, int y, int is_player1)
{
    if (x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT)
        return 0;
    
    TileType tile = ctx->field[y][x];
    
    /* Can't move into walls, barriers, or static enemies */
    if (tile == TILE_WALL || tile == TILE_BARRIER || tile == TILE_ENEMY)
        return 0;
    
    /* Check collision with moving enemies */
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (ctx->enemies[i].active && ctx->enemies[i].x == x && ctx->enemies[i].y == y)
            return 0;
    }
    
    /* Check collision with mirror boxes */
    for (int i = 0; i < MAX_MIRROR_BOXES; i++) {
        if (ctx->mirror_boxes[i].x == x && ctx->mirror_boxes[i].y == y)
            return 0;
    }
    
    return 1;
}

/* Try to push a mirror box */
static int try_push_mirror_box(GameContext* ctx, int player_num, int from_x, int from_y, int to_x, int to_y)
{
    /* Calculate push direction */
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    
    /* Find which mirror box is at the push location */
    int box_idx = -1;
    for (int i = 0; i < MAX_MIRROR_BOXES; i++) {
        if (ctx->mirror_boxes[i].x == to_x && ctx->mirror_boxes[i].y == to_y) {
            /* Check if this player can move this box */
            if (ctx->mirror_boxes[i].owner == player_num) {
                box_idx = i;
                break;
            } else {
                return 0; /* Can't move opponent's box */
            }
        }
    }
    
    if (box_idx == -1)
        return 0;
    
    /* Check destination for box */
    int box_dest_x = to_x + dx;
    int box_dest_y = to_y + dy;
    
    if (box_dest_x < 0 || box_dest_x >= FIELD_WIDTH || 
        box_dest_y < 0 || box_dest_y >= FIELD_HEIGHT)
        return 0;
    
    TileType dest_tile = ctx->field[box_dest_y][box_dest_x];
    
    /* Can only push box into empty space or goal */
    if (dest_tile != TILE_EMPTY && dest_tile != TILE_GOAL)
        return 0;
    
    /* Check no other box at destination */
    for (int i = 0; i < MAX_MIRROR_BOXES; i++) {
        if (ctx->mirror_boxes[i].x == box_dest_x && ctx->mirror_boxes[i].y == box_dest_y)
            return 0;
    }
    
    /* Move the box and its mirror */
    ctx->mirror_boxes[box_idx].x = box_dest_x;
    ctx->mirror_boxes[box_idx].y = box_dest_y;
    
    /* Mirror movement: find the paired box and move it the same way */
    /* Player 1's boxes 0,1 mirror to each other */
    /* Player 2's boxes 2,3 mirror to each other */
    int mirror_idx = -1;
    if (player_num == 1) {
        mirror_idx = (box_idx == 0) ? 1 : 0;
    } else {
        mirror_idx = (box_idx == 2) ? 3 : 2;
    }
    
    /* Move mirror box in same direction */
    int mirror_dest_x = ctx->mirror_boxes[mirror_idx].x + dx;
    int mirror_dest_y = ctx->mirror_boxes[mirror_idx].y + dy;
    
    /* Check if mirror destination is valid */
    if (mirror_dest_x >= 0 && mirror_dest_x < FIELD_WIDTH &&
        mirror_dest_y >= 0 && mirror_dest_y < FIELD_HEIGHT) {
        TileType mirror_tile = ctx->field[mirror_dest_y][mirror_dest_x];
        if (mirror_tile == TILE_EMPTY || mirror_tile == TILE_GOAL) {
            /* Check no box collision */
            int collision = 0;
            for (int i = 0; i < MAX_MIRROR_BOXES; i++) {
                if (ctx->mirror_boxes[i].x == mirror_dest_x && ctx->mirror_boxes[i].y == mirror_dest_y) {
                    collision = 1;
                    break;
                }
            }
            if (!collision) {
                ctx->mirror_boxes[mirror_idx].x = mirror_dest_x;
                ctx->mirror_boxes[mirror_idx].y = mirror_dest_y;
            }
        }
    }
    
    return 1;
}

/* Move enemies toward their target player */
static void update_enemies(GameContext* ctx)
{
    ctx->enemy_move_counter++;
    
    /* Enemies move every 15 frames (slow movement) */
    if (ctx->enemy_move_counter < 15)
        return;
    
    ctx->enemy_move_counter = 0;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!ctx->enemies[i].active)
            continue;
        
        Enemy* enemy = &ctx->enemies[i];
        Player* target = (enemy->target_player == 1) ? &ctx->player1 : &ctx->player2;
        
        /* Simple pathfinding: move toward player */
        int dx = 0, dy = 0;
        
        if (target->x < enemy->x)
            dx = -1;
        else if (target->x > enemy->x)
            dx = 1;
        
        if (target->y < enemy->y)
            dy = -1;
        else if (target->y > enemy->y)
            dy = 1;
        
        /* Try to move horizontally first */
        int new_x = enemy->x + dx;
        int new_y = enemy->y;
        
        if (new_x >= 0 && new_x < FIELD_WIDTH) {
            TileType tile = ctx->field[new_y][new_x];
            if (tile == TILE_EMPTY || tile == TILE_GOAL) {
                /* Check no collision with other enemies or boxes */
                int collision = 0;
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (j != i && ctx->enemies[j].active && 
                        ctx->enemies[j].x == new_x && ctx->enemies[j].y == new_y) {
                        collision = 1;
                        break;
                    }
                }
                for (int j = 0; j < MAX_MIRROR_BOXES; j++) {
                    if (ctx->mirror_boxes[j].x == new_x && ctx->mirror_boxes[j].y == new_y) {
                        collision = 1;
                        break;
                    }
                }
                if (!collision) {
                    enemy->x = new_x;
                    return;
                }
            }
        }
        
        /* Try vertical if horizontal failed */
        new_x = enemy->x;
        new_y = enemy->y + dy;
        
        if (new_y >= 0 && new_y < FIELD_HEIGHT) {
            TileType tile = ctx->field[new_y][new_x];
            if (tile == TILE_EMPTY || tile == TILE_GOAL) {
                int collision = 0;
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (j != i && ctx->enemies[j].active && 
                        ctx->enemies[j].x == new_x && ctx->enemies[j].y == new_y) {
                        collision = 1;
                        break;
                    }
                }
                for (int j = 0; j < MAX_MIRROR_BOXES; j++) {
                    if (ctx->mirror_boxes[j].x == new_x && ctx->mirror_boxes[j].y == new_y) {
                        collision = 1;
                        break;
                    }
                }
                if (!collision) {
                    enemy->y = new_y;
                }
            }
        }
    }
}

/* Update game logic */
void game_update(GameContext* ctx, SceCtrlData* pad)
{
    static SceCtrlData oldpad = {0};
    static int move_delay = 0;
    
    if (ctx->state != GAME_RUNNING)
        return;
    
    /* Add delay between moves */
    if (move_delay > 0) {
        move_delay--;
        oldpad = *pad;
        return;
    }
    
    int p1_dx = 0, p1_dy = 0;
    int p2_dx = 0, p2_dy = 0;
    
    /* Player 1 controls: D-pad */
    if ((pad->Buttons & PSP_CTRL_UP) && !(oldpad.Buttons & PSP_CTRL_UP))
        p1_dy = -1;
    if ((pad->Buttons & PSP_CTRL_DOWN) && !(oldpad.Buttons & PSP_CTRL_DOWN))
        p1_dy = 1;
    if ((pad->Buttons & PSP_CTRL_LEFT) && !(oldpad.Buttons & PSP_CTRL_LEFT))
        p1_dx = -1;
    if ((pad->Buttons & PSP_CTRL_RIGHT) && !(oldpad.Buttons & PSP_CTRL_RIGHT))
        p1_dx = 1;
    
    /* Player 2 controls: Action buttons (ABXO mapped as directions) */
    if ((pad->Buttons & PSP_CTRL_TRIANGLE) && !(oldpad.Buttons & PSP_CTRL_TRIANGLE))
        p2_dy = -1;  /* Triangle = Up */
    if ((pad->Buttons & PSP_CTRL_CROSS) && !(oldpad.Buttons & PSP_CTRL_CROSS))
        p2_dy = 1;   /* Cross = Down */
    if ((pad->Buttons & PSP_CTRL_SQUARE) && !(oldpad.Buttons & PSP_CTRL_SQUARE))
        p2_dx = -1;  /* Square = Left */
    if ((pad->Buttons & PSP_CTRL_CIRCLE) && !(oldpad.Buttons & PSP_CTRL_CIRCLE))
        p2_dx = 1;   /* Circle = Right */
    
    /* Move Player 1 */
    if (p1_dx != 0 || p1_dy != 0) {
        int new_x = ctx->player1.x + p1_dx;
        int new_y = ctx->player1.y + p1_dy;
        
        /* Check if moving into moving enemy - instant death */
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (ctx->enemies[i].active && ctx->enemies[i].x == new_x && ctx->enemies[i].y == new_y) {
                ctx->state = GAME_LOSE;
                return;
            }
        }
        
        /* Check for mirror box push */
        int is_mirror_box = 0;
        for (int i = 0; i < MAX_MIRROR_BOXES; i++) {
            if (ctx->mirror_boxes[i].x == new_x && ctx->mirror_boxes[i].y == new_y) {
                is_mirror_box = 1;
                if (try_push_mirror_box(ctx, 1, ctx->player1.x, ctx->player1.y, new_x, new_y)) {
                    ctx->player1.x = new_x;
                    ctx->player1.y = new_y;
                    move_delay = 5;
                }
                break;
            }
        }
        
        if (!is_mirror_box && can_move_to(ctx, new_x, new_y, 1)) {
            /* Check collision with player 2 */
            if (new_x != ctx->player2.x || new_y != ctx->player2.y) {
                ctx->player1.x = new_x;
                ctx->player1.y = new_y;
                move_delay = 5;
            }
        }
    }
    
    /* Move Player 2 */
    if (p2_dx != 0 || p2_dy != 0) {
        int new_x = ctx->player2.x + p2_dx;
        int new_y = ctx->player2.y + p2_dy;
        
        /* Check if moving into moving enemy - instant death */
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (ctx->enemies[i].active && ctx->enemies[i].x == new_x && ctx->enemies[i].y == new_y) {
                ctx->state = GAME_LOSE;
                return;
            }
        }
        
        /* Check for mirror box push */
        int is_mirror_box = 0;
        for (int i = 0; i < MAX_MIRROR_BOXES; i++) {
            if (ctx->mirror_boxes[i].x == new_x && ctx->mirror_boxes[i].y == new_y) {
                is_mirror_box = 1;
                if (try_push_mirror_box(ctx, 2, ctx->player2.x, ctx->player2.y, new_x, new_y)) {
                    ctx->player2.x = new_x;
                    ctx->player2.y = new_y;
                    move_delay = 5;
                }
                break;
            }
        }
        
        if (!is_mirror_box && can_move_to(ctx, new_x, new_y, 0)) {
            /* Check collision with player 1 */
            if (new_x != ctx->player1.x || new_y != ctx->player1.y) {
                ctx->player2.x = new_x;
                ctx->player2.y = new_y;
                move_delay = 5;
            }
        }
    }
    
    /* Update enemy positions */
    update_enemies(ctx);
    
    /* Check if player collides with enemy after enemy movement */
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (ctx->enemies[i].active) {
            if ((ctx->enemies[i].x == ctx->player1.x && ctx->enemies[i].y == ctx->player1.y) ||
                (ctx->enemies[i].x == ctx->player2.x && ctx->enemies[i].y == ctx->player2.y)) {
                ctx->state = GAME_LOSE;
                return;
            }
        }
    }
    
    /* Count boxes in goals */
    ctx->boxes_in_goal = 0;
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (ctx->field[y][x] == TILE_GOAL) {
                /* Check if a box is on this goal */
                /* (In this simple version, we'll just check if all boxes are placed) */
            }
        }
    }
    
    /* Check for SELECT button to quit (press, not hold) */
    if ((pad->Buttons & PSP_CTRL_SELECT) && !(oldpad.Buttons & PSP_CTRL_SELECT)) {
        ctx->state = GAME_QUIT;
    }
    
    oldpad = *pad;
}

/* Render game graphics */
void game_render(GameContext* ctx)
{
    pspDebugScreenClear();
    
    /* Draw title at top */
    pspDebugScreenSetXY(15, 0);
    printf("SPLIT-FIELD - Level %d", ctx->level);
    
    /* Draw field */
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            int screen_x = FIELD_OFFSET_X + (x * TILE_SIZE);
            int screen_y = FIELD_OFFSET_Y + (y * TILE_SIZE);
            
            unsigned int color = 0xFF202020; /* Dark gray background */
            
            switch (ctx->field[y][x]) {
                case TILE_EMPTY:
                    color = 0xFF101010;
                    break;
                case TILE_WALL:
                    color = 0xFF666666;
                    break;
                case TILE_BOX:
                    color = 0xFF996633; /* Brown */
                    break;
                case TILE_GHOST_BOX:
                    color = 0xFF9966FF; /* Purple */
                    break;
                case TILE_ENEMY:
                    color = 0xFFFF0000; /* Red */
                    break;
                case TILE_GOAL:
                    color = 0xFF00FF00; /* Green */
                    break;
                case TILE_BARRIER:
                    color = 0xFFFFFF00; /* Yellow barrier */
                    break;
            }
            
            draw_rect(screen_x, screen_y, TILE_SIZE, TILE_SIZE, color);
            
            /* Draw tile border for better visibility */
            if (ctx->field[y][x] != TILE_EMPTY) {
                draw_tile_border(screen_x, screen_y, TILE_SIZE, 0xFF000000);
            }
        }
    }
    
    /* Draw mirror boxes */
    for (int i = 0; i < MAX_MIRROR_BOXES; i++) {
        int screen_x = FIELD_OFFSET_X + (ctx->mirror_boxes[i].x * TILE_SIZE);
        int screen_y = FIELD_OFFSET_Y + (ctx->mirror_boxes[i].y * TILE_SIZE);
        
        /* Color based on owner */
        unsigned int box_color = (ctx->mirror_boxes[i].owner == 1) ? 0xFFFF8844 : 0xFF4488FF;
        draw_rect(screen_x, screen_y, TILE_SIZE, TILE_SIZE, box_color);
        draw_tile_border(screen_x, screen_y, TILE_SIZE, 0xFF000000);
    }
    
    /* Draw moving enemies */
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (ctx->enemies[i].active) {
            int screen_x = FIELD_OFFSET_X + (ctx->enemies[i].x * TILE_SIZE);
            int screen_y = FIELD_OFFSET_Y + (ctx->enemies[i].y * TILE_SIZE);
            
            /* Pulsing red for moving enemies */
            draw_rect(screen_x + 2, screen_y + 2, TILE_SIZE - 4, TILE_SIZE - 4, 0xFFFF0000);
            draw_tile_border(screen_x + 2, screen_y + 2, TILE_SIZE - 4, 0xFF880000);
        }
    }
    
    /* Draw players */
    int p1_screen_x = FIELD_OFFSET_X + (ctx->player1.x * TILE_SIZE);
    int p1_screen_y = FIELD_OFFSET_Y + (ctx->player1.y * TILE_SIZE);
    draw_rect(p1_screen_x + 2, p1_screen_y + 2, TILE_SIZE - 4, TILE_SIZE - 4, 0xFFFF4444);
    draw_tile_border(p1_screen_x + 2, p1_screen_y + 2, TILE_SIZE - 4, 0xFF000000);
    
    int p2_screen_x = FIELD_OFFSET_X + (ctx->player2.x * TILE_SIZE);
    int p2_screen_y = FIELD_OFFSET_Y + (ctx->player2.y * TILE_SIZE);
    draw_rect(p2_screen_x + 2, p2_screen_y + 2, TILE_SIZE - 4, TILE_SIZE - 4, 0xFF4444FF);
    draw_tile_border(p2_screen_x + 2, p2_screen_y + 2, TILE_SIZE - 4, 0xFF000000);
    
    /* Draw instructions at bottom */
    pspDebugScreenSetXY(2, 32);
    printf("P1(Red) D-PAD | P2(Blue) ABXO | YELLOW=Barrier");
    pspDebugScreenSetXY(2, 33);
    printf("Orange boxes=P1 | Blue boxes=P2 | MIRROR MOVES!");
    pspDebugScreenSetXY(2, 34);
    printf("Push to GREEN goals | Avoid moving RED enemies | SELECT:Quit");
    
    /* Draw game state messages */
    if (ctx->state == GAME_WIN) {
        pspDebugScreenSetXY(20, 15);
        printf("*** LEVEL COMPLETE! ***");
    } else if (ctx->state == GAME_LOSE) {
        pspDebugScreenSetXY(22, 15);
        printf("*** GAME OVER! ***");
    }
}

/* Main game loop */
void game_run(GameContext* ctx)
{
    SceCtrlData pad;
    
    while (ctx->state == GAME_RUNNING) {
        sceCtrlReadBufferPositive(&pad, 1);
        game_update(ctx, &pad);
        game_render(ctx);
        sceDisplayWaitVblankStart();
    }
    
    /* Show end screen briefly */
    if (ctx->state == GAME_WIN || ctx->state == GAME_LOSE) {
        game_render(ctx);
        sceDisplayWaitVblankStart();
        
        /* Wait for any button to return to menu */
        while (1) {
            sceCtrlReadBufferPositive(&pad, 1);
            if (pad.Buttons) {
                break;
            }
            sceDisplayWaitVblankStart();
        }
    }
}

/* Cleanup game resources */
void game_cleanup(GameContext* ctx)
{
    /* Nothing to cleanup in this simple version */
    (void)ctx;
}
