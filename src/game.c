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

/* ---- Démarrer une vague ---- */
void game_start_wave(Game *game, int wave_number) {
    printf("=== WAVE %d START ===\n", wave_number);
    
    // Réinitialiser le système d'ennemis
    enemy_init(&game->enemies, 800, 600);
    
    // Démarrer la vague
    enemy_start_wave(&game->enemies, wave_number);
}

/* ---- Fonction qui met à jour le jeu ---- */
void game_update(Game *game) {
    // Si transition de vague en cours, attendre
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
    
    /* ---- Logique des balles ---- */
    bullet_update(bullets, MAX_BULLETS);
    bullet_check_collisions(bullets, MAX_BULLETS, &game->enemies, &score);
    
    /* ---- Logique des ennemis ---- */
    enemy_update(&game->enemies, 800);

    /* ---- Vérifier si des ennemis touchent le fond ---- */
    if (lives_check_danger(&lives, &game->enemies, 600)) {
        printf("GAME OVER! No more lives!\n");
        // Le game over est géré par le menu maintenant
        return;
    }

    /* ---- Vérifier si tous les ennemis sont détruits ---- */
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

/* ---- Fonction affichage/rendering ---- */
void game_render(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    // Affichage texte de transition
    if (game->wave_transition) {
        text_render(&textJeu, game->renderer);
        SDL_RenderPresent(game->renderer);
        return;
    }
    
    /* ---- Dessiner le joueur ---- */
    player_render(&player, game->renderer);
    
    /* ---- Dessiner les ennemis ---- */
    enemy_render(&game->enemies, game->renderer);
    
    /* ---- Dessiner les balles ---- */
    bullet_render(bullets, MAX_BULLETS, game->renderer, score.value);

    /* ---- Afficher le score ---- */
    score_render(&score, game->renderer);

    /* ---- Afficher les vies et warning ---- */
    lives_render(&lives, game->renderer);
    
    /* Afficher le frame */
    SDL_RenderPresent(game->renderer);
}