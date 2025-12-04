#include "player.h"
#include "bullet.h"
#include "pickup.h"
#include "lives.h"
#include "hps_button.h"      // NOUVEAU
#include "DE1-SoC_I2C.h"     // NOUVEAU

// Déclaration externe du sprite manager global
extern SpriteManager *g_sprite_manager;

// Seuils pour l'accéléromètre (à ajuster selon vos tests)
#define ACCEL_THRESHOLD 0.15f  // Seuil de détection d'inclinaison
#define ACCEL_MAX 1.0f         // Valeur max pour normaliser

void player_init(Player *p, int screenWidth, int screenHeight) {
    p->x = screenWidth / 2;
    p->y = screenHeight - 50;
    p->speed = 20;
    p->current_sprite = VAISSEAU_1;
    p->flame_frame = 0;
    p->flame_timer = 0;
    
    // NOUVEAU
    p->bullet_level = 0;
    p->speed_multiplier = 1.0f;
    p->speed_bonus_end = 0;
    p->slow_debuff_end = 0;
}

void player_update_with_accel(Player *p) {
    // Gérer les effets temporaires
    Uint32 current_time = SDL_GetTicks();
    
    if (current_time > p->speed_bonus_end) {
        p->speed_multiplier = 1.0f;
    }
    if (current_time > p->slow_debuff_end) {
        p->speed_multiplier = 1.0f;
    }
    
    // Vitesse effective
    int effective_speed = (int)(p->speed * p->speed_multiplier);
    
    // Lire l'accéléromètre (axe X pour gauche/droite)
    float accel_x = read_accel_x();
    
    // Déplacer le joueur selon l'inclinaison
    if (accel_x < -ACCEL_THRESHOLD) {
        // Incliné à gauche
        float intensity = (-accel_x - ACCEL_THRESHOLD) / (ACCEL_MAX - ACCEL_THRESHOLD);
        if (intensity > 1.0f) intensity = 1.0f;
        
        p->x -= (int)(effective_speed * intensity);
        p->current_sprite = VAISSEAU_2;
    }
    else if (accel_x > ACCEL_THRESHOLD) {
        // Incliné à droite
        float intensity = (accel_x - ACCEL_THRESHOLD) / (ACCEL_MAX - ACCEL_THRESHOLD);
        if (intensity > 1.0f) intensity = 1.0f;
        
        p->x += (int)(effective_speed * intensity);
        p->current_sprite = VAISSEAU_3;
    }
    else {
        // Position neutre
        p->current_sprite = VAISSEAU_1;
    }
    
    // Limiter aux bords
    if (p->x < 0) p->x = 0;
    if (p->x > 776) p->x = 776; // 800 - 24

    // Animation de la flamme
    p->flame_timer++;
    if (p->flame_timer >= 10) {
        p->flame_frame = (p->flame_frame + 1) % 3;
        p->flame_timer = 0;
    }
}

void player_update(Player *p, const Uint8 *keystates) {
    // Gérer les effets temporaires
    Uint32 current_time = SDL_GetTicks();
    
    if (current_time > p->speed_bonus_end) {
        p->speed_multiplier = 1.0f;
    }
    if (current_time > p->slow_debuff_end) {
        p->speed_multiplier = 1.0f;
    }
    
    // Vitesse effective
    int effective_speed = (int)(p->speed * p->speed_multiplier);
    
    // Changer le sprite selon la direction
    if (keystates[SDL_SCANCODE_LEFT]) {
        p->x -= effective_speed;
        p->current_sprite = VAISSEAU_2;
    }
    else if (keystates[SDL_SCANCODE_RIGHT]) {
        p->x += effective_speed;
        p->current_sprite = VAISSEAU_3;
    }
    else {
        p->current_sprite = VAISSEAU_1;
    }
    
    // Limiter aux bords
    if (p->x < 0) p->x = 0;
    if (p->x > 776) p->x = 776; // 800 - 24

    // Animation de la flamme
    p->flame_timer++;
    if (p->flame_timer >= 10) {
        p->flame_frame = (p->flame_frame + 1) % 3;
        p->flame_timer = 0;
    }
}

void player_render(Player *p, SDL_Renderer *renderer) {
    if (g_sprite_manager) {
        // ----- DESSINER LA FLAMME -----
        SpriteID flame_sprite;
        switch (p->flame_frame) {
            case 0: flame_sprite = FLAMME_1; break;
            case 1: flame_sprite = FLAMME_2; break;
            case 2: flame_sprite = FLAMME_3; break;
            default: flame_sprite = FLAMME_1; break;
        }

        // Flamme en dessous du vaisseau
        renderSprite(g_sprite_manager, renderer, flame_sprite, p->x, p->y + 20, 3);

        // Vaisseau par-dessus
        renderSprite(g_sprite_manager, renderer, p->current_sprite, p->x, p->y, 3);
    } else {
        // Fallback: rectangle vert
        SDL_Rect rect = { p->x, p->y, 40, 20 };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void player_handle_shooting_hps(Player *p, Bullet bullets[], int score) {
    static int prev_button = 0;
    int button = hps_button_is_pressed();
    
    // Détection front montant (bouton vient d'être appuyé)
    if (button && !prev_button) {
        int damage;
        
        switch (p->bullet_level) {
            case 0:
                damage = 5;  // BALLES_1
                break;
            case 1:
                damage = 8;  // BALLES_2
                break;
            case 2:
                damage = 10; // BALLES_3
                break;
            default:
                damage = 5;
        }
        
        bullet_shoot(bullets, MAX_BULLETS, p->x + 4, p->y, damage);
    }
    
    prev_button = button;
}


void player_handle_shooting(Player *p, const Uint8 *keys, Bullet bullets[], int score) {
    static int prev_space = 0;
    int space = keys[SDL_SCANCODE_SPACE];
    
    if (space && !prev_space) {
        int damage;
        
        // Dégâts basés sur bullet_level au lieu du score
        switch (p->bullet_level) {
            case 0:
                damage = 5;  // BALLES_1
                break;
            case 1:
                damage = 8;  // BALLES_2
                break;
            case 2:
                damage = 10; // BALLES_3
                break;
            default:
                damage = 5;
        }
        
        bullet_shoot(bullets, MAX_BULLETS, p->x + 4, p->y, damage);
    }
    
    prev_space = space;
}

void player_apply_bonus(Player *p, int bonus_type, Lives *lives) {
    Uint32 current_time = SDL_GetTicks();
    
    switch (bonus_type) {
        case PICKUP_BONUS_HEART:
            // +1 vie (max 3)
            if (lives->current_lives < lives->max_lives) {
                lives->current_lives++;
                printf("Bonus coeur ! Vies: %d\n", lives->current_lives);
            }
            break;
            
        case PICKUP_BONUS_GREEN:
            // Améliorer les balles (max niveau 2)
            if (p->bullet_level < 2) {
                p->bullet_level++;
                printf("Balles améliorées ! Niveau: %d\n", p->bullet_level);
            }
            break;
            
        case PICKUP_BONUS_BLUE:
            // Vitesse x1.5 pendant 5 secondes
            p->speed_multiplier = 1.5f;
            p->speed_bonus_end = current_time + 5000;
            p->slow_debuff_end = 0;  // Annule le malus
            printf("Speed boost activé !\n");
            break;
            
        case PICKUP_BONUS_ORANGE:
            // Ralentir ennemis (géré dans enemy.c)
            printf("Ralentissement ennemis activé !\n");
            break;
            
        case PICKUP_BONUS_BROWN:
            // MALUS: Vitesse x0.5 pendant 3 secondes
            p->speed_multiplier = 0.5f;
            p->slow_debuff_end = current_time + 3000;
            p->speed_bonus_end = 0;  // Annule le bonus
            printf("MALUS: Ralenti !\n");
            break;
            
        case PICKUP_BOMB:
            // MALUS: -1 vie
            lives_lose_one(lives);
            printf("BOMBE ! Vie perdue !\n");
            break;
            
        default:
            break;
    }
}