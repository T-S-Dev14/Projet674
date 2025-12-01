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