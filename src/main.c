#include "game.h"
#include "player.h"
#include "bullet.h"
#include "score.h"
#include "text.h"

/* === configuration générale === */
#define CHEMIN_VERS_POLICE "assets/space_invaders.ttf"
#define WIDTH 800
#define HEIGHT 600

/** === objets partagés / globaux === */
Score score;                        // score du joueur
Player player;                      // joueur principal
Bullet bullets[MAX_BULLETS];        // tableau de munitions
TextJeu textJeu;

int previousSpaceState = 0;         // état précédent de la touche ESPACE

    
int main() {

    Game game;
    score.value = 0;

    /** === initialisation du jeu === */
    if (!game_init(&game, "Space Invaders", WIDTH, HEIGHT)){
        return 1;
    }

    
    
    /** === initialisation du joueur === */
    player_init(&player, WIDTH, HEIGHT);

    /** === init enemies (spaces invaders arghhh watch out, here they come!!!!) === */
    enemy_init(&game.enemies, WIDTH, HEIGHT);
    
    /** === initialisation des munitions === */
    bullet_init(bullets, MAX_BULLETS);

    /** === initialisation du score === */
    if (!score_init(&score, CHEMIN_VERS_POLICE, 24)) {
        printf("Erreur chargement de la police\n");
        return 1;
    }

    if (!text_init(&textJeu, CHEMIN_VERS_POLICE, 24)) {
        printf("Erreur chargement de la police\n");
        return 1;
    }

    /** === BOUCLE DE JEU PRINCIPAL === */
    while (game.running) {

        /** --- gestion des événements (SDL) --- */
        game_handle_events(&game);

        /** --- état du clavier --- */
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);

        /** --- mise à jour du joueur --- */
        player_update(&player, keystates);

        /** --- gestion de tir du joueur --- */
        player_handle_shooting(&player, keystates, bullets);



        /** --- mise à jour globale du jeu --- */
        game_update(&game);

        /** --- rendu graphique (joueur, ennemis, tirs, score) --- */
        game_render(&game);  // Le joueur sera dessiné **dans cette fonction**

        /** --- régulation de la vitesse du jeu --- */
        SDL_Delay(16); // ~60 FPS
    }

     /** === nettoyage avant fermeture === */
    game_cleanup(&game);
    text_cleanup(&textJeu);

    return 0;
}