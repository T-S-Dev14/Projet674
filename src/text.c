#include "text.h"
#include <stdio.h>


int text_init(TextJeu *textJeu, const char *fontPath, int fontSize) {

    if (TTF_Init() < 0)
        return 0;

    textJeu->font = TTF_OpenFont(fontPath, fontSize);
    if (!textJeu->font){
        return 0;
    }
    return 1;
}

void text_render(TextJeu *textJeu, SDL_Renderer *renderer) {
    // Vérifier si il y a du texte à afficher
    if (textJeu->TextPrint[0] == '\0') {
        return;  // Rien à afficher
    }
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(textJeu->font, textJeu->TextPrint, white);
    
    if (!surface) {
        printf("Erreur création surface texte: %s\n", TTF_GetError());
        return;
    }
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // ← NOUVEAU: Centrer le texte au milieu de l'écran (800x600)
    int text_width = surface->w;
    int text_height = surface->h;
    
    SDL_Rect dest = {
        (800 - text_width) / 2,   // Centré horizontalement
        (600 - text_height) / 2,  // Centré verticalement
        text_width,
        text_height
    };
    
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
}

void text_clear(TextJeu *textJeu) {
    textJeu->TextPrint[0] = '\0';
}

void text_cleanup(TextJeu *textJeu) {
   
    if (textJeu->font) {
        TTF_CloseFont(textJeu->font);
        textJeu->font = NULL;
    }
    TTF_Quit();
}