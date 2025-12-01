#include "bullet.h"

// Déclaration externe du sprite manager global
extern SpriteManager *g_sprite_manager;

void bullet_init(Bullet bullets[], int size) {
    for (int i = 0; i < size; i++) {
        bullets[i].active = 0;
        bullets[i].speed = 8;
    }
}

void bullet_shoot(Bullet bullets[], int size, int x, int y) {
    for (int i = 0; i < size; i++) {
        if (!bullets[i].active) {
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].active = 1;
            break;
        }
    }
}

void bullet_update(Bullet bullets[], int size) {
    for (int i = 0; i < size; i++) {
        if (bullets[i].active) {
            bullets[i].y -= bullets[i].speed;
            if (bullets[i].y < 0)
                bullets[i].active = 0;
        }
    }
}

void bullet_render(Bullet bullets[], int size, SDL_Renderer *renderer, int score) {
    // Déterminer quel sprite utiliser selon le score
    SpriteID bullet_sprite;
    if (score >= 50) {
        bullet_sprite = BALLES_3;      // Score >= 50
    } else if (score >= 25) {
        bullet_sprite = BALLES_2;      // Score >= 25
    } else {
        bullet_sprite = BALLES_1;      // Score < 25
    }
    
    for (int i = 0; i < size; i++) {
        if (bullets[i].active) {
            if (g_sprite_manager) {
                // Dessiner avec le sprite
                renderSprite(g_sprite_manager, renderer, bullet_sprite, 
                           bullets[i].x, bullets[i].y, 2);  // Échelle 2
            } else {
                // Fallback: rectangle jaune
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_Rect rect = { bullets[i].x, bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}


/* ---- Vérifier les collisions bullets → ennemis ---- */
void bullet_check_collisions(Bullet bullets[], int size, EnemyGrid *enemies, Score *score) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;  // Ignorer les bullets inactives
        
        // Vérifier si cette bullet touche un ennemi
        int hit_index = enemy_check_collision(
            enemies, 
            bullets[i].x, 
            bullets[i].y, 
            BULLET_WIDTH, 
            BULLET_HEIGHT
        );
        
        // Si un ennemi a été touché
        if (hit_index >= 0) {
            bullets[i].active = 0;                                   // désactiver la bullet
            EnemyType typeEnemy = enemies->enemies[hit_index].type;  // récupérer le type d'ennemie
            
            // ajouter score en fonction du type d'ennemie abbatu.
            switch (typeEnemy) {
              case ENEMY_TYPE_1:
                score_add(score, 10);   // Ennemis du haut = 10 points
                break;

              case ENEMY_TYPE_2:
                score_add(score, 20);   // Ennemis du milieu = 20 points
                break;

              case ENEMY_TYPE_3:
                score_add(score, 30);   // Ennemis du bas = 30 points
                break;

              default:
                // Optionnel : type inconnu, aucune action ou debug
                break;
            }
        }
    }
}