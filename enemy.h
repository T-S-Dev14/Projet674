#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>


/* ----  Enemy types ---- */
typedef enum {
    ENEMY_TYPE_1 = 0,  // Top row (10 points)
    ENEMY_TYPE_2 = 1,  // Middle rows (20 points)
    ENEMY_TYPE_3 = 2   // Bottom row (30 points)
} EnemyType;


/* ---- Individual enemy structure ---- */ 
typedef struct {
    int x;
    int y;

    int alive;
    int sprite_frame;  // For animation (0 or 1)

    EnemyType type;
} Enemy;


/* ---- Enemy formation/grid ---- */ 
typedef struct {
    Enemy enemies[55];  // 11 columns x 5 rows

    int count;
    int alive_count;
    int direction;      // 1 = right, -1 = left
    int speed;
    int drop_distance;

    Uint32 last_move_time;
    Uint32 animation_time;
} EnemyGrid;



/* Function prototypes */
void enemy_init(EnemyGrid *grid, int screenWidth, int screenHeight);
void enemy_update(EnemyGrid *grid);
void enemy_render(EnemyGrid *grid, SDL_Renderer *renderer);
int enemy_check_collision(EnemyGrid *grid, int x, int y, int width, int height);
int enemy_check_reached_bottom(EnemyGrid *grid, int screenHeight);



#endif // ENEMY_H