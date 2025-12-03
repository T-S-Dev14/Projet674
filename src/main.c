#include "game.h"
#include "player.h"
#include "bullet.h"
#include "score.h"
#include "text.h"
#include "lives.h"
#include "menu.h"

/* === configuration générale === */
#define CHEMIN_VERS_POLICE "assets/space_invaders.ttf"
#define WIDTH 800
#define HEIGHT 600

/** === objets partagés / globaux === */
Score score;
TextJeu textJeu;
Bullet bullets[MAX_BULLETS];
Lives lives;
Player player;
Menu menu;

// Déclaration externe du sprite manager global (défini dans game.c)
extern SpriteManager *g_sprite_manager;

/* ---- Réinitialiser le jeu pour une nouvelle partie ---- */
void reset_game(Game *game) {
    // Reset score
    score.value = 0;
    
    // Reset lives
    lives_init(&lives);
    
    // Reset player
    player_init(&player, WIDTH, HEIGHT);
    
    // Reset bullets
    bullet_init(bullets, MAX_BULLETS);
    
    // Reset texte
    text_clear(&textJeu);
    
    // IMPORTANT: Réinitialiser complètement le système d'ennemis
    enemy_init(&game->enemies, WIDTH, HEIGHT);
    
    // Reset vagues
    game->current_wave = 1;
    game->wave_transition = 0;
    game->wave_transition_time = 0;
    
    // Démarrer vague 1
    game_start_wave(game, 1);
    
    printf("=== NOUVELLE PARTIE ===\n");
}

int main() {
    Game game;
    
    /** === INITIALISATION SDL (UNE SEULE FOIS) === */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    
    if (TTF_Init() < 0) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    
    /** === Créer la fenêtre et le renderer === */
    game.window = SDL_CreateWindow("Space Invaders",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    WIDTH, HEIGHT, 0);
    if (!game.window) {
        printf("Erreur création fenêtre\n");
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);
    if (!game.renderer) {
        printf("Erreur création renderer\n");
        SDL_DestroyWindow(game.window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    /** === Charger les sprites === */
    game.sprite_manager = createSpriteManager(game.renderer, CHEMIN_VERS_SPRITE);
    if (!game.sprite_manager) {
        printf("Erreur chargement sprites!\n");
        SDL_DestroyRenderer(game.renderer);
        SDL_DestroyWindow(game.window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    g_sprite_manager = game.sprite_manager;
    
    /** === Initialiser le menu === */
    if (!menu_init(&menu, CHEMIN_VERS_POLICE)) {
        printf("Erreur initialisation menu\n");
        destroySpriteManager(game.sprite_manager);
        SDL_DestroyRenderer(game.renderer);
        SDL_DestroyWindow(game.window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    /** === Initialiser le score (police déjà chargée) === */
    score.font = TTF_OpenFont(CHEMIN_VERS_POLICE, 24);
    if (!score.font) {
        printf("Erreur chargement police score\n");
        menu_cleanup(&menu);
        destroySpriteManager(game.sprite_manager);
        SDL_DestroyRenderer(game.renderer);
        SDL_DestroyWindow(game.window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    score.value = 0;
    
    /** === Initialiser le texte === */
    textJeu.font = TTF_OpenFont(CHEMIN_VERS_POLICE, 24);
    if (!textJeu.font) {
        printf("Erreur chargement police texte\n");
        TTF_CloseFont(score.font);
        menu_cleanup(&menu);
        destroySpriteManager(game.sprite_manager);
        SDL_DestroyRenderer(game.renderer);
        SDL_DestroyWindow(game.window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    /** === Initialiser le jeu (sans démarrer) === */
    game.running = 1;
    lives_init(&lives);
    player_init(&player, WIDTH, HEIGHT);
    bullet_init(bullets, MAX_BULLETS);
    // enemy_init sera appelé dans reset_game()
    
    int game_initialized = 0;  // Flag pour savoir si on doit reset
    
    /** === BOUCLE PRINCIPALE === */
    while (game.running) {
        SDL_Event event;
        
        /** --- Gestion des événements --- */
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game.running = 0;
            }
            
            // Le menu gère ses propres events
            menu_handle_event(&menu, &event, &game.running);
        }
        
        GameState current_state = menu_get_state(&menu);
        
        /** --- Logique selon l'état --- */
        if (current_state == STATE_PLAYING) {
            // Si on vient de démarrer, initialiser le jeu
            if (!game_initialized) {
                reset_game(&game);
                game_initialized = 1;
            }
            
            // Récupérer l'état du clavier
            const Uint8 *keystates = SDL_GetKeyboardState(NULL);
            
            // Mettre à jour le joueur
            player_update(&player, keystates);
            
            // Gérer le tir
            player_handle_shooting(&player, keystates, bullets, score.value);
            
            // Mettre à jour le jeu
            game_update(&game);
            
            // Vérifier game over
            if (lives_is_game_over(&lives)) {
                menu_set_final_score(&menu, score.value);
                menu_set_state(&menu, STATE_GAMEOVER);
                game_initialized = 0;  // Permettre un nouveau reset
            }
            
            // Rendu du jeu
            game_render(&game);
        }
        else if (current_state == STATE_PAUSED) {
            // Afficher le jeu en arrière-plan PUIS l'overlay de pause
            SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
            SDL_RenderClear(game.renderer);
            
            // Redessiner le jeu (sans Present)
            player_render(&player, game.renderer);
            enemy_render(&game.enemies, game.renderer);
            bullet_render(bullets, MAX_BULLETS, game.renderer, score.value);
            score_render(&score, game.renderer);
            lives_render(&lives, game.renderer);
            
            // Afficher l'overlay de pause par-dessus
            menu_render(&menu, game.renderer);
        }
        else if (current_state == STATE_MENU || current_state == STATE_GAMEOVER) {
            // Afficher le menu
            menu_render(&menu, game.renderer);
            
            // Reset le flag si on revient au menu
            if (current_state == STATE_MENU) {
                game_initialized = 0;
            }
        }
        
        // Réguler le framerate
        SDL_Delay(16);  // ~60 FPS
    }
    
    /** === NETTOYAGE === */
    TTF_CloseFont(textJeu.font);
    TTF_CloseFont(score.font);
    menu_cleanup(&menu);
    destroySpriteManager(game.sprite_manager);
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    TTF_Quit();
    SDL_Quit();
    
    printf("Jeu fermé proprement.\n");
    return 0;
}