#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "sprites.h"
#include "bullet.h"

typedef struct {
    int x, y;
    int speed;
    SpriteID current_sprite;
} Player;


void player_init(Player *p, int screenWidth, int screenHeight);
void player_update(Player *p, const Uint8 *keystates);
void player_render(Player *p, SDL_Renderer *renderer);
void player_handle_shooting(Player *p, const Uint8 *keys, Bullet bullets[]);

#endif