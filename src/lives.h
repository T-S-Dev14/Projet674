#ifndef LIVES_H
#define LIVES_H

#include <SDL2/SDL.h>
#include "sprites.h"
#include "enemy.h"  

#define MAX_LIVES 3
#define DANGER_THRESHOLD 400

typedef struct {
    int current_lives;
    int max_lives;
    int danger_warning;
    Uint32 invincibility_time;
    Uint32 last_hit_time;
} Lives;

void lives_init(Lives *lives);
int lives_check_danger(Lives *lives, EnemyGrid *enemies, int screenHeight);
int lives_lose_one(Lives *lives);
void lives_render(Lives *lives, SDL_Renderer *renderer);
int lives_is_game_over(Lives *lives);

#endif // LIVES_H