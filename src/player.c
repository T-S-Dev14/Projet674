#include "player.h"

// Déclaration externe du sprite manager global
extern SpriteManager *g_sprite_manager;

void player_init(Player *p, int screenWidth, int screenHeight) {
    p->x = screenWidth / 2;
    p->y = screenHeight - 50;
    p->speed = 5;
    p->current_sprite = VAISSEAU_1;
    p->flame_frame = 0;
    p->flame_timer = 0;
}

void player_update(Player *p, const Uint8 *keystates) {
    // Changer le sprite selon la direction
    if (keystates[SDL_SCANCODE_LEFT]) {
        p->x -= p->speed;
        p->current_sprite = VAISSEAU_2;  // Sprite gauche
    }
    else if (keystates[SDL_SCANCODE_RIGHT]) {
        p->x += p->speed;
        p->current_sprite = VAISSEAU_3;  // Sprite droite
    }
    else {
        p->current_sprite = VAISSEAU_1;  // Sprite neutre
    }

    // ---------- Animation de la flamme ----------
    p->flame_timer++;
    if (p->flame_timer >= 10) {
        p->flame_frame = (p->flame_frame + 1) % 3;
        p->flame_timer = 0;
    }
} 

void player_render(Player *p, SDL_Renderer *renderer) {
    if (g_sprite_manager) {
        // ----- DESSINER LA FLAMME -----
        SpriteID flame_sprite;
        switch (p->flame_frame) {
            case 0: flame_sprite = FLAMME_1; break;
            case 1: flame_sprite = FLAMME_2; break;
            case 2: flame_sprite = FLAMME_3; break;
            default: flame_sprite = FLAMME_1; break;
        }

        // Flamme en dessous du vaisseau
        renderSprite(g_sprite_manager, renderer, flame_sprite, p->x, p->y + 20, 3);

        // Vaisseau par-dessus
        renderSprite(g_sprite_manager, renderer, p->current_sprite, p->x, p->y, 3);
    } else {
        // Fallback: rectangle vert
        SDL_Rect rect = { p->x, p->y, 40, 20 };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void player_handle_shooting(Player *p, const Uint8 *keys, Bullet bullets[], int score) {
    static int prev_space = 0;
    int space = keys[SDL_SCANCODE_SPACE];
    
    if (space && !prev_space) {
        int damage;
        if (score >= 50) {
            damage = 10;  // BALLES_3 : 30 dégâts
        } else if (score >= 25) {
            damage = 8;  // BALLES_2 : 20 dégâts
        } else {
            damage = 5;  // BALLES_1 : 10 dégâts
        }
        
        bullet_shoot(bullets, MAX_BULLETS, p->x + 4, p->y, damage);
    }
    
    prev_space = space;
}