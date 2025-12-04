#include "highscore.h"
#include <stdio.h>
#include <string.h>

/* ---- Initialiser le gestionnaire de highscores ---- */
int highscore_init(HighscoreManager *manager, const char *fontPath, int fontSize) {
    manager->count = 0;
    memset(manager->entries, 0, sizeof(manager->entries));
    
    manager->font = TTF_OpenFont(fontPath, fontSize);
    if (!manager->font) {
        printf("Erreur chargement police highscores: %s\n", TTF_GetError());
        return 0;
    }
    
    // Charger les scores existants
    highscore_load(manager);
    
    return 1;
}

/* ---- Charger les scores depuis le fichier ---- */
void highscore_load(HighscoreManager *manager) {
    FILE *file = fopen(HIGHSCORE_FILE, "r");
    if (!file) {
        printf("Aucun fichier de scores existant, création...\n");
        return;
    }
    
    manager->count = 0;
    while (manager->count < MAX_HIGHSCORES && 
           fscanf(file, "%19s %d", 
                  manager->entries[manager->count].name,
                  &manager->entries[manager->count].score) == 2) {
        manager->count++;
    }
    
    fclose(file);
    printf("Chargé %d scores\n", manager->count);
}

/* ---- Sauvegarder les scores dans le fichier ---- */
void highscore_save(HighscoreManager *manager) {
    FILE *file = fopen(HIGHSCORE_FILE, "w");
    if (!file) {
        printf("Erreur: impossible de sauvegarder les scores\n");
        return;
    }
    
    for (int i = 0; i < manager->count; i++) {
        fprintf(file, "%s %d\n", 
                manager->entries[i].name, 
                manager->entries[i].score);
    }
    
    fclose(file);
    printf("Scores sauvegardés (%d entrées)\n", manager->count);
}

/* ---- Ajouter un nouveau score ---- */
void highscore_add(HighscoreManager *manager, const char *name, int score) {
    // Trouver où insérer le score (tri par ordre décroissant)
    int insert_pos = manager->count;
    
    for (int i = 0; i < manager->count; i++) {
        if (score > manager->entries[i].score) {
            insert_pos = i;
            break;
        }
    }
    
    // Si le score n'entre pas dans le top, ignorer
    if (insert_pos >= MAX_HIGHSCORES) {
        return;
    }
    
    // Décaler les scores inférieurs
    for (int i = MAX_HIGHSCORES - 1; i > insert_pos; i--) {
        if (i - 1 >= 0) {
            manager->entries[i] = manager->entries[i - 1];
        }
    }
    
    // Insérer le nouveau score
    strncpy(manager->entries[insert_pos].name, name, MAX_NAME_LENGTH - 1);
    manager->entries[insert_pos].name[MAX_NAME_LENGTH - 1] = '\0';
    manager->entries[insert_pos].score = score;
    
    // Mettre à jour le compteur
    if (manager->count < MAX_HIGHSCORES) {
        manager->count++;
    }
    
    printf("Nouveau score ajouté: %s - %d (position %d)\n", name, score, insert_pos + 1);
    
    // Sauvegarder automatiquement
    highscore_save(manager);
}

/* ---- Vérifier si un score entre dans le top ---- */
int highscore_is_new_highscore(HighscoreManager *manager, int score) {
    // Si on a moins de MAX_HIGHSCORES, c'est toujours un highscore
    if (manager->count < MAX_HIGHSCORES) {
        return 1;
    }
    
    // Sinon, vérifier si le score est meilleur que le dernier
    return score > manager->entries[manager->count - 1].score;
}

/* ---- Afficher les highscores ---- */
void highscore_render(HighscoreManager *manager, SDL_Renderer *renderer, int x, int y) {
    if (!manager->font) {
        return;
    }
    
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    
    // Titre
    SDL_Surface *title_surface = TTF_RenderText_Solid(manager->font, "HIGH SCORES", yellow);
    if (title_surface) {
        SDL_Texture *title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
        SDL_Rect title_rect = {x, y, title_surface->w, title_surface->h};
        SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
        SDL_FreeSurface(title_surface);
        SDL_DestroyTexture(title_texture);
    }
    
    // Afficher les scores
    int line_height = 30;
    for (int i = 0; i < manager->count && i < 10; i++) {
        char line[100];
        sprintf(line, "%d. %s - %d", i + 1, 
                manager->entries[i].name, 
                manager->entries[i].score);
        
        SDL_Surface *surface = TTF_RenderText_Solid(manager->font, line, white);
        if (surface) {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = {x + 20, y + 40 + i * line_height, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
}

/* ---- Nettoyer ---- */
void highscore_cleanup(HighscoreManager *manager) {
    if (manager->font) {
        TTF_CloseFont(manager->font);
        manager->font = NULL;
    }
}