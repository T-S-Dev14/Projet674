#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// États du jeu
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER
} GameState;

// Bouton de menu
typedef struct {
    SDL_Rect rect;
    const char *text;
    int selected;  // Au lieu de "hovered"
} MenuButton;

// Système de menu
typedef struct {
    TTF_Font *title_font;
    TTF_Font *button_font;
    TTF_Font *small_font;
    
    MenuButton buttons[3];
    int button_count;
    int selected_button;  // Index du bouton sélectionné
    
    GameState current_state;
    GameState previous_state;
    
    int final_score;  // Pour l'écran game over
} Menu;

// Fonctions publiques
int menu_init(Menu *menu, const char *fontPath);
void menu_cleanup(Menu *menu);

void menu_handle_event(Menu *menu, SDL_Event *event, int *game_running);
void menu_update(Menu *menu);
void menu_render(Menu *menu, SDL_Renderer *renderer);

// Gestion des états
void menu_set_state(Menu *menu, GameState state);
GameState menu_get_state(Menu *menu);

// Pour game over
void menu_set_final_score(Menu *menu, int score);

#endif // MENU_H