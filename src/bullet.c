#include "bullet.h"

// Déclaration externe du sprite manager global
extern SpriteManager *g_sprite_manager;

void bullet_init(Bullet bullets[], int size) {
    for (int i = 0; i < size; i++) {
        bullets[i].active = 0;
        bullets[i].speed = 8;
        bullets[i].damage = 5;  // Dégâts par défaut
    }
}

void bullet_shoot(Bullet bullets[], int size, int x, int y, int damage) {
    for (int i = 0; i < size; i++) {
        if (!bullets[i].active) {
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].active = 1;
            bullets[i].damage = damage;
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

void bullet_render(Bullet bullets[], int size, SDL_Renderer *renderer, int score_value) {
    // Déterminer quel sprite utiliser selon le score
    SpriteID bullet_sprite;
    if (score_value >= 50) {
        bullet_sprite = BALLES_3;
    } else if (score_value >= 25) {
        bullet_sprite = BALLES_2;
    } else {
        bullet_sprite = BALLES_1;
    }
    
    for (int i = 0; i < size; i++) {
        if (bullets[i].active) {
            if (g_sprite_manager) {
                renderSprite(g_sprite_manager, renderer, bullet_sprite, 
                           bullets[i].x, bullets[i].y, 2);
            } else {
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
        if (!bullets[i].active) continue;
        
        int result = enemy_check_collision(
            enemies, 
            bullets[i].x, 
            bullets[i].y, 
            BULLET_WIDTH, 
            BULLET_HEIGHT,
            bullets[i].damage
        );
        
        if (result >= 0) {
            bullets[i].active = 0;
            
            if (result > 0) {
                score_add(score, result);
            }
        }
    }
}