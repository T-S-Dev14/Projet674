#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>
#include "sprites.h"

#define MAX_BULLETS 20
#define BULLET_WIDTH 3
#define BULLET_HEIGHT 12

typedef struct {
    int x, y;
    int active;
    int speed;
} Bullet;

void bullet_init(Bullet bullets[], int size);
void bullet_shoot(Bullet bullets[], int size, int x, int y);
void bullet_update(Bullet bullets[], int size);
//void bullet_render(Bullet bullets[], int size, SDL_Renderer *renderer);
void bullet_render(Bullet bullets[], int size, SDL_Renderer *renderer, int score); 

#endif