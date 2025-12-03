#include "menu.h"
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Couleurs
static const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
static const SDL_Color COLOR_YELLOW = {255, 255, 0, 255};
static const SDL_Color COLOR_RED = {255, 0, 0, 255};
static const SDL_Color COLOR_GREEN = {0, 255, 0, 255};

/* ---- Initialisation ---- */
int menu_init(Menu *menu, const char *fontPath) {
    // TTF_Init() sera appelé UNE SEULE FOIS dans main.c avant tout
    
    menu->title_font = TTF_OpenFont(fontPath, 48);
    menu->button_font = TTF_OpenFont(fontPath, 32);
    menu->small_font = TTF_OpenFont(fontPath, 20);
    
    if (!menu->title_font || !menu->button_font || !menu->small_font) {
        printf("Erreur chargement polices menu: %s\n", TTF_GetError());
        return 0;
    }
    
    // Initialiser les boutons du menu principal
    menu->button_count = 3;
    menu->selected_button = 0;  // Commence sur "START"
    
    // Bouton "START"
    menu->buttons[0].rect = (SDL_Rect){300, 250, 200, 60};
    menu->buttons[0].text = "START";
    menu->buttons[0].selected = 1;  // Sélectionné par défaut
    
    // Bouton "INSTRUCTIONS"
    menu->buttons[1].rect = (SDL_Rect){300, 330, 200, 60};
    menu->buttons[1].text = "HOW TO PLAY";
    menu->buttons[1].selected = 0;
    
    // Bouton "QUIT"
    menu->buttons[2].rect = (SDL_Rect){300, 410, 200, 60};
    menu->buttons[2].text = "QUIT";
    menu->buttons[2].selected = 0;
    
    menu->current_state = STATE_MENU;
    menu->final_score = 0;
    
    return 1;
}

/* ---- Nettoyage ---- */
void menu_cleanup(Menu *menu) {
    if (menu->title_font) TTF_CloseFont(menu->title_font);
    if (menu->button_font) TTF_CloseFont(menu->button_font);
    if (menu->small_font) TTF_CloseFont(menu->small_font);
}

/* ---- Helper: Dessiner du texte centré ---- */
static void render_text_centered(SDL_Renderer *renderer, TTF_Font *font, 
                                 const char *text, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return;
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    SDL_Rect dest = {
        (SCREEN_WIDTH - surface->w) / 2,
        y,
        surface->w,
        surface->h
    };
    
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
}

/* ---- Helper: Dessiner un bouton ---- */
static void render_button(SDL_Renderer *renderer, TTF_Font *font, 
                         MenuButton *btn) {
    // Bordure (jaune si sélectionné, blanc sinon)
    SDL_Color border_color = btn->selected ? COLOR_YELLOW : COLOR_WHITE;
    SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, 
                          border_color.b, 255);
    
    // Double bordure si sélectionné pour mieux voir
    SDL_RenderDrawRect(renderer, &btn->rect);
    if (btn->selected) {
        SDL_Rect inner = {btn->rect.x + 2, btn->rect.y + 2, 
                         btn->rect.w - 4, btn->rect.h - 4};
        SDL_RenderDrawRect(renderer, &inner);
    }
    
    // Fond légèrement transparent si sélectionné
    if (btn->selected) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 50);
        SDL_RenderFillRect(renderer, &btn->rect);
    }
    
    // Texte centré dans le bouton
    SDL_Surface *surface = TTF_RenderText_Solid(font, btn->text, 
                                               btn->selected ? COLOR_YELLOW : COLOR_WHITE);
    if (!surface) return;
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    SDL_Rect text_rect = {
        btn->rect.x + (btn->rect.w - surface->w) / 2,
        btn->rect.y + (btn->rect.h - surface->h) / 2,
        surface->w,
        surface->h
    };
    
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
    SDL_DestroyTexture(texture);
}

/* ---- Gérer les événements selon l'état ---- */
void menu_handle_event(Menu *menu, SDL_Event *event, int *game_running) {
    if (event->type != SDL_KEYDOWN) return;
    
    switch (menu->current_state) {
        case STATE_MENU:
            if (event->key.keysym.sym == SDLK_UP || 
                event->key.keysym.sym == SDLK_w) {
                // Monter dans le menu
                menu->buttons[menu->selected_button].selected = 0;
                menu->selected_button--;
                if (menu->selected_button < 0) {
                    menu->selected_button = menu->button_count - 1;
                }
                menu->buttons[menu->selected_button].selected = 1;
            }
            else if (event->key.keysym.sym == SDLK_DOWN || 
                     event->key.keysym.sym == SDLK_s) {
                // Descendre dans le menu
                menu->buttons[menu->selected_button].selected = 0;
                menu->selected_button++;
                if (menu->selected_button >= menu->button_count) {
                    menu->selected_button = 0;
                }
                menu->buttons[menu->selected_button].selected = 1;
            }
            else if (event->key.keysym.sym == SDLK_RETURN || 
                     event->key.keysym.sym == SDLK_SPACE) {
                // Activer le bouton sélectionné
                if (menu->selected_button == 0) {  // START
                    menu_set_state(menu, STATE_PLAYING);
                }
                else if (menu->selected_button == 1) {  // INSTRUCTIONS
                    printf("=== INSTRUCTIONS ===\n");
                    printf("FLECHES : Déplacer le vaisseau\n");
                    printf("ESPACE : Tirer\n");
                    printf("ESC/P : Pause\n");
                    printf("Détruisez tous les ennemis pour passer à la vague suivante!\n");
                }
                else if (menu->selected_button == 2) {  // QUIT
                    *game_running = 0;
                }
            }
            break;
            
        case STATE_PLAYING:
            if (event->key.keysym.sym == SDLK_ESCAPE || 
                event->key.keysym.sym == SDLK_p) {
                menu_set_state(menu, STATE_PAUSED);
            }
            break;
            
        case STATE_PAUSED:
            if (event->key.keysym.sym == SDLK_ESCAPE || 
                event->key.keysym.sym == SDLK_p || 
                event->key.keysym.sym == SDLK_RETURN || 
                event->key.keysym.sym == SDLK_SPACE) {
                // Reprendre le jeu
                menu_set_state(menu, STATE_PLAYING);
            }
            else if (event->key.keysym.sym == SDLK_q) {
                // Retour au menu
                menu_set_state(menu, STATE_MENU);
            }
            break;
            
        case STATE_GAMEOVER:
            if (event->key.keysym.sym == SDLK_RETURN || 
                event->key.keysym.sym == SDLK_SPACE || 
                event->key.keysym.sym == SDLK_ESCAPE) {
                // Retour au menu
                menu_set_state(menu, STATE_MENU);
            }
            break;
    }
}

/* ---- Mise à jour ---- */
void menu_update(Menu *menu) {
    // Pour des animations futures
}

/* ---- Rendu selon l'état ---- */
void menu_render(Menu *menu, SDL_Renderer *renderer) {
    switch (menu->current_state) {
        case STATE_MENU:
            // Fond noir
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            // Titre
            render_text_centered(renderer, menu->title_font, 
                               "SPACE INVADERS", 80, COLOR_YELLOW);
            
            // Sous-titre
            render_text_centered(renderer, menu->small_font, 
                               "Defend Earth from alien invasion!", 
                               140, COLOR_WHITE);
            
            // Boutons
            for (int i = 0; i < menu->button_count; i++) {
                render_button(renderer, menu->button_font, &menu->buttons[i]);
            }
            
            // Instructions clavier
            render_text_centered(renderer, menu->small_font, 
                               "Use UP/DOWN arrows and ENTER to navigate", 
                               500, COLOR_GREEN);
            
            // Crédits
            render_text_centered(renderer, menu->small_font, 
                               "Created for DE1-SoC | SDL2 Game", 
                               550, COLOR_GREEN);
            break;
            
        case STATE_PAUSED:
            // Overlay semi-transparent sur le jeu
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
            SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer, &overlay);
            
            // Texte "PAUSED"
            render_text_centered(renderer, menu->title_font, 
                               "PAUSED", 200, COLOR_YELLOW);
            
            render_text_centered(renderer, menu->button_font, 
                               "Press ESC, P or ENTER to resume", 280, COLOR_WHITE);
            
            render_text_centered(renderer, menu->button_font, 
                               "Press Q to quit to menu", 330, COLOR_WHITE);
            break;
            
        case STATE_GAMEOVER:
            // Fond noir
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            // "GAME OVER"
            render_text_centered(renderer, menu->title_font, 
                               "GAME OVER", 150, COLOR_RED);
            
            // Score final
            char score_text[64];
            sprintf(score_text, "Final Score: %d", menu->final_score);
            render_text_centered(renderer, menu->button_font, 
                               score_text, 250, COLOR_YELLOW);
            
            // Instructions
            render_text_centered(renderer, menu->small_font, 
                               "Press ENTER or SPACE to return to menu", 
                               400, COLOR_WHITE);
            break;
            
        case STATE_PLAYING:
            // Le rendu du jeu est géré par game_render()
            break;
    }
    
    SDL_RenderPresent(renderer);
}

/* ---- Changer d'état ---- */
void menu_set_state(Menu *menu, GameState state) {
    menu->previous_state = menu->current_state;
    menu->current_state = state;
    
    printf("État changé: %d -> %d\n", menu->previous_state, state);
}

GameState menu_get_state(Menu *menu) {
    return menu->current_state;
}

void menu_set_final_score(Menu *menu, int score) {
    menu->final_score = score;
}