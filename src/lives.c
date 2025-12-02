#include "lives.h"

extern SpriteManager *g_sprite_manager;

/* ---- Initialiser le système de vies ---- */
void lives_init(Lives *lives) {
    lives->current_lives = MAX_LIVES;
    lives->max_lives = MAX_LIVES;
    lives->danger_warning = 0;
    lives->invincibility_time = 1000;  // 1 seconde d'invincibilité
    lives->last_hit_time = 0;
}

/* ---- Vérifier si des ennemis sont dans la zone de danger ---- */
int lives_check_danger(Lives *lives, EnemyGrid *enemies, int screenHeight) {
    lives->danger_warning = 0;  // Reset
    
    // Parcourir tous les ennemis vivants
    for (int i = 0; i < enemies->count; i++) {
        if (!enemies->enemies[i].alive) continue;
        
        Enemy *e = &enemies->enemies[i];
        
        // Vérifier si l'ennemi est dans la zone de danger
        if (e->y >= DANGER_THRESHOLD) {
            lives->danger_warning = 1;  // Activer l'alerte
            
            // Vérifier si l'ennemi a atteint le fond complet
            if (e->y + e->height >= screenHeight - 60) {
                // Vérifier l'invincibilité
                Uint32 current_time = SDL_GetTicks();
                if (current_time - lives->last_hit_time > lives->invincibility_time) {
                    // Perdre une vie !
                    lives_lose_one(lives);
                    lives->last_hit_time = current_time;
                    
                    // Détruire l'ennemi qui a touché
                    e->alive = 0;
                    enemies->alive_count--;
                    
                    printf("Vie perdue ! Vies restantes : %d\n", lives->current_lives);
                    
                    // Vérifier game over
                    if (lives_is_game_over(lives)) {
                        return 1;  // Game over !
                    }
                }
            }
        }
    }
    
    return 0;  // Pas de game over
}

/* ---- Perdre une vie ---- */
int lives_lose_one(Lives *lives) {
    if (lives->current_lives > 0) {
        lives->current_lives--;
        return 1;  // Vie perdue
    }
    return 0;  // Déjà à 0
}

/* ---- Afficher les cœurs et le warning ---- */
void lives_render(Lives *lives, SDL_Renderer *renderer) {
    if (!g_sprite_manager) return;
    
    int heart_x = 10;      // Position X de départ (bas gauche)
    int heart_y = 550;     // Position Y (bas de l'écran - 50px)
    int heart_spacing = 30; // Espace entre les cœurs
    int heart_scale = 3;    // Échelle des cœurs
    
    // Afficher les cœurs restants
    for (int i = 0; i < lives->current_lives; i++) {
        renderSprite(g_sprite_manager, renderer, COEUR, 
                    heart_x + (i * heart_spacing), heart_y, heart_scale);
    }
    
    // Afficher le WARNING si danger
    if (lives->danger_warning) {
        int warning_x = 10;
        int warning_y = heart_y - 40;  // Au-dessus des cœurs
        int warning_scale = 4;         // WARNING est 64x8, échelle 2 = 128x16
        
        // Faire clignoter le warning (toutes les 250ms)
        Uint32 time = SDL_GetTicks();
        if ((time / 250) % 2 == 0) {  // Clignote
            renderSprite(g_sprite_manager, renderer, WARNING, 
                        warning_x, warning_y, warning_scale);
        }
    }
}

/* ---- Vérifier si c'est game over ---- */
int lives_is_game_over(Lives *lives) {
    return lives->current_lives <= 0;
}