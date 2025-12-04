#include "game.h"
#include "player.h"
#include "bullet.h"
#include "score.h"
#include "text.h"
#include "lives.h"
#include "pickup.h"
#include "hps_button.h"
#include "DE1-SoC_I2C.h"

/* === configuration générale === */
#define CHEMIN_VERS_POLICE "assets/space_invaders.ttf"
#define WIDTH 800
#define HEIGHT 600

/** === objets partagés / globaux === */
Score score;                        // score du joueur
TextJeu textJeu;                    // texte de transition
Bullet bullets[MAX_BULLETS];        // tableau de munitions
Lives lives;  
Player player;                      // joueur principal (EN DERNIER)

    
int main() {
    Game game;
    
    /** === initialisation du jeu === */
    if (!game_init(&game, "Space Invaders", WIDTH, HEIGHT)){
        return 1;
    }

    SDL_StartTextInput();

    /** === initialisation du score === */
    if (!score_init(&score, CHEMIN_VERS_POLICE, 24)) {
        printf("Erreur chargement de la police pour score\n");
        return 1;
    }
    score.value = 0;  

    /** === initialisation du texte === */
    if (!text_init(&textJeu, CHEMIN_VERS_POLICE, 24)) {
        printf("Erreur chargement de la police pour texte\n");
        return 1;
    }

    if (init_accel() < 0) {
        fprintf(stderr, "Erreur: impossible d'initialiser l'accéléromètre\n");
        return 1;
    }
    printf("Accéléromètre initialisé\n");
    
    // NOUVEAU : Initialiser le bouton HPS
    if (hps_button_init() < 0) {
        fprintf(stderr, "Erreur: impossible d'initialiser le bouton HPS\n");
        fprintf(stderr, "Assurez-vous d'exécuter avec sudo\n");
        return 1;
    }
    printf("Bouton HPS initialisé\n");
    
    lives_init(&lives);

    /** === initialisation du joueur === */
    player_init(&player, WIDTH, HEIGHT);

    /** === init enemies === */
    enemy_init(&game.enemies, WIDTH, HEIGHT);
    
    /** === initialisation des munitions === */
    bullet_init(bullets, MAX_BULLETS);

    /** === BOUCLE DE JEU PRINCIPAL === */
    while (game.running) {

        /** --- gestion des événements (SDL) --- */
        game_handle_events(&game);

        /** --- état du clavier --- */
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);

        /** --- mise à jour du joueur --- */
        //player_update(&player, keystates);
        player_update_with_accel(&player);
        

        /** --- gestion de tir du joueur --- */
        //player_handle_shooting(&player, keystates, bullets, score.value);
        player_handle_shooting_hps(&player, bullets, score.value);

        /** --- mise à jour globale du jeu --- */
        game_update(&game);

        /** --- rendu graphique (joueur, ennemis, tirs, score) --- */
        game_render(&game);

        /** --- régulation de la vitesse du jeu --- */
        SDL_Delay(16); // ~60 FPS
    }
    
    SDL_StopTextInput();
     /** === nettoyage avant fermeture === */
    score_cleanup(&score);
    text_cleanup(&textJeu);
    game_cleanup(&game);
    hps_button_cleanup();

    return 0;
}