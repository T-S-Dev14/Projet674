#ifndef PICKUP_H
#define PICKUP_H

#include <SDL2/SDL.h>
#include "player.h"  // AJOUTER CETTE LIGNE

#define MAX_PICKUPS 50

typedef enum {
    PICKUP_COIN_GREEN,      // Pièce verte (10 points)
    PICKUP_COIN_YELLOW,     // Pièce jaune (25 points)
    PICKUP_BONUS_HEART,     // +1 vie
    PICKUP_BONUS_GREEN,     // Améliore les balles
    PICKUP_BONUS_BLUE,      // Augmente vitesse joueur
    PICKUP_BONUS_ORANGE,    // Ralentit ennemis
    PICKUP_BONUS_BROWN,     // MALUS: Ralentit joueur
    PICKUP_BOMB            // MALUS: -1 vie
} PickupType;

typedef struct {
    int x, y;
    float speed;
    int active;
    PickupType type;
    int animation_frame;    // 0-2 pour les pièces
    Uint32 animation_timer;
} Pickup;

typedef struct {
    Pickup pickups[MAX_PICKUPS];
    int count;
} PickupManager;

void pickup_init(PickupManager *manager);
void pickup_spawn(PickupManager *manager, int x, int y, PickupType type);
void pickup_update(PickupManager *manager);
void pickup_render(PickupManager *manager, SDL_Renderer *renderer);
int pickup_check_collision(PickupManager *manager, Player *player);

#endif