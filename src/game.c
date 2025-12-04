#include "game.h"
#include "text.h"
#include "lives.h" 
#include <string.h>

extern Player player;
extern Bullet bullets[MAX_BULLETS];
extern Score score;
extern TextJeu textJeu;
extern Lives lives;  

SpriteManager *g_sprite_manager = NULL;

/* ---- Helper : Dessiner texte centré ---- */
static void render_text_centered(SDL_Renderer *renderer, TTF_Font *font, 
                                 const char *text, int y, SDL_Color color) {
    if (!font) return;
    
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return;
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = {
        (800 - surface->w) / 2,
        y,
        surface->w,
        surface->h
    };
    
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
}

/* ---- Helper : Dessiner un bouton ---- */
static void render_button(SDL_Renderer *renderer, TTF_Font *font, MenuButton *btn) {
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    
    // Bordure
    SDL_Color border_color = btn->selected ? yellow : white;
    SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, 255);
    SDL_RenderDrawRect(renderer, &btn->rect);
    
    // Double bordure si sélectionné
    if (btn->selected) {
        SDL_Rect inner = {btn->rect.x + 2, btn->rect.y + 2, 
                         btn->rect.w - 4, btn->rect.h - 4};
        SDL_RenderDrawRect(renderer, &inner);
    }
    
    // Fond légèrement transparent si sélectionné
    if (btn->selected) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 50);
        SDL_RenderFillRect(renderer, &btn->rect);
    }
    
    // Texte centré dans le bouton
    SDL_Surface *surface = TTF_RenderText_Solid(font, btn->text, 
                                               btn->selected ? yellow : white);
    if (!surface) return;
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect text_rect = {
        btn->rect.x + (btn->rect.w - surface->w) / 2,
        btn->rect.y + (btn->rect.h - surface->h) / 2,
        surface->w,
        surface->h
    };
    
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
    SDL_DestroyTexture(texture);
}

/* ---- Initialisation du jeu ---- */
int game_init(Game *game, const char *title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

    if (TTF_Init() < 0) {
        printf("Erreur initialisation SDL_ttf: %s\n", TTF_GetError());
        return 0;
    }
    
    IMG_Init(IMG_INIT_PNG);
    
    game->window = SDL_CreateWindow(title,
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     width, height, 0);
                                     
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    game->running = 1;
    
    game->sprite_manager = createSpriteManager(game->renderer, CHEMIN_VERS_SPRITE);
    if (!game->sprite_manager) {
        printf("Erreur chargement sprites!\n");
        return 0;
    }
    
    g_sprite_manager = game->sprite_manager;

    if (!highscore_init(&game->highscores, "assets/space_invaders.ttf", 24)) {
        printf("Erreur initialisation highscores\n");
        return 0;
    }
    
    // État initial = MENU
    game->state = GAME_STATE_MENU;
    game->player_name[0] = '\0';
    game->name_length = 0;
    game->current_wave = 1;
    game->wave_transition = 0;
    game->wave_transition_time = 0;
    
    // Initialiser les boutons du menu principal
    game->button_count = 3;
    game->selected_button = 0;
    
    game->menu_buttons[0].rect = (SDL_Rect){250, 250, 300, 60};
    game->menu_buttons[0].text = "START GAME";
    game->menu_buttons[0].selected = 1;
    
    game->menu_buttons[1].rect = (SDL_Rect){250, 330, 300, 60};
    game->menu_buttons[1].text = "HOW TO PLAY";
    game->menu_buttons[1].selected = 0;
    
    game->menu_buttons[2].rect = (SDL_Rect){250, 410, 300, 60};
    game->menu_buttons[2].text = "HIGHSCORES";
    game->menu_buttons[2].selected = 0;
    
    // NOUVEAU : Initialiser les boutons du menu pause
    game->pause_button_count = 3;
    game->selected_pause_button = 0;
    
    game->pause_buttons[0].rect = (SDL_Rect){250, 250, 300, 60};
    game->pause_buttons[0].text = "RESUME";
    game->pause_buttons[0].selected = 1;
    
    game->pause_buttons[1].rect = (SDL_Rect){250, 330, 300, 60};
    game->pause_buttons[1].text = "RESTART";
    game->pause_buttons[1].selected = 0;
    
    game->pause_buttons[2].rect = (SDL_Rect){250, 410, 300, 60};
    game->pause_buttons[2].text = "QUIT TO MENU";
    game->pause_buttons[2].selected = 0;
    
    return 1;
}

/* ---- Démarrer une vague ---- */
void game_start_wave(Game *game, int wave_number) {
    printf("=== WAVE %d START ===\n", wave_number);
    
    pickup_init(&game->pickups);
    enemy_init(&game->enemies, 800, 600);
    enemy_start_wave(&game->enemies, wave_number);
}

/* ---- Gérer le menu principal ---- */
void game_handle_menu_input(Game *game, SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_UP) {
            game->menu_buttons[game->selected_button].selected = 0;
            game->selected_button--;
            if (game->selected_button < 0) {
                game->selected_button = game->button_count - 1;
            }
            game->menu_buttons[game->selected_button].selected = 1;
        }
        else if (event->key.keysym.sym == SDLK_DOWN) {
            game->menu_buttons[game->selected_button].selected = 0;
            game->selected_button++;
            if (game->selected_button >= game->button_count) {
                game->selected_button = 0;
            }
            game->menu_buttons[game->selected_button].selected = 1;
        }
        else if (event->key.keysym.sym == SDLK_RETURN || 
                 event->key.keysym.sym == SDLK_SPACE) {
            if (game->selected_button == 0) {  // START GAME
                game->state = GAME_STATE_NAME_INPUT;
            }
            else if (game->selected_button == 1) {  // HOW TO PLAY
                game->state = GAME_STATE_RULES;
            }
            else if (game->selected_button == 2) {  // HIGHSCORES
                game->state = GAME_STATE_HIGHSCORES_VIEW;
            }
        }
    }
}

/* ---- NOUVEAU : Gérer le menu pause ---- */
void game_handle_pause_input(Game *game, SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_UP) {
            game->pause_buttons[game->selected_pause_button].selected = 0;
            game->selected_pause_button--;
            if (game->selected_pause_button < 0) {
                game->selected_pause_button = game->pause_button_count - 1;
            }
            game->pause_buttons[game->selected_pause_button].selected = 1;
        }
        else if (event->key.keysym.sym == SDLK_DOWN) {
            game->pause_buttons[game->selected_pause_button].selected = 0;
            game->selected_pause_button++;
            if (game->selected_pause_button >= game->pause_button_count) {
                game->selected_pause_button = 0;
            }
            game->pause_buttons[game->selected_pause_button].selected = 1;
        }
        else if (event->key.keysym.sym == SDLK_RETURN || 
                 event->key.keysym.sym == SDLK_SPACE) {
            if (game->selected_pause_button == 0) {  // RESUME
                game->state = GAME_STATE_PLAYING;
            }
            else if (game->selected_pause_button == 1) {  // RESTART
                // Réinitialiser tout
                game->state = GAME_STATE_NAME_INPUT;
                game->player_name[0] = '\0';
                game->name_length = 0;
                game->current_wave = 1;
                score.value = 0;
                lives.current_lives = MAX_LIVES;
                
                // Nettoyer les entités
                for (int i = 0; i < MAX_BULLETS; i++) {
                    bullets[i].active = 0;
                }
                for (int i = 0; i < game->enemies.count; i++) {
                    game->enemies.enemies[i].alive = 0;
                }
                game->enemies.alive_count = 0;
            }
            else if (game->selected_pause_button == 2) {  // QUIT TO MENU
                // Retour au menu sans sauvegarder
                game->state = GAME_STATE_MENU;
                game->player_name[0] = '\0';
                game->name_length = 0;
                game->current_wave = 1;
                
                // Nettoyer les entités
                for (int i = 0; i < MAX_BULLETS; i++) {
                    bullets[i].active = 0;
                }
                for (int i = 0; i < game->enemies.count; i++) {
                    game->enemies.enemies[i].alive = 0;
                }
                game->enemies.alive_count = 0;
            }
        }
        else if (event->key.keysym.sym == SDLK_ESCAPE || 
                 event->key.keysym.sym == SDLK_p) {
            // Reprendre direct avec ESC ou P
            game->state = GAME_STATE_PLAYING;
        }
    }
}

/* ---- Gérer la saisie du nom ---- */
void game_handle_name_input(Game *game, SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_ESCAPE) {
            game->state = GAME_STATE_MENU;
            game->player_name[0] = '\0';
            game->name_length = 0;
        }
        else if (event->key.keysym.sym == SDLK_RETURN && game->name_length > 0) {
            game->player_name[game->name_length] = '\0';
            game->state = GAME_STATE_PLAYING;
            
            score.value = 0;
            game->current_wave = 1;
            lives.current_lives = MAX_LIVES;
            
            game_start_wave(game, 1);
            printf("Joueur: %s - Début du jeu!\n", game->player_name);
        }
        else if (event->key.keysym.sym == SDLK_BACKSPACE && game->name_length > 0) {
            game->name_length--;
            game->player_name[game->name_length] = '\0';
        }
    }
    else if (event->type == SDL_TEXTINPUT) {
        if (game->name_length < MAX_NAME_LENGTH - 1) {
            strcat(game->player_name, event->text.text);
            game->name_length = strlen(game->player_name);
        }
    }
}

/* ---- Gérer les événements ---- */
void game_handle_events(Game *game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            game->running = 0;
        
        if (game->state == GAME_STATE_MENU) {
            game_handle_menu_input(game, &event);
        }
        else if (game->state == GAME_STATE_PAUSED) {
            game_handle_pause_input(game, &event);
        }
        else if (game->state == GAME_STATE_NAME_INPUT) {
            game_handle_name_input(game, &event);
        }
        else if (game->state == GAME_STATE_RULES || 
                 game->state == GAME_STATE_HIGHSCORES_VIEW) {
            if (event.type == SDL_KEYDOWN) {
                game->state = GAME_STATE_MENU;
            }
        }
        else if (game->state == GAME_STATE_GAME_OVER) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                game->state = GAME_STATE_MENU;
                game->player_name[0] = '\0';
                game->name_length = 0;
                game->current_wave = 1;
            }
        }
        else if (game->state == GAME_STATE_PLAYING) {
            // NOUVEAU : Appuyer sur P ou ESC pour mettre en pause
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_p || 
                    event.key.keysym.sym == SDLK_ESCAPE) {
                    game->state = GAME_STATE_PAUSED;
                    // Réinitialiser la sélection du menu pause
                    game->selected_pause_button = 0;
                    for (int i = 0; i < game->pause_button_count; i++) {
                        game->pause_buttons[i].selected = (i == 0);
                    }
                }
            }
        }
    }
}

/* ---- Mise à jour du jeu ---- */
void game_update(Game *game) {
    if (game->state != GAME_STATE_PLAYING) {
        return;
    }

    if (game->wave_transition) {
        Uint32 current_time = SDL_GetTicks();
        
        if (current_time - game->wave_transition_time > 2000) {
            game->wave_transition = 0;
            text_clear(&textJeu);
            game_start_wave(game, game->current_wave);
        }
        
        return;
    }
    
    bullet_update(bullets, MAX_BULLETS);
    bullet_check_collisions(bullets, MAX_BULLETS, &game->enemies, &score);
    
    if (game->enemies.has_pending_pickup) {
        pickup_spawn(&game->pickups, 
                    game->enemies.pending_pickup_x, 
                    game->enemies.pending_pickup_y,
                    game->enemies.pending_pickup_type);
        game->enemies.has_pending_pickup = 0;
    }

    pickup_update(&game->pickups);

    int pickup_result = pickup_check_collision(&game->pickups, &player);
    if (pickup_result >= 0) {
        if (pickup_result == PICKUP_COIN_GREEN) {
            score_add(&score, 10);
            printf("Pièce verte ramassée ! +10 points\n");
        } else if (pickup_result == PICKUP_COIN_YELLOW) {
            score_add(&score, 25);
            printf("Pièce jaune ramassée ! +25 points\n");
        } else {
            player_apply_bonus(&player, pickup_result, &lives);
        }
    }
    
    enemy_update(&game->enemies, 800);

    if (lives_check_danger(&lives, &game->enemies, 600)) {
        printf("GAME OVER! No more lives!\n");
        
        game->state = GAME_STATE_GAME_OVER;
        
        for (int i = 0; i < MAX_BULLETS; i++) {
            bullets[i].active = 0;
        }
        
        for (int i = 0; i < game->enemies.count; i++) {
            game->enemies.enemies[i].alive = 0;
        }
        game->enemies.alive_count = 0;
        
        lives.danger_warning = 0;
        text_clear(&textJeu);
        
        if (highscore_is_new_highscore(&game->highscores, score.value)) {
            printf("🎉 NOUVEAU HIGHSCORE!\n");
        }
        highscore_add(&game->highscores, game->player_name, score.value);
        
        return;
    }

    if (game->enemies.alive_count == 0 && 
        game->enemies.enemies_to_spawn == 0 && 
        game->enemies.asteroids_to_spawn == 0) {
        
        printf("All enemies destroyed! Starting next wave...\n");
        
        game->wave_transition = 1;
        game->wave_transition_time = SDL_GetTicks();
        sprintf(textJeu.TextPrint, "WAVE %d", game->current_wave + 1);
        game->current_wave++;
    }
}

/* ---- Afficher le menu principal ---- */
void game_render_menu(Game *game) {
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {0, 255, 0, 255};
    
    render_text_centered(game->renderer, textJeu.font, 
                        "SPACE INVADERS", 80, yellow);
    
    render_text_centered(game->renderer, score.font, 
                        "Defend Earth from alien invasion!", 140, white);
    
    for (int i = 0; i < game->button_count; i++) {
        render_button(game->renderer, textJeu.font, &game->menu_buttons[i]);
    }
    
    render_text_centered(game->renderer, score.font, 
                        "Use UP/DOWN and ENTER", 500, green);
}

/* ---- NOUVEAU : Afficher le menu pause ---- */
void game_render_pause(Game *game) {
    // D'abord dessiner le jeu en arrière-plan (figé)
    player_render(&player, game->renderer);
    enemy_render(&game->enemies, game->renderer);
    bullet_render(bullets, MAX_BULLETS, game->renderer, player.bullet_level);
    pickup_render(&game->pickups, game->renderer);
    score_render(&score, game->renderer);
    lives_render(&lives, game->renderer);
    
    // Overlay semi-transparent
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, 800, 600};
    SDL_RenderFillRect(game->renderer, &overlay);
    
    // Titre "PAUSED"
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color green = {0, 255, 0, 255};
    
    render_text_centered(game->renderer, textJeu.font, 
                        "PAUSED", 80, yellow);
    
    // Boutons
    for (int i = 0; i < game->pause_button_count; i++) {
        render_button(game->renderer, textJeu.font, &game->pause_buttons[i]);
    }
    
    render_text_centered(game->renderer, score.font, 
                        "Use UP/DOWN and ENTER | Press P or ESC to resume", 
                        500, green);
}

/* ---- Afficher les règles ---- */
void game_render_rules(Game *game) {
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {0, 255, 0, 255};
    
    render_text_centered(game->renderer, textJeu.font, 
                        "HOW TO PLAY", 50, yellow);
    
    if (score.font) {
        render_text_centered(game->renderer, score.font, 
                            "ARROW KEYS : Move your ship", 150, white);
        render_text_centered(game->renderer, score.font, 
                            "SPACE : Shoot", 190, white);
        render_text_centered(game->renderer, score.font, 
                            "P or ESC : Pause game", 230, white);
        
        render_text_centered(game->renderer, score.font, 
                            "Destroy all enemies to advance to the next wave!", 300, green);
        render_text_centered(game->renderer, score.font, 
                            "Collect green coins (+10) and yellow coins (+25)", 340, green);
        render_text_centered(game->renderer, score.font, 
                            "Watch out for asteroids and boss enemies!", 380, green);
        render_text_centered(game->renderer, score.font, 
                            "Don't let enemies reach the bottom!", 420, green);
        
        render_text_centered(game->renderer, score.font, 
                            "Press any key to return to menu", 520, yellow);
    }
}

/* ---- Afficher les highscores ---- */
void game_render_highscores_view(Game *game) {
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    
    render_text_centered(game->renderer, textJeu.font, 
                        "HIGHSCORES", 50, yellow);
    
    highscore_render(&game->highscores, game->renderer, 250, 120);
    
    render_text_centered(game->renderer, score.font, 
                        "Press any key to return to menu", 520, white);
}

/* ---- Afficher l'écran de saisie du nom ---- */
void game_render_name_input(Game *game) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    
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
    
    if (score.font) {
        SDL_Surface *prompt = TTF_RenderText_Solid(score.font, "Enter your name:", white);
        if (prompt) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, prompt);
            SDL_Rect rect = {(800 - prompt->w) / 2, 250, prompt->w, prompt->h};
            SDL_RenderCopy(game->renderer, tex, NULL, &rect);
            SDL_FreeSurface(prompt);
            SDL_DestroyTexture(tex);
        }
        
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
        
        SDL_Surface *hint = TTF_RenderText_Solid(score.font, "Press ENTER to start or ESC to go back", white);
        if (hint) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, hint);
            SDL_Rect rect = {(800 - hint->w) / 2, 380, hint->w, hint->h};
            SDL_RenderCopy(game->renderer, tex, NULL, &rect);
            SDL_FreeSurface(hint);
            SDL_DestroyTexture(tex);
        }
    }
}

/* ---- Afficher l'écran de game over ---- */
void game_render_game_over(Game *game) {
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    
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
    
    highscore_render(&game->highscores, game->renderer, 250, 200);
    
    if (score.font) {
        SDL_Surface *hint = TTF_RenderText_Solid(score.font, "Press ENTER to return to menu", white);
        if (hint) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, hint);
            SDL_Rect rect = {(800 - hint->w) / 2, 520, hint->w, hint->h};
            SDL_RenderCopy(game->renderer, tex, NULL, &rect);
            SDL_FreeSurface(hint);
            SDL_DestroyTexture(tex);
        }
    }
}

/* ---- Rendu ---- */
void game_render(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    if (game->state == GAME_STATE_MENU) {
        game_render_menu(game);
    }
    else if (game->state == GAME_STATE_RULES) {
        game_render_rules(game);
    }
    else if (game->state == GAME_STATE_HIGHSCORES_VIEW) {
        game_render_highscores_view(game);
    }
    else if (game->state == GAME_STATE_PAUSED) {
        game_render_pause(game);
    }
    else if (game->state == GAME_STATE_NAME_INPUT) {
        game_render_name_input(game);
    }
    else if (game->state == GAME_STATE_GAME_OVER) {
        game_render_game_over(game);
    }
    else if (game->state == GAME_STATE_PLAYING) {
        if (game->wave_transition) {
            text_render(&textJeu, game->renderer);
        }
        else {
            player_render(&player, game->renderer);
            enemy_render(&game->enemies, game->renderer);
            bullet_render(bullets, MAX_BULLETS, game->renderer, player.bullet_level);
            pickup_render(&game->pickups, game->renderer);
            score_render(&score, game->renderer);
            lives_render(&lives, game->renderer);
        }
    }
    
    SDL_RenderPresent(game->renderer);
}

/* ---- Nettoyage ---- */
void game_cleanup(Game *game) {
    highscore_cleanup(&game->highscores);
    
    if (game->sprite_manager) {
        destroySpriteManager(game->sprite_manager);
    }
    
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}