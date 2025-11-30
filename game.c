#include "game.h"
#include "player.h"
#include "bullet.h"

extern Player player;
extern Bullet bullets[MAX_BULLETS];


/* ---- initialisation du jeu ---- */
int game_init(Game *game, const char *title, int width, int height) {
    /* ---- Logique de SDL  ----*/
    // vérifier si SDL s'est bien lancé
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

    // taille de la fenêtre
    game->window = SDL_CreateWindow(title,
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    width, height,
                                    0);

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    game->running = 1;


    /* ---- init enemies (spaces invaders arghhh watch out, here they come!!!!) ---- */
    enemy_init(&game->enemies, width, height);

    return 1;
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
    }
}



/* ---- fonction qui met à jour le jeu  ---- */
void game_update(Game *game) {
    // TODO: gestion du joueur, tirs, ennemis, collisions…

    /* ---- bullet logic ---- */
    bullet_update(bullets, MAX_BULLETS);

    /* ---- enemy logic ---- */
    enemy_update(&game->enemies);
    
    if (enemy_check_reached_bottom(&game->enemies, 600)) {
        game->running = 0;  // Game over
    }
}


/* ---- fonction affichage/rendering ---- */
void game_render(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    /* ----Dessiner le joueur ---- */
    player_render(&player, game->renderer);

    /* ---- Dessiner les enemies ----*/
    enemy_render(&game->enemies, game->renderer);

    /* afficher le frame */
    SDL_RenderPresent(game->renderer);
}



/* ---- fonction nettoyage des ressources du jeu lorqu'on quitte le jeu ---- */
void game_cleanup(Game *game) {
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
}