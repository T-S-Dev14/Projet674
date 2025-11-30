#include "enemy.h"
#include <string.h>


/* GLOBAL VALUES */
#define ENEMY_WIDTH 30
#define ENEMY_HEIGHT 20
#define ENEMY_SPACING_X 50
#define ENEMY_SPACING_Y 40
#define ENEMY_START_Y 50
#define ENEMY_MOVE_SPEED 500        // milliseconds between moves
#define ENEMY_ANIMATION_SPEED 250



/* ---- initialiser les enemies ----*/
void enemy_init(EnemyGrid *grid, int screenWidth, int screenHeight) {
    memset(grid, 0, sizeof(EnemyGrid));
    
    grid->direction = 1;  // Start moving right
    grid->speed = ENEMY_MOVE_SPEED;
    grid->drop_distance = 20;
    grid->count = 0;
    grid->alive_count = 0;
    grid->last_move_time = SDL_GetTicks();
    grid->animation_time = SDL_GetTicks();
    
    // Calculate starting X to center the formation
    int total_width = 11 * ENEMY_SPACING_X;
    int start_x = (screenWidth - total_width) / 2;
    
    // Create 5 rows x 11 columns
    for (int row = 0; row < 5; row++) {
        EnemyType type;
        
        // enemies type
        if (row == 0) type = ENEMY_TYPE_1;
        else if (row == 4) type = ENEMY_TYPE_3;
        else type = ENEMY_TYPE_2;

        for (int col = 0; col < 11; col++) {
            int index = row * 11 + col;

            grid->enemies[index].x = start_x + col * ENEMY_SPACING_X;
            grid->enemies[index].y = ENEMY_START_Y + row * ENEMY_SPACING_Y;
            grid->enemies[index].type = type;
            grid->enemies[index].alive = 1;
            grid->enemies[index].sprite_frame = 0;
            grid->count++;
            grid->alive_count++;
        }
    }
}



/* ---- mettre à jour les enemies ---- */
void enemy_update(EnemyGrid *grid) {
    Uint32 current_time = SDL_GetTicks();
    
    // Animation
    if (current_time - grid->animation_time > ENEMY_ANIMATION_SPEED) {
        for (int i = 0; i < grid->count; i++) {
            if (grid->enemies[i].alive) {
                grid->enemies[i].sprite_frame = 
                    (grid->enemies[i].sprite_frame + 1) % 2;
            }
        }
        grid->animation_time = current_time;
    }
    
    // Movement
    if (current_time - grid->last_move_time > grid->speed) {
        int should_drop = 0;
        int should_reverse = 0;
        
        // Check if any enemy hit the edge
        for (int i = 0; i < grid->count; i++) {
            if (!grid->enemies[i].alive) continue;
            
            int next_x = grid->enemies[i].x + (grid->direction * 10);
            
            if (next_x <= 0 || next_x >= 800 - ENEMY_WIDTH) {
                should_drop = 1;
                should_reverse = 1;
                break;
            }
        }
        
        // Move all enemies
        for (int i = 0; i < grid->count; i++) {
            if (!grid->enemies[i].alive) continue;
            
            if (should_drop) {
                grid->enemies[i].y += grid->drop_distance;
            } else {
                grid->enemies[i].x += grid->direction * 10;
            }
        }
        
        if (should_reverse) {
            grid->direction *= -1;
        }
        
        grid->last_move_time = current_time;
        
        // Speed up as enemies are destroyed
        if (grid->alive_count < 20) {
            grid->speed = 300;
        } else if (grid->alive_count < 35) {
            grid->speed = 400;
        }
    }
}



/* ---- render/images des enemies ----*/
void enemy_render(EnemyGrid *grid, SDL_Renderer *renderer) {
    for (int i = 0; i < grid->count; i++) {
        if (!grid->enemies[i].alive) continue;
        
        Enemy *e = &grid->enemies[i];
        
        // Choose color based on enemy type
        if (e->type == ENEMY_TYPE_1) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red
        } else if (e->type == ENEMY_TYPE_2) {
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);  // Orange
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Yellow
        }
        
        // Simple animation: change size slightly
        int offset = e->sprite_frame * 2;
        SDL_Rect rect = { 
            e->x + offset, 
            e->y + offset, 
            ENEMY_WIDTH - offset * 2, 
            ENEMY_HEIGHT - offset * 2 
        };
        
        SDL_RenderFillRect(renderer, &rect);
    }
}



/* ---- vérifier la collision des enemies (avec la bordure) ---- */
int enemy_check_collision(EnemyGrid *grid, int x, int y, int width, int height) {
    for (int i = 0; i < grid->count; i++) {
        if (!grid->enemies[i].alive) continue;
        
        Enemy *e = &grid->enemies[i];
        
        // AABB collision detection
        if (x < e->x + ENEMY_WIDTH &&
            x + width > e->x &&
            y < e->y + ENEMY_HEIGHT &&
            y + height > e->y) {
            
            e->alive = 0;
            grid->alive_count--;
            return i;  // Return index of hit enemy
        }
    }
    return -1;  // No collision
}


/* ---- vérifier les enemies s'ils atteignent le fond ----*/
int enemy_check_reached_bottom(EnemyGrid *grid, int screenHeight) {
    for (int i = 0; i < grid->count; i++) {
        if (!grid->enemies[i].alive) continue;
        
        if (grid->enemies[i].y + ENEMY_HEIGHT >= screenHeight - 60) {
            return 1;  // Game over!
        }
    }
    return 0;
}