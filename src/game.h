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
#include "lives.h"
#include "pickup.h"
#include "highscore.h"


#define CHEMIN_VERS_SPRITE "assets/sprite.png"


typedef enum {
    GAME_STATE_MENU,            // Menu principal (NOUVEAU)
    GAME_STATE_RULES,           // Règles du jeu (NOUVEAU)
    GAME_STATE_PAUSED, 
    GAME_STATE_HIGHSCORES_VIEW, // Voir les highscores (NOUVEAU)
    GAME_STATE_NAME_INPUT,      // Saisie du nom
    GAME_STATE_PLAYING,         // En jeu
    GAME_STATE_WAVE_TRANSITION, // Entre deux vagues
    GAME_STATE_GAME_OVER        // Game over
} GameState;

typedef struct {
    SDL_Rect rect;
    const char *text;
    int selected;
} MenuButton;


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

    PickupManager pickups;

    GameState state;
    HighscoreManager highscores;
    char player_name[MAX_NAME_LENGTH];
    int name_length;
    MenuButton menu_buttons[3];
    int selected_button;
    int button_count;

    MenuButton pause_buttons[3];
    int selected_pause_button;
    int pause_button_count;

} Game;


int game_init(Game *game, const char *title, int width, int height);
void game_handle_events(Game *game);
void game_update(Game *game);
void game_render(Game *game);
void game_cleanup(Game *game);
void game_start_wave(Game *game, int wave_number);

void game_handle_name_input(Game *game, SDL_Event *event);
void game_render_name_input(Game *game);
void game_render_game_over(Game *game);

void game_handle_menu_input(Game *game, SDL_Event *event);
void game_handle_name_input(Game *game, SDL_Event *event);

void game_render_pause(Game *game);
void game_handle_pause_input(Game *game, SDL_Event *event);

#endif
