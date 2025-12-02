#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "enemy.h"
#include "sprites.h"
#include "player.h"
#include "bullet.h"
#include "score.h"
#include "text.h"

#define CHEMIN_VERS_SPRITE "assets/sprite.png"


//structure de gestion de la partie
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int running;
    SpriteManager *sprite_manager;
    EnemyGrid enemies;  
    int current_wave;           // Numéro de la vague actuelle
    int wave_transition;        // 1 si on est entre deux vague
    Uint32 wave_transition_time; // Temps de début de la transition
} Game;


int game_init(Game *game, const char *title, int width, int height);
void game_handle_events(Game *game);
void game_update(Game *game);
void game_render(Game *game);
void game_cleanup(Game *game);
void game_start_wave(Game *game, int wave_number);

#endif
