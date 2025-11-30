#include "game.h"
#include "player.h"
#include "bullet.h"

Player player; 
Bullet bullets[MAX_BULLETS];

int main() {

    Game game;

    if (!game_init(&game, "Space Invaders", 800, 600))
        return 1;

    player_init(&player, 800, 600);
    bullet_init(bullets, MAX_BULLETS);


    while (game.running) {

        game_handle_events(&game);

        const Uint8 *keystates = SDL_GetKeyboardState(NULL);
        player_update(&player, keystates);
        // Espace pour tirer
        if (keystates[SDL_SCANCODE_SPACE]) {
            bullet_shoot(bullets, MAX_BULLETS, player.x + 18, player.y);
        }


        game_update(&game);

        // Nettoyage + dessin joueur
        game_render(&game);

        // Dessin bullets
        bullet_render(bullets, MAX_BULLETS, game.renderer);

        // Affichage final
        SDL_RenderPresent(game.renderer);


        SDL_Delay(16); // ~60 FPS
    }

    game_cleanup(&game);
    return 0;
}