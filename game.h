#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "enemy.h"
#include "sprites.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int running;
    SpriteManager *sprite_manager;

    EnemyGrid enemies;  
} Game;


int game_init(Game *game, const char *title, int width, int height);
void game_handle_events(Game *game);
void game_update(Game *game);
void game_render(Game *game);
void game_cleanup(Game *game);

#endif
