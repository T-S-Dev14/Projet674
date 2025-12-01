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

    game->current_wave = 1;
    game->wave_transition = 0;
    game->wave_transition_time = 0;
    
    /* ---- Démarrer la vague  1 --- */
    game_start_wave(game, 1);
    
    return 1;
}

void game_start_wave(Game *game, int wave_number) {
    printf("=== WAVE %d START ===\n", wave_number);
    
    // Réinitialiser les ennemis
    enemy_init(&game->enemies, 800, 600);
    
    // Augmenter la difficulté selon la vague
    // Vague 1: vitesse normale (500ms)
    // Vague 2: plus rapide (400ms)
    // Vague 3+: encore plus rapide
    int base_speed = 500;
    game->enemies.speed = base_speed - (wave_number - 1) * 100;
    
    // Vitesse minimale: 200ms
    if (game->enemies.speed < 200) {
        game->enemies.speed = 200;
    }
    
    // Distance de descente augmente avec les vagues
    game->enemies.drop_distance = 20 + (wave_number - 1) * 5;
    
    printf("Speed: %dms, Drop: %dpx\n", 
           game->enemies.speed, 
           game->enemies.drop_distance);
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

    if (game->wave_transition) {
        Uint32 current_time = SDL_GetTicks();
        
        // Attendre 2 secondes avant la prochaine vague
        if (current_time - game->wave_transition_time > 2000) {
            game->wave_transition = 0;
            game_start_wave(game, game->current_wave);
        }
        
        return; // Ne pas mettre à jour le jeu pendant la transition
    }
    
    /* ---- bullet logic ---- */
    bullet_update(bullets, MAX_BULLETS);
    bullet_check_collisions(bullets, MAX_BULLETS, &game->enemies, &score);  // NOUVEAU
    
    /* ---- enemy logic ---- */
    enemy_update(&game->enemies);

    if (game->enemies.alive_count == 0) {
        printf("All enemies destroyed! Starting next wave...\n");
        game->current_wave++;
        game->wave_transition = 1;
        game->wave_transition_time = SDL_GetTicks();
    }

    if (enemy_check_reached_bottom(&game->enemies, 600)) {
        game->running = 0;  // Game over !!!!!
    }
}

/* ---- fonction affichage/rendering ---- */
void game_render(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    if (game->wave_transition) {
        // Afficher "WAVE X" au centre de l'écran
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
        
        // Message simple en rectangles (temporaire)
        // Tu pourras améliorer avec SDL_ttf plus tard
        char wave_text[32];
        sprintf(wave_text, "WAVE %d", game->current_wave);
        
        // Pour l'instant, juste un carré blanc au centre
        SDL_Rect wave_indicator = {350, 280, 100, 40};
        SDL_RenderFillRect(game->renderer, &wave_indicator);
        
        SDL_RenderPresent(game->renderer);
        return;
    }
    
    /* ----Dessiner le joueur ---- */
    player_render(&player, game->renderer);  // PAS DE CHANGEMENT ICI
    
    /* ---- Dessiner les enemies ----*/
    enemy_render(&game->enemies, game->renderer);
    
    /* ---- Dessiner les bullets ----*/
    bullet_render(bullets, MAX_BULLETS, game->renderer, score.value);

    /* ---- Afficher le score  ----*/
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