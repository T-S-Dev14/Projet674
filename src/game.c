#include "game.h"
#include "text.h"
#include "lives.h" 

extern Player player;
extern Bullet bullets[MAX_BULLETS];
extern Score score;
extern TextJeu textJeu;
extern Lives lives;  

// Variable globale pour accéder au sprite manager partout
SpriteManager *g_sprite_manager = NULL;

/* ---- initialisation du jeu ---- */
int game_init(Game *game, const char *title, int width, int height) {
    
    /*------- Initialisation SDL & Fenêtre -------*/
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }
    
    IMG_Init(IMG_INIT_PNG);
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
    
    // Stocker globalement
    g_sprite_manager = game->sprite_manager;

    game->current_wave = 1;
    game->wave_transition = 0;
    game->wave_transition_time = 0;
    
    /* ---- Démarrer la vague 1 --- */
    game_start_wave(game, 1);
    
    return 1;
}

void game_start_wave(Game *game, int wave_number) {
    printf("=== WAVE %d START ===\n", wave_number);
    
    // Réinitialiser le système d'ennemis
    enemy_init(&game->enemies, 800, 600);
    
    // Démarrer la vague
    enemy_start_wave(&game->enemies, wave_number);
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
            text_clear(&textJeu);
            game_start_wave(game, game->current_wave);
        }
        
        return; // Ne pas mettre à jour le jeu pendant la transition
    }
    
    /* ---- bullet logic ---- */
    bullet_update(bullets, MAX_BULLETS);
    bullet_check_collisions(bullets, MAX_BULLETS, &game->enemies, &score);
    
    /* ---- enemy logic ---- */
    enemy_update(&game->enemies, 800);

    if (lives_check_danger(&lives, &game->enemies, 600)) {
        printf("GAME OVER! No more lives!\n");
        game->running = 0;
        return;
    }

    // Vérifier si tous les ennemis ET météorites sont détruits/spawnés
    if (game->enemies.alive_count == 0 && 
        game->enemies.enemies_to_spawn == 0 && 
        game->enemies.asteroids_to_spawn == 0) {
        printf("All enemies destroyed! Starting next wave...\n");
        
        // Afficher "WAVE X" AVANT d'incrémenter
        game->wave_transition = 1;
        game->wave_transition_time = SDL_GetTicks();
        sprintf(textJeu.TextPrint, "WAVE %d", game->current_wave);
        game->current_wave++;
    }
}

/* ---- fonction affichage/rendering ---- */
void game_render(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    if (game->wave_transition) {
        text_render(&textJeu, game->renderer);
        SDL_RenderPresent(game->renderer);
        return;
    }
    
    /* ---- Dessiner le joueur ---- */
    player_render(&player, game->renderer);
    
    /* ---- Dessiner les enemies ----*/
    enemy_render(&game->enemies, game->renderer);
    
    /* ---- Dessiner les bullets ----*/
    bullet_render(bullets, MAX_BULLETS, game->renderer, score.value);

    /* ---- Afficher le score  ----*/
    score_render(&score, game->renderer);

    /* ---- Afficher les vies et warning ---- */
    lives_render(&lives, game->renderer);
    
    /* afficher le frame */
    SDL_RenderPresent(game->renderer);
}

/* ---- fonction nettoyage des ressources du jeu ---- */
void game_cleanup(Game *game) {
    if (game->sprite_manager) {
        destroySpriteManager(game->sprite_manager);
    }
    
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    
    IMG_Quit();
    SDL_Quit();
}