#include "game.h"
#include "text.h"
#include "lives.h" 
#include <string.h>

extern Player player;
extern Bullet bullets[MAX_BULLETS];
extern Score score;
extern TextJeu textJeu;
extern Lives lives;  

// Variable globale pour accéder au sprite manager partout
SpriteManager *g_sprite_manager = NULL;

/* ---- initialisation du jeu ---- */
int game_init(Game *game, const char *title, int width, int height) {
    
    /*------- Initialisation SDL & Fenêtre -------*/
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

    /* ← IMPORTANT: Initialiser TTF AVANT highscore_init */
    if (TTF_Init() < 0) {
        printf("Erreur initialisation SDL_ttf: %s\n", TTF_GetError());
        return 0;
    }
    
    IMG_Init(IMG_INIT_PNG);
    
    game->window = SDL_CreateWindow(title,
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     width, height,
                                     0);
                                     
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    game->running = 1;
    
    /*------- Charger les sprites -------*/
    game->sprite_manager = createSpriteManager(game->renderer, CHEMIN_VERS_SPRITE);
    if (!game->sprite_manager) {
        printf("Erreur chargement sprites!\n");
        return 0;
    }
    
    // Stocker globalement
    g_sprite_manager = game->sprite_manager;

    /* Maintenant on peut initialiser highscores (TTF est prêt) */
    if (!highscore_init(&game->highscores, "assets/space_invaders.ttf", 24)) {
        printf("Erreur initialisation highscores\n");
        return 0;
    }
    
    /* NOUVEAU: État initial = saisie du nom */
    game->state = GAME_STATE_NAME_INPUT;
    game->player_name[0] = '\0';
    game->name_length = 0;

    game->current_wave = 1;
    game->wave_transition = 0;
    game->wave_transition_time = 0;
    
    /* ← NE PAS démarrer la vague ici, attendre la saisie du nom */
    
    return 1;
}

void game_start_wave(Game *game, int wave_number) {
    printf("=== WAVE %d START ===\n", wave_number);
    
    // Réinitialiser le système d'ennemis
    enemy_init(&game->enemies, 800, 600);
    
    // Démarrer la vague
    enemy_start_wave(&game->enemies, wave_number);
}

/* ---- NOUVEAU: Gérer la saisie du nom ---- */
void game_handle_name_input(Game *game, SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_RETURN && game->name_length > 0) {
            // Entrée pressée et nom non vide → démarrer le jeu
            game->player_name[game->name_length] = '\0';
            game->state = GAME_STATE_PLAYING;
            
            /* ← IMPORTANT: Réinitialiser TOUT ici */
            score.value = 0;
            game->current_wave = 1;
            lives.current_lives = MAX_LIVES;
            
            game_start_wave(game, 1);  // ← Démarrer MAINTENANT
            printf("Joueur: %s - Début du jeu!\n", game->player_name);
        }
        else if (event->key.keysym.sym == SDLK_BACKSPACE && game->name_length > 0) {
            // Effacer un caractère
            game->name_length--;
            game->player_name[game->name_length] = '\0';
        }
    }
    else if (event->type == SDL_TEXTINPUT) {
        // Ajouter un caractère
        if (game->name_length < MAX_NAME_LENGTH - 1) {
            strcat(game->player_name, event->text.text);
            game->name_length = strlen(game->player_name);
        }
    }
}

/* ---- gérer les événements ----*/
void game_handle_events(Game *game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            game->running = 0;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE)
                game->running = 0;
        }
        
        /* ← NOUVEAU: Gestion selon l'état */
        if (game->state == GAME_STATE_NAME_INPUT) {
            game_handle_name_input(game, &event);
        }
        else if (game->state == GAME_STATE_GAME_OVER) {
            // Appuyer sur Entrée pour recommencer
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                game->state = GAME_STATE_NAME_INPUT;
                game->player_name[0] = '\0';
                game->name_length = 0;
                game->current_wave = 1;
            }
        }
    }
}

/* ---- fonction qui met à jour le jeu  ---- */
void game_update(Game *game) {
    /* Ne mettre à jour que si on joue */
    if (game->state != GAME_STATE_PLAYING) {
        return;
    }

    if (game->wave_transition) {
        Uint32 current_time = SDL_GetTicks();
        
        // Attendre 2 secondes avant la prochaine vague
        if (current_time - game->wave_transition_time > 2000) {
            game->wave_transition = 0;
            text_clear(&textJeu);
            game_start_wave(game, game->current_wave);
        }
        
        return; // Ne pas mettre à jour le jeu pendant la transition
    }
    
    /* ---- bullet logic ---- */
    bullet_update(bullets, MAX_BULLETS);
    bullet_check_collisions(bullets, MAX_BULLETS, &game->enemies, &score);
    
    /* ---- enemy logic ---- */
    enemy_update(&game->enemies, 800);

    /* ← MODIFIÉ: Passer en GAME_OVER au lieu de quitter */
    if (lives_check_danger(&lives, &game->enemies, 600)) {
        printf("GAME OVER! No more lives!\n");
        
        game->state = GAME_STATE_GAME_OVER;  // ← Changement d'état
        
        // Enregistrer le score
        if (highscore_is_new_highscore(&game->highscores, score.value)) {
            printf("🎉 NOUVEAU HIGHSCORE!\n");
        }
        highscore_add(&game->highscores, game->player_name, score.value);
        
        return;  // Ne pas continuer la logique
    }

    // Vérifier si tous les ennemis ET météorites sont détruits/spawnés
    if (game->enemies.alive_count == 0 && 
        game->enemies.enemies_to_spawn == 0 && 
        game->enemies.asteroids_to_spawn == 0) {
        printf("All enemies destroyed! Starting next wave...\n");
        
        // Afficher "WAVE X"
        game->wave_transition = 1;
        game->wave_transition_time = SDL_GetTicks();
        sprintf(textJeu.TextPrint, "WAVE %d", game->current_wave + 1);  // ← Afficher le PROCHAIN numéro
        game->current_wave++;
    }
}

/* ---- NOUVEAU: Afficher l'écran de saisie du nom ---- */
void game_render_name_input(Game *game) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    
    // Titre
    if (textJeu.font) {
        SDL_Surface *title = TTF_RenderText_Solid(textJeu.font, "SPACE INVADERS", yellow);
        if (title) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, title);
            SDL_Rect rect = {(800 - title->w) / 2, 150, title->w, title->h};
            SDL_RenderCopy(game->renderer, tex, NULL, &rect);
            SDL_FreeSurface(title);
            SDL_DestroyTexture(tex);
        }
    }
    
    // Instructions
    if (score.font) {
        SDL_Surface *prompt = TTF_RenderText_Solid(score.font, "Entrez votre nom:", white);
        if (prompt) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, prompt);
            SDL_Rect rect = {(800 - prompt->w) / 2, 250, prompt->w, prompt->h};
            SDL_RenderCopy(game->renderer, tex, NULL, &rect);
            SDL_FreeSurface(prompt);
            SDL_DestroyTexture(tex);
        }
        
        // Afficher le nom en cours de saisie
        char display_name[MAX_NAME_LENGTH + 2];
        sprintf(display_name, "%s_", game->player_name);
        
        SDL_Surface *name = TTF_RenderText_Solid(textJeu.font, display_name, yellow);
        if (name) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, name);
            SDL_Rect rect = {(800 - name->w) / 2, 300, name->w, name->h};
            SDL_RenderCopy(game->renderer, tex, NULL, &rect);
            SDL_FreeSurface(name);
            SDL_DestroyTexture(tex);
        }
        
        // Instructions
        SDL_Surface *hint = TTF_RenderText_Solid(score.font, "Appuyez sur ENTREE pour commencer", white);
        if (hint) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, hint);
            SDL_Rect rect = {(800 - hint->w) / 2, 380, hint->w, hint->h};
            SDL_RenderCopy(game->renderer, tex, NULL, &rect);
            SDL_FreeSurface(hint);
            SDL_DestroyTexture(tex);
        }
    }
}

/* ---- NOUVEAU: Afficher l'écran de game over ---- */
void game_render_game_over(Game *game) {
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    
    // GAME OVER
    if (textJeu.font) {
        SDL_Surface *title = TTF_RenderText_Solid(textJeu.font, "GAME OVER", red);
        if (title) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, title);
            SDL_Rect rect = {(800 - title->w) / 2, 50, title->w, title->h};
            SDL_RenderCopy(game->renderer, tex, NULL, &rect);
            SDL_FreeSurface(title);
            SDL_DestroyTexture(tex);
        }
    }
    
    // Score final
    if (score.font) {
        char final_score[100];
        sprintf(final_score, "%s - Score: %d", game->player_name, score.value);
        
        SDL_Surface *surf = TTF_RenderText_Solid(textJeu.font, final_score, white);
        if (surf) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, surf);
            SDL_Rect rect = {(800 - surf->w) / 2, 120, surf->w, surf->h};
            SDL_RenderCopy(game->renderer, tex, NULL, &rect);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    }
    
    // Highscores
    highscore_render(&game->highscores, game->renderer, 200, 200);
    
    // Instructions
    if (score.font) {
        SDL_Surface *hint = TTF_RenderText_Solid(score.font, "Appuyez sur ENTREE pour rejouer", white);
        if (hint) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, hint);
            SDL_Rect rect = {(800 - hint->w) / 2, 520, hint->w, hint->h};
            SDL_RenderCopy(game->renderer, tex, NULL, &rect);
            SDL_FreeSurface(hint);
            SDL_DestroyTexture(tex);
        }
    }
}

/* ---- fonction affichage/rendering ---- */
void game_render(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    /* ← NOUVEAU: Rendu selon l'état */
    if (game->state == GAME_STATE_NAME_INPUT) {
        game_render_name_input(game);
    }
    else if (game->state == GAME_STATE_GAME_OVER) {
        game_render_game_over(game);
    }
    else if (game->state == GAME_STATE_PLAYING) {
        /* Transition vague */
        if (game->wave_transition) {
            text_render(&textJeu, game->renderer);
        }
        else {
            /* Rendu normal du jeu */
            player_render(&player, game->renderer);
            enemy_render(&game->enemies, game->renderer);
            bullet_render(bullets, MAX_BULLETS, game->renderer, score.value);
            score_render(&score, game->renderer);
            lives_render(&lives, game->renderer);
        }
    }
    
    /* afficher le frame */
    SDL_RenderPresent(game->renderer);
}

/* ---- fonction nettoyage des ressources du jeu ---- */
void game_cleanup(Game *game) {
    highscore_cleanup(&game->highscores);  // ← Nettoyer highscores
    
    if (game->sprite_manager) {
        destroySpriteManager(game->sprite_manager);
    }
    
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    
    TTF_Quit();  // ← NOUVEAU: Fermer TTF ici
    IMG_Quit();
    SDL_Quit();
}