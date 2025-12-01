#include "game.h"
#include "player.h"
#include "bullet.h"
#include "score.h"
#include <SDL2/SDL_image.h>

extern Player player;
extern Bullet bullets[MAX_BULLETS];
extern Score score;

// Variable globale pour accéder au sprite manager partout
SpriteManager *g_sprite_manager = NULL;

/* ---- initialisation du jeu ---- */
int game_init(Game *game, const char *title, int width, int height) {
    /* ---- Logique de SDL  ----*/
    // vérifier si SDL s'est bien lancé
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }
    
    // Initialiser SDL_image
    IMG_Init(IMG_INIT_PNG);
    
    // taille de la fenêtre
    game->window = SDL_CreateWindow(title,
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     width, height,
                                     0);
    
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    game->running = 1;
    
    // NOUVEAU: Charger les sprites
    game->sprite_manager = createSpriteManager(game->renderer, "sprite.png");
    if (!game->sprite_manager) {
        printf("Erreur chargement sprites!\n");
        return 0;
    }
    
    // Stocker globalement pour y accéder dans player_render
    g_sprite_manager = game->sprite_manager;
    
    /* ---- init enemies (spaces invaders arghhh watch out, here they come!!!!) ---- */
    enemy_init(&game->enemies, width, height);
    
    return 1;
}

/* ---- gérer les événements ----*/
void game_handle_events(Game *game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            game->running = 0;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE)
                game->running = 0;
        }
    }
}

/* ---- fonction qui met à jour le jeu  ---- */
/* ---- fonction qui met à jour le jeu  ---- */
void game_update(Game *game) {
    /* ---- bullet logic ---- */
    bullet_update(bullets, MAX_BULLETS);
    
    /* ---- NOUVEAU: Vérifier les collisions bullets → ennemis ---- */
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;  // Ignorer les bullets inactives
        
        // Vérifier si cette bullet touche un ennemi
        int hit_index = enemy_check_collision(
            &game->enemies, 
            bullets[i].x, 
            bullets[i].y, 
            BULLET_WIDTH, 
            BULLET_HEIGHT
        );
        
        // Si un ennemi a été touché
        if (hit_index >= 0) {
            // Désactiver la bullet
            bullets[i].active = 0;
            
            // Ajouter des points selon le type d'ennemi touché
            Enemy *hit_enemy = &game->enemies.enemies[hit_index];
            
            if (hit_enemy->type == ENEMY_TYPE_1) {
                score_add(&score, 10);  // Ennemis du haut = 10 points
            } 
            else if (hit_enemy->type == ENEMY_TYPE_2) {
                score_add(&score, 20);  // Ennemis du milieu = 20 points
            } 
            else if (hit_enemy->type == ENEMY_TYPE_3) {
                score_add(&score, 30);  // Ennemis du bas = 30 points
            }
        }
    }
    
    /* ---- enemy logic ---- */
    enemy_update(&game->enemies);
    
    if (enemy_check_reached_bottom(&game->enemies, 600)) {
        game->running = 0;  // Game over
    }
}

/* ---- fonction affichage/rendering ---- */
void game_render(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);
    
    /* ----Dessiner le joueur ---- */
    player_render(&player, game->renderer);  // PAS DE CHANGEMENT ICI
    
    /* ---- Dessiner les enemies ----*/
    enemy_render(&game->enemies, game->renderer);
    
    bullet_render(bullets, MAX_BULLETS, game->renderer);
    score_render(&score, game->renderer);
    
    /* afficher le frame */
    SDL_RenderPresent(game->renderer);
}

/* ---- fonction nettoyage des ressources du jeu lorqu'on quitte le jeu ---- */
void game_cleanup(Game *game) {
    // NOUVEAU: Nettoyer les sprites
    if (game->sprite_manager) {
        destroySpriteManager(game->sprite_manager);
    }
    
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    
    IMG_Quit();
    SDL_Quit();
}