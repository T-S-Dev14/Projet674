#include "game.h"
#include "player.h"

extern Player player;

/* ---- initialisation du jeu ---- */
int game_init(Game *game, const char *title, int width, int height) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

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



void game_update(Game *game) {
    // TODO: gestion du joueur, tirs, ennemis, collisions…

    /* ---- enemy logic ---- */
    enemy_update(&game->enemies);
    
    if (enemy_check_reached_bottom(&game->enemies, 600)) {
        game->running = 0;  // Game over
    }
}



void game_render(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    // Dessiner le joueur
    player_render(&player, game->renderer);

    // Dessiner les enemis
    enemy_render(&game->enemies, game->renderer);

    SDL_RenderPresent(game->renderer);
}



void game_cleanup(Game *game) {
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
}
