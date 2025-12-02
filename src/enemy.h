#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "sprites.h"

/* ---- Enemy types basés sur les sprites ---- */
typedef enum {
    ENEMY_SMALL_GREEN = 0,    // Petits aliens (10 HP, 10 points)
    ENEMY_SMALL_ORANGE,
    ENEMY_SMALL_RED,
    ENEMY_SMALL_YELLOW,
    ENEMY_BIG_GREEN,          // Gros aliens (20 HP, 30 points)
    ENEMY_BIG_ORANGE,
    ENEMY_BIG_GREY,
    ENEMY_BOSS,               // Boss (40 HP, 100 points)
    ENEMY_ASTEROID_SMALL,     // Petite météorite (15 HP, 5 points)
    ENEMY_ASTEROID_BIG        // Grosse météorite (30 HP, 15 points)
} EnemyType;

/* ---- Individual enemy structure ---- */ 
typedef struct {
    int x;
    int y;
    float speed;           // Vitesse de descente (pixels/frame)
    
    int alive;
    int hp;                // Points de vie actuels
    int max_hp;            // Points de vie maximum
    int score_value;       // Points donnés quand détruit
    
    int sprite_frame;      // Pour animation (0 ou 1)
    EnemyType type;
    
    int width;             // Largeur de la hitbox
    int height;            // Hauteur de la hitbox
} Enemy;

/* ---- Enemy manager ---- */ 
typedef struct {
    Enemy enemies[100];    // Maximum 100 ennemis à l'écran
    
    int count;             // Nombre total d'ennemis créés
    int alive_count;       // Nombre d'ennemis vivants
    
    int wave_number;       // Numéro de la vague actuelle
    int enemies_to_spawn;  // Ennemis restants à faire apparaître
    int asteroids_to_spawn ;
    
    Uint32 last_spawn_time;    // Dernier spawn
    Uint32 spawn_delay;        // Délai entre chaque spawn (ms)
    Uint32 animation_time;     // Timer pour animation
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