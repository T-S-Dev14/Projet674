#include "pickup.h"
#include "sprites.h"
#include <stdlib.h>
#include <stdio.h>

extern SpriteManager *g_sprite_manager;

void pickup_init(PickupManager *manager) {
    manager->count = 0;
    for (int i = 0; i < MAX_PICKUPS; i++) {
        manager->pickups[i].active = 0;
    }
}

void pickup_spawn(PickupManager *manager, int x, int y, PickupType type) {
    // Trouver un slot libre
    for (int i = 0; i < MAX_PICKUPS; i++) {
        if (!manager->pickups[i].active) {
            manager->pickups[i].x = x;
            manager->pickups[i].y = y;
            manager->pickups[i].speed = 2.0f;
            manager->pickups[i].active = 1;
            manager->pickups[i].type = type;
            manager->pickups[i].animation_frame = 0;
            manager->pickups[i].animation_timer = SDL_GetTicks();
            manager->count++;
            return;
        }
    }
}

void pickup_update(PickupManager *manager) {
    Uint32 current_time = SDL_GetTicks();
    
    for (int i = 0; i < MAX_PICKUPS; i++) {
        if (!manager->pickups[i].active) continue;
        
        Pickup *p = &manager->pickups[i];
        
        // Déplacer vers le bas
        p->y += p->speed;
        
        // Désactiver si sort de l'écran
        if (p->y > 600) {
            p->active = 0;
            manager->count--;
            continue;
        }
        
        // Animation des pièces (change toutes les 100ms)
        if (p->type == PICKUP_COIN_GREEN || p->type == PICKUP_COIN_YELLOW) {
            if (current_time - p->animation_timer > 100) {
                p->animation_frame = (p->animation_frame + 1) % 3;
                p->animation_timer = current_time;
            }
        }
    }
}

void pickup_render(PickupManager *manager, SDL_Renderer *renderer) {
    if (!g_sprite_manager) return;
    
    for (int i = 0; i < MAX_PICKUPS; i++) {
        if (!manager->pickups[i].active) continue;
        
        Pickup *p = &manager->pickups[i];
        SpriteID sprite;
        
        switch (p->type) {
            case PICKUP_COIN_GREEN:
                // Animation: PIECES_VERTE_1 -> 2 -> 3
                if (p->animation_frame == 0) sprite = PIECES_VERTE_1;
                else if (p->animation_frame == 1) sprite = PIECES_VERTE_2;
                else sprite = PIECES_VERTE_3;
                break;
                
            case PICKUP_COIN_YELLOW:
                // Animation: PIECES_JAUNE_1 -> 2 -> 3
                if (p->animation_frame == 0) sprite = PIECES_JAUNE_1;
                else if (p->animation_frame == 1) sprite = PIECES_JAUNE_2;
                else sprite = PIECES_JAUNE_3;
                break;
                
            case PICKUP_BONUS_HEART:
                sprite = COEUR;
                break;
                
            case PICKUP_BONUS_GREEN:
                sprite = BONUS_VERT;
                break;
                
            case PICKUP_BONUS_BLUE:
                sprite = BONUS_BLEU;
                break;
                
            case PICKUP_BONUS_ORANGE:
                sprite = BONUS_ORANGE;
                break;
                
            case PICKUP_BONUS_BROWN:
                sprite = BONUS_MARRON;
                break;
                
            case PICKUP_BOMB:
                sprite = BOMBE;
                break;
                
            default:
                sprite = PIECES_VERTE_1;
        }
        
        renderSprite(g_sprite_manager, renderer, sprite, p->x, p->y, 3);
    }
}

int pickup_check_collision(PickupManager *manager, Player *player) {
    int player_width = 24;  // 8 * 3
    int player_height = 24;
    int pickup_size = 24;   // 8 * 3
    
    for (int i = 0; i < MAX_PICKUPS; i++) {
        if (!manager->pickups[i].active) continue;
        
        Pickup *p = &manager->pickups[i];
        
        // AABB collision
        if (player->x < p->x + pickup_size &&
            player->x + player_width > p->x &&
            player->y < p->y + pickup_size &&
            player->y + player_height > p->y) {
            
            // Collision détectée !
            int result = p->type;
            p->active = 0;
            manager->count--;
            
            return result;  // Retourne le type de pickup
        }
    }
    
    return -1;  // Pas de collision
}