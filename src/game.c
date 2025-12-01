#include "game.h"

extern Player player;
extern Bullet bullets[MAX_BULLETS];
extern Score score;

// Variable globale pour accéder au sprite manager partout
SpriteManager *g_sprite_manager = NULL;

/* ---- initialisation du jeu ---- */
int game_init(Game *game, const char *title, int width, int height) {
    
    /*------- Initialisation SDL & Fenêtre -------*/
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }
    
    IMG_Init(IMG_INIT_PNG); // Initialiser SDL_image
    game->window = SDL_CreateWindow(title,
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     width, height,
                                     0);
                                     
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    game->running = 1;
    

    
    /*------- Charger les sprites -------*/
    game->sprite_manager = createSpriteManager(game->renderer, CHEMIN_VERS_SPRITE);
    if (!game->sprite_manager) {
        printf("Erreur chargement sprites!\n");
        return 0;
    }
    
    // Stocker globalement pour y accéder dans player_render
    g_sprite_manager = game->sprite_manager;
    
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
void game_update(Game *game) {
    
    /* ---- bullet logic ---- */
    bullet_update(bullets, MAX_BULLETS);
    bullet_check_collisions(bullets, MAX_BULLETS, &game->enemies, &score);  // NOUVEAU
    
    /* ---- enemy logic ---- */
    enemy_update(&game->enemies);
    if (enemy_check_reached_bottom(&game->enemies, 600)) {
        game->running = 0;  // Game over !!!!!
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
    
    bullet_render(bullets, MAX_BULLETS, game->renderer, score.value);
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