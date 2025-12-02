#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    char TextPrint[100];
    TTF_Font *font;
} TextJeu;


int text_init(TextJeu *textJeu, const char *fontPath, int fontSize);
void text_render(TextJeu *textJeu, SDL_Renderer *renderer);
void text_clear(TextJeu *textJeu);
void text_cleanup(TextJeu *tetxJeu);

#endif