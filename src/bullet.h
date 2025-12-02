#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>
#include "sprites.h"
#include "enemy.h"
#include "score.h"

#define MAX_BULLETS 20
#define BULLET_WIDTH 16
#define BULLET_HEIGHT 16

typedef struct {
    int x, y;
    int active;
    int speed;
    int damage; 
} Bullet;

void bullet_init(Bullet bullets[], int size);
void bullet_shoot(Bullet bullets[], int size, int x, int y, int damage);
void bullet_update(Bullet bullets[], int size);
void bullet_render(Bullet bullets[], int size, SDL_Renderer *renderer, int score_value); 
void bullet_check_collisions(Bullet bullets[], int size, EnemyGrid *enemies, Score *score);

#endif