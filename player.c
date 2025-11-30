#include "player.h"

void player_init(Player *p, int screenWidth, int screenHeight) {
    p->x = screenWidth / 2;
    p->y = screenHeight - 50;
    p->speed = 5;
}

void player_update(Player *p, const Uint8 *keystates) {
    if (keystates[SDL_SCANCODE_LEFT])
        p->x -= p->speed;

    if (keystates[SDL_SCANCODE_RIGHT])
        p->x += p->speed;
}

void player_render(Player *p, SDL_Renderer *renderer) {
    SDL_Rect rect = { p->x, p->y, 40, 20 };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
}
