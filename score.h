#ifndef SCORE_H
#define SCORE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    int value;
    TTF_Font *font;
} Score;

int score_init(Score *score, const char *fontPath, int fontSize);
void score_add(Score *score, int points);
void score_render(Score *score, SDL_Renderer *renderer);
void score_cleanup(Score *score);

#endif
