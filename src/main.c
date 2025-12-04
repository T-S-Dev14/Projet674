#include "game.h"
#include "player.h"
#include "bullet.h"
#include "score.h"
#include "text.h"
#include "lives.h"
#include "menu.h"

/* === Configuration === */
#define CHEMIN_VERS_POLICE "assets/space_invaders.ttf"
#define WIDTH 800
#define HEIGHT 600

/* === Variables globales === */
Score score;
TextJeu textJeu;
Bullet bullets[MAX_BULLETS];
Lives lives;
Player player;
Menu menu;

extern SpriteManager *g_sprite_manager;

/* ===================================================================
   INITIALISATION COMPLÈTE
   =================================================================== */
int init_all(Game *game) {
    // SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return 0;
    }
    
    if (TTF_Init() < 0) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 0;
    }
    
    // Fenêtre et renderer
    game->window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    if (!game->window) {
        printf("Erreur création fenêtre\n");
        TTF_Quit();
        SDL_Quit();
        return 0;
    }
    
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer) {
        printf("Erreur création renderer\n");
        SDL_DestroyWindow(game->window);
        TTF_Quit();
        SDL_Quit();
        return 0;
    }
    
    // Sprites
    game->sprite_manager = createSpriteManager(game->renderer, CHEMIN_VERS_SPRITE);
    if (!game->sprite_manager) {
        printf("Erreur sprites\n");
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        TTF_Quit();
        SDL_Quit();
        return 0;
    }
    g_sprite_manager = game->sprite_manager;
    
    // Menu
    if (!menu_init(&menu, CHEMIN_VERS_POLICE)) {
        printf("Erreur menu\n");
        destroySpriteManager(game->sprite_manager);
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        TTF_Quit();
        SDL_Quit();
        return 0;
    }
    
    // Polices score et texte
    score.font = TTF_OpenFont(CHEMIN_VERS_POLICE, 24);
    textJeu.font = TTF_OpenFont(CHEMIN_VERS_POLICE, 24);
    if (!score.font || !textJeu.font) {
        printf("Erreur polices\n");
        return 0;
    }
    
    score.value = 0;
    game->running = 1;
    
    return 1;
}

/* ===================================================================
   NETTOYAGE COMPLET
   =================================================================== */
void cleanup_all(Game *game) {
    if (textJeu.font) TTF_CloseFont(textJeu.font);
    if (score.font) TTF_CloseFont(score.font);
    menu_cleanup(&menu);
    destroySpriteManager(game->sprite_manager);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    TTF_Quit();
    SDL_Quit();
}

/* ===================================================================
   RESET POUR NOUVELLE PARTIE
   =================================================================== */
void reset_game(Game *game) {
    score.value = 0;
    lives_init(&lives);
    player_init(&player, WIDTH, HEIGHT);
    bullet_init(bullets, MAX_BULLETS);
    text_clear(&textJeu);
    
    enemy_init(&game->enemies, WIDTH, HEIGHT);
    game->current_wave = 1;
    game->wave_transition = 0;
    game->wave_transition_time = 0;
    game_start_wave(game, 1);
    
    printf("=== NOUVELLE PARTIE ===\n");
}

/* ===================================================================
   RENDU EN PAUSE (sans Present)
   =================================================================== */
void render_paused_background(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);
    
    player_render(&player, game->renderer);
    enemy_render(&game->enemies, game->renderer);
    bullet_render(bullets, MAX_BULLETS, game->renderer, score.value);
    score_render(&score, game->renderer);
    lives_render(&lives, game->renderer);
}

/* ===================================================================
   BOUCLE DE JEU PRINCIPALE
   =================================================================== */
void game_loop(Game *game) {
    int game_initialized = 0;

    while (game->running) {
        SDL_Event event;
        
        // Événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game->running = 0;
            }
            menu_handle_event(&menu, &event, &game->running);
        }
        
        GameState state = menu_get_state(&menu);
        //printf("STATE = %d\n", state);
        
        // Logique et rendu selon l'état
        switch (state) {
            case STATE_PLAYING:
                if (!game_initialized) {
                    reset_game(game);
                    game_initialized = 1;
                }
                
                const Uint8 *keys = SDL_GetKeyboardState(NULL);
                player_update(&player, keys);
                player_handle_shooting(&player, keys, bullets, score.value);
                game_update(game);
                
                if (lives_is_game_over(&lives)) {
                    menu_set_final_score(&menu, score.value);
                    menu_set_state(&menu, STATE_GAMEOVER);
                    game_initialized = 0;
                }
                
                game_render(game);
                break;
                
            case STATE_PAUSED:
                render_paused_background(game);
                menu_render(&menu, game->renderer);
                break;
                
            case STATE_MENU:
            case STATE_GAMEOVER:
                menu_render(&menu, game->renderer);
                game_initialized = 0;
                break;
        }
        
        SDL_Delay(16);  // ~60 FPS
    }
}

/* ===================================================================
   MAIN - POINT D'ENTRÉE
   =================================================================== */
int main() {
    Game game;
    
    if (!init_all(&game)) {
        printf("Échec initialisation\n");
        return 1;
    }
    
    game_loop(&game);
    cleanup_all(&game);
    
    
    printf("Jeu fermé proprement.\n");
    return 0;
}