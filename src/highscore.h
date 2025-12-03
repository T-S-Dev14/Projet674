#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define MAX_NAME_LENGTH 20
#define MAX_HIGHSCORES 10
#define HIGHSCORE_FILE "highscores.txt"

/* Structure pour un score */
typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} HighscoreEntry;

/* Structure pour gérer les highscores */
typedef struct {
    HighscoreEntry entries[MAX_HIGHSCORES];
    int count;  // Nombre de scores enregistrés
    TTF_Font *font;
} HighscoreManager;

/* Fonctions publiques */
int highscore_init(HighscoreManager *manager, const char *fontPath, int fontSize);
void highscore_load(HighscoreManager *manager);
void highscore_save(HighscoreManager *manager);
void highscore_add(HighscoreManager *manager, const char *name, int score);
int highscore_is_new_highscore(HighscoreManager *manager, int score);
void highscore_render(HighscoreManager *manager, SDL_Renderer *renderer, int x, int y);
void highscore_cleanup(HighscoreManager *manager);

#endif