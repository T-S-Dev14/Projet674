#include "player.h"

// Déclaration externe du sprite manager global
extern SpriteManager *g_sprite_manager;

void player_init(Player *p, int screenWidth, int screenHeight) {
    p->x = screenWidth / 2;
    p->y = screenHeight - 50;
    p->speed = 5;
    p->current_sprite = VAISSEAU_1;  // NOUVEAU: Sprite par défaut
}

void player_update(Player *p, const Uint8 *keystates) {
    // MODIFIÉ: Changer le sprite selon la direction
    if (keystates[SDL_SCANCODE_LEFT]) {
        p->x -= p->speed;
        p->current_sprite = VAISSEAU_2;  // Sprite gauche
    }
    else if (keystates[SDL_SCANCODE_RIGHT]) {
        p->x += p->speed;
        p->current_sprite = VAISSEAU_3;  // Sprite droite
    }
    else {
        p->current_sprite = VAISSEAU_1;  // Sprite neutre (aucune touche)
    }
}

void player_render(Player *p, SDL_Renderer *renderer) {
    // Utiliser le sprite si disponible, sinon rectangle vert
    if (g_sprite_manager) {
        renderSprite(g_sprite_manager, renderer, p->current_sprite, p->x, p->y, 3);
    } else {
        // Fallback: rectangle vert
        SDL_Rect rect = { p->x, p->y, 40, 20 };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void player_handle_shooting(Player *p, const Uint8 *keys, Bullet bullets[]) {
    static int prev_space = 0;
    int space = keys[SDL_SCANCODE_SPACE];
    if (space && !prev_space) {
        bullet_shoot(bullets, MAX_BULLETS, p->x + 18, p->y);
    }
    prev_space = space;
}