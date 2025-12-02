#ifndef LIVES_H
#define LIVES_H

#include <SDL2/SDL.h>
#include "sprites.h"
#include "enemy.h"

#define MAX_LIVES 3
#define DANGER_THRESHOLD 400  // Zone de danger : y >= 450 (sur écran 600px)

typedef struct {
    int current_lives;        // Nombre de vies actuelles (0-3)
    int max_lives;            // Maximum de vies (3)
    int danger_warning;       // 1 si un ennemi est en zone de danger
    Uint32 invincibility_time; // Temps d'invincibilité après avoir perdu une vie
    Uint32 last_hit_time;     // Dernier moment où on a perdu une vie
} Lives;

void lives_init(Lives *lives);
int lives_check_danger(Lives *lives, EnemyGrid *enemies, int screenHeight);
int lives_lose_one(Lives *lives);
void lives_render(Lives *lives, SDL_Renderer *renderer);
int lives_is_game_over(Lives *lives);

#endif // LIVES_H