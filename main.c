#include "game.h"
#include "player.h"
#include "bullet.h"
#include "score.h"

Score score;
Player player; 
Bullet bullets[MAX_BULLETS];

int previousSpaceState = 0;




int main() {

    Game game;
    score.value = 0;

    if (!game_init(&game, "Space Invaders", 800, 600))
        return 1;

    player_init(&player, 800, 600);
    bullet_init(bullets, MAX_BULLETS);

    if (!score_init(&score, "space_invaders.ttf", 24)) {
        printf("Erreur chargement de la police\n");
        return 1;
    }



    while (game.running) {

        game_handle_events(&game);

        const Uint8 *keystates = SDL_GetKeyboardState(NULL);
        player_update(&player, keystates);
        // Espace pour tirer
        int spacePressed = keystates[SDL_SCANCODE_SPACE];

        if (spacePressed && !previousSpaceState) {
            // Tir au centre du vaisseau
            bullet_shoot(bullets, MAX_BULLETS, player.x + 18, player.y);
            score_add(&score, 1); // TEMPORAIRE pour vérifier
        }

        previousSpaceState = spacePressed;

        game_update(&game);

        // Nettoyage + dessin joueur
        game_render(&game);

        // Dessin bullets
        bullet_render(bullets, MAX_BULLETS, game.renderer);

        score_render(&score, game.renderer);

        // Affichage final
        SDL_RenderPresent(game.renderer);


        SDL_Delay(16); // ~60 FPS
    }

    game_cleanup(&game);
    return 0;
}