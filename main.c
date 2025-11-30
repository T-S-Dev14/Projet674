#include "game.h"
#include "player.h"

Player player; // <<< ajouter ici

int main() {

    Game game;

    if (!game_init(&game, "Space Invaders", 800, 600)){
        return 1;
    }
    
    player_init(&player, 800, 600);

    
    while (game.running) {
        game_handle_events(&game);

        const Uint8 *keystates = SDL_GetKeyboardState(NULL);
        player_update(&player, keystates);

        game_update(&game);

        game_render(&game);  // Le joueur sera dessiné **dans cette fonction**

        SDL_Delay(16); // ~60 FPS
    }

    game_cleanup(&game);
    return 0;
}
