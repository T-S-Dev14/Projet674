#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

typedef struct {
    int x, y;
    int speed;
} Player;

void player_init(Player *p, int screenWidth, int screenHeight);
void player_update(Player *p, const Uint8 *keystates);
void player_render(Player *p, SDL_Renderer *renderer);

#endif
