#include "player.h"

extern SpriteManager *g_sprite_manager;

void player_init(Player *p, int screenWidth, int screenHeight) {
    p->x = screenWidth / 2;
    p->y = screenHeight - 50;
    p->speed = 8;
    p->current_sprite = VAISSEAU_1;
    p->flame_frame = 0;
    p->flame_timer = 0;
}

void player_update(Player *p, const Uint8 *keystates) {
    if (keystates[SDL_SCANCODE_LEFT]) {
        p->x -= p->speed;
        p->current_sprite = VAISSEAU_2;
    }
    else if (keystates[SDL_SCANCODE_RIGHT]) {
        p->x += p->speed;
        p->current_sprite = VAISSEAU_3;
    }
    else {
        p->current_sprite = VAISSEAU_1;
    }

    p->flame_timer++;
    if (p->flame_timer >= 10) {
        p->flame_frame = (p->flame_frame + 1) % 3;
        p->flame_timer = 0;
    }
} 

void player_render(Player *p, SDL_Renderer *renderer) {
    if (g_sprite_manager) {
        SpriteID flame_sprite;
        switch (p->flame_frame) {
            case 0: flame_sprite = FLAMME_1; break;
            case 1: flame_sprite = FLAMME_2; break;
            case 2: flame_sprite = FLAMME_3; break;
            default: flame_sprite = FLAMME_1; break;
        }

        renderSprite(g_sprite_manager, renderer, flame_sprite, p->x, p->y + 20, 3);
        renderSprite(g_sprite_manager, renderer, p->current_sprite, p->x, p->y, 3);
    } else {
        SDL_Rect rect = { p->x, p->y, 40, 20 };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void player_handle_shooting(Player *p, const Uint8 *keys, Bullet bullets[]) {
    static int prev_space = 0;
    int space = keys[SDL_SCANCODE_SPACE];
    
    if (space && !prev_space) {
        int damage = 10;
        bullet_shoot(bullets, MAX_BULLETS, p->x + 4, p->y, damage);
    }
    
    prev_space = space;
}