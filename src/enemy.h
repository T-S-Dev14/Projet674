#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "sprites.h"

/* ---- Enemy types basés sur les sprites ---- */
typedef enum {
    ENEMY_SMALL_GREEN = 0,
    ENEMY_SMALL_ORANGE,
    ENEMY_SMALL_RED,
    ENEMY_SMALL_YELLOW,
    ENEMY_BIG_GREEN,
    ENEMY_BIG_ORANGE,
    ENEMY_BIG_GREY,
    ENEMY_BOSS,
    ENEMY_ASTEROID_SMALL,
    ENEMY_ASTEROID_BIG
} EnemyType;

/* ---- Individual enemy structure ---- */ 
typedef struct {
    int x;
    int y;
    float speed;
    
    int alive;
    int hp;
    int max_hp;
    int score_value;
    
    int sprite_frame;
    EnemyType type;
    
    int width;
    int height;
} Enemy;

/* ---- Enemy manager ---- */ 
typedef struct {
    Enemy enemies[100];
    
    int count;
    int alive_count;
    
    int wave_number;
    int enemies_to_spawn;
    int asteroids_to_spawn;
    
    Uint32 last_spawn_time;
    Uint32 spawn_delay;
    Uint32 animation_time;
    
    // NOUVEAU : Pour spawn de pickups
    int has_pending_pickup;
    int pending_pickup_x;
    int pending_pickup_y;
    int pending_pickup_type;  // On utilise int au lieu de PickupType pour éviter la dépendance
} EnemyGrid;

/* Function prototypes */
void enemy_init(EnemyGrid *grid, int screenWidth, int screenHeight);
void enemy_start_wave(EnemyGrid *grid, int wave_number);
void enemy_spawn_one(EnemyGrid *grid, int screenWidth);
void enemy_update(EnemyGrid *grid, int screenWidth);
void enemy_render(EnemyGrid *grid, SDL_Renderer *renderer);
int enemy_check_collision(EnemyGrid *grid, int x, int y, int width, int height, int damage);
int enemy_check_reached_bottom(EnemyGrid *grid, int screenHeight);

#endif // ENEMY_H