#include "game.h"
#include "player.h"
#include "bullet.h"

/* ---- variables globales ----*/
Player player;                  // déclaration du joueur 
Bullet bullets[MAX_BULLETS];    // définir le nombre max de BULLETS présent sur l'écran

int previousSpaceState = 0;     // variable temp



/* --- main ----*/
int main() {

    /* --- INSTANCIER LE JEU ---- */
    Game game;      

    if (!game_init(&game, "Space Invaders", 800, 600)){
        return 1;
    }
    

    /* ---- INSTANCIER LE JOUEUR ---- */
    player_init(&player, 800, 600);

    
    /* ---- INSTANCIER LES MUNITIONS DE TIRS ----*/
    bullet_init(bullets, MAX_BULLETS);
    


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
        }
        previousSpaceState = spacePressed;


        /* --- mettre à jour le tick du jeu */
        game_update(&game);
        game_render(&game);  // Le joueur sera dessiné **dans cette fonction**


        /* Dessiner les bullets */
        bullet_render(bullets, MAX_BULLETS, game.renderer);

        // Affichage final
        SDL_RenderPresent(game.renderer);


        SDL_Delay(16); // ~60 FPS
    }

    game_cleanup(&game);
    return 0;
}