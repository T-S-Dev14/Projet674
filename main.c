#include "game.h"
#include "player.h"
#include "bullet.h"
#include "score.h"

/* ---- variables globales ----*/
Score score;
Player player;                  // déclaration du joueur 
Bullet bullets[MAX_BULLETS];    // définir le nombre max de BULLETS présent sur l'écran

int previousSpaceState = 0;     // variable temp




int main() {

    Game game;
    score.value = 0;

    if (!game_init(&game, "Space Invaders", 800, 600)){
        return 1;
    }
    

    /* ---- INSTANCIER LE JOUEUR ---- */
    player_init(&player, 800, 600);

    
    /* ---- INSTANCIER LES MUNITIONS DE TIRS ----*/
    bullet_init(bullets, MAX_BULLETS);
    

    if (!score_init(&score, "space_invaders.ttf", 24)) {
        printf("Erreur chargement de la police\n");
        return 1;
    }



    /* ---- BOUCLE DE JEU PRINCIPAL ----*/
    while (game.running) {

        game_handle_events(&game);

        const Uint8 *keystates = SDL_GetKeyboardState(NULL);
        player_update(&player, keystates);

        /* ---- logique tir ------------------------------------*/
        /* (vérifie si on appuye "ESPACE", qui tire des balles) */
        int spacePressed = keystates[SDL_SCANCODE_SPACE];                  
        if (spacePressed && !previousSpaceState) {
            // Tir au centre du vaisseau
            bullet_shoot(bullets, MAX_BULLETS, player.x + 18, player.y);
            score_add(&score, 1); // TEMPORAIRE pour vérifier
        }
        previousSpaceState = spacePressed;


        /* --- mettre à jour le tick du jeu */
        game_update(&game);
        game_render(&game);  // Le joueur sera dessiné **dans cette fonction**


        /* Dessiner les bullets */
        bullet_render(bullets, MAX_BULLETS, game.renderer);

        score_render(&score, game.renderer);

        // Affichage final
        //SDL_RenderPresent(game.renderer);


        SDL_Delay(16); // ~60 FPS
    }

    game_cleanup(&game);
    return 0;
}