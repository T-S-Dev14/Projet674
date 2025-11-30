#include "bullet.h"

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

void bullet_render(Bullet bullets[], int size, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    
    for (int i = 0; i < size; i++) {
        if (bullets[i].active) {
            SDL_Rect rect = { bullets[i].x, bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT };

            SDL_RenderFillRect(renderer, &rect);
        }
    }
}