#include "score.h"
#include <stdio.h>

int score_init(Score *score, const char *fontPath, int fontSize) {

    if (TTF_Init() < 0)
        return 0;

    score->font = TTF_OpenFont(fontPath, fontSize);
    if (!score->font)
        return 0;

    score->value = 0;
    return 1;
}

void score_add(Score *score, int points) {
    score->value += points;
}

void score_render(Score *score, SDL_Renderer *renderer) {

    char text[32];
    sprintf(text, "Score : %d", score->value);

    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface *surface = TTF_RenderText_Solid(score->font, text, white);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dest = {10, 10, surface->w, surface->h};

    SDL_FreeSurface(surface);

    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
}

void score_cleanup(Score *score) {
    TTF_CloseFont(score->font);
    TTF_Quit();
}
