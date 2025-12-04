#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "sprites.h"
#include "bullet.h"  // AJOUTER CETTE LIGNE
#include "lives.h"   // AJOUTER CETTE LIGNE

typedef struct {
    int x, y;
    int speed;
    SpriteID current_sprite;
    int flame_frame;
    int flame_timer;
    
    // Système de bonus
    int bullet_level;
    float speed_multiplier;
    Uint32 speed_bonus_end;
    Uint32 slow_debuff_end;
} Player;

void player_init(Player *p, int screenWidth, int screenHeight);
void player_update(Player *p, const Uint8 *keystates);
void player_render(Player *p, SDL_Renderer *renderer);
void player_handle_shooting(Player *p, const Uint8 *keys, Bullet bullets[], int score);
void player_apply_bonus(Player *p, int bonus_type, Lives *lives);
void player_update_with_accel(Player *p);
void player_handle_shooting_hps(Player *p, Bullet bullets[], int score);
#endif