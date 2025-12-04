#include "enemy.h"
#include "pickup.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

extern SpriteManager *g_sprite_manager;

#define ENEMY_ANIMATION_SPEED 250

/* ---- Fonction helper : créer un ennemi selon son type ---- */
static void create_enemy(Enemy *e, EnemyType type, int x, int y, float base_speed) {
    e->x = x;
    e->y = y;
    e->alive = 1;
    e->sprite_frame = 0;
    e->type = type;
    
    // Définir les caractéristiques selon le type
    switch (type) {
        // Petits aliens : 10 HP, 10 points, 8x8 sprite échelle 3
        case ENEMY_SMALL_GREEN:
        case ENEMY_SMALL_ORANGE:
        case ENEMY_SMALL_RED:
        case ENEMY_SMALL_YELLOW:
            e->max_hp = 10;
            e->hp = 10;
            e->score_value = 10;
            e->width = 24;   // 8 * 3
            e->height = 24;
            e->speed = base_speed;
            break;
            
        // Gros aliens : 20 HP, 30 points, 16x16 sprite échelle 3
        case ENEMY_BIG_GREEN:
        case ENEMY_BIG_ORANGE:
        case ENEMY_BIG_GREY:
            e->max_hp = 20;
            e->hp = 20;
            e->score_value = 30;
            e->width = 48;   // 16 * 3
            e->height = 48;
            e->speed = base_speed * 0.8f;  // Plus lents
            break;
            
        // Boss : 40 HP, 100 points, 32x24 sprite échelle 3
        case ENEMY_BOSS:
            e->max_hp = 60;
            e->hp = 60;
            e->score_value = 100;
            e->width = 96;   // 32 * 3
            e->height = 72;  // 24 * 3
            e->speed = base_speed * 0.5f;  // Très lent
            break;
            
        case ENEMY_ASTEROID_SMALL:
            e->max_hp = 20;
            e->hp = 20;
            e->score_value = 5;
            e->width = 24;   // 8 * 3
            e->height = 24;
            e->speed = base_speed;  // Très lentes
            break;
            
        case ENEMY_ASTEROID_BIG:
            e->max_hp = 40;
            e->hp = 40;
            e->score_value = 15;
            e->width = 48;   // 16 * 3
            e->height = 48;
            e->speed = base_speed;  // Extrêmement lentes
            break;
    }
}

/* ---- Initialiser le système d'ennemis ---- */
void enemy_init(EnemyGrid *grid, int screenWidth, int screenHeight) {
    memset(grid, 0, sizeof(EnemyGrid));
    
    grid->count = 0;
    grid->alive_count = 0;
    grid->wave_number = 0;
    grid->animation_time = SDL_GetTicks();
    
    // Initialiser le générateur aléatoire
    srand(time(NULL));
    
    (void)screenWidth;
    (void)screenHeight;
}

/* ---- Démarrer une nouvelle vague ---- */
void enemy_start_wave(EnemyGrid *grid, int wave_number) {
    grid->wave_number = wave_number;
    
    // Nombre d'ennemis selon la vague
    // Vague 1: 10, Vague 2: 15, Vague 3: 20, etc.
    grid->enemies_to_spawn = 5 + (wave_number * 5);
    
    // Vague 1: 2, Vague 2: 3, Vague 3: 4, etc.
    grid->asteroids_to_spawn = 1 + wave_number;
    
    // Délai entre spawns diminue avec les vagues
    // Vague 1: 1000ms, Vague 2: 800ms, Vague 3: 600ms
    grid->spawn_delay = 1200 - (wave_number * 100);
    if (grid->spawn_delay < 300) grid->spawn_delay = 300;  // Minimum 300ms
    
    grid->last_spawn_time = SDL_GetTicks();
    
    printf("=== WAVE %d START ===\n", wave_number);
    printf("Enemies to spawn: %d\n", grid->enemies_to_spawn);
    printf("Asteroids to spawn: %d\n", grid->asteroids_to_spawn);
    printf("Spawn delay: %dms\n", grid->spawn_delay);
}

/* ---- Faire apparaître un ennemi ---- */
void enemy_spawn_one(EnemyGrid *grid, int screenWidth) {
    if (grid->count >= 100) return;  // Limite max
    
    // Vérifier s'il reste des choses à spawner
    int total_remaining = grid->enemies_to_spawn + grid->asteroids_to_spawn;
    if (total_remaining <= 0) return;
    
    Enemy *e = &grid->enemies[grid->count];
    
    // Position X aléatoire en haut de l'écran
    int x = 50 + (rand() % (screenWidth - 100));
    int y = -50;  // Commence au-dessus de l'écran
    
    // Vitesse de base augmente avec les vagues
    float base_speed = 1.0f + (grid->wave_number * 0.3f);
    
    int spawn_asteroid = 0;
    
    if (grid->asteroids_to_spawn > 0 && grid->enemies_to_spawn > 0) {
        // Les deux sont disponibles : 30% de chance de météorite
        spawn_asteroid = (rand() % 100) < 30;
    } else if (grid->asteroids_to_spawn > 0) {
        // Plus que des météorites
        spawn_asteroid = 1;
    }
    // Sinon spawn_asteroid reste 0 (ennemi normal)
    
    EnemyType type;
    
    if (spawn_asteroid) {
        int asteroid_type = rand() % 100;
        if (asteroid_type < 40) {
            type = ENEMY_ASTEROID_SMALL;  // 40% petite
        } else {
            type = ENEMY_ASTEROID_BIG;    // 60% grosse
        }
        grid->asteroids_to_spawn--;
        
    } else {
        // Spawner un ennemi normal
        int random = rand() % 100;
        
        if (grid->wave_number >= 5 && random < 5) {
            // 5% de chance de boss à partir de la vague 5
            type = ENEMY_BOSS;
        } else if (grid->wave_number >= 3 && random < 30) {
            // 30% de gros aliens à partir de la vague 3
            int big_type = rand() % 3;
            type = ENEMY_BIG_GREEN + big_type;
        } else {
            // Petits aliens (différentes couleurs)
            int small_type = rand() % 4;
            type = ENEMY_SMALL_GREEN + small_type;
        }
        grid->enemies_to_spawn--;
    }
    
    create_enemy(e, type, x, y, base_speed);
    
    grid->count++;
    grid->alive_count++;
}

/* ---- Mettre à jour les ennemis ---- */
void enemy_update(EnemyGrid *grid, int screenWidth) {
    Uint32 current_time = SDL_GetTicks();
    
    // Animation
    if (current_time - grid->animation_time > ENEMY_ANIMATION_SPEED) {
        for (int i = 0; i < grid->count; i++) {
            if (grid->enemies[i].alive) {
                grid->enemies[i].sprite_frame = 
                    (grid->enemies[i].sprite_frame + 1) % 2;
            }
        }
        grid->animation_time = current_time;
    }
    
    // Spawn de nouveaux ennemis
    if ((grid->enemies_to_spawn > 0 || grid->asteroids_to_spawn > 0) && 
        current_time - grid->last_spawn_time > grid->spawn_delay) {
        enemy_spawn_one(grid, screenWidth);
        grid->last_spawn_time = current_time;
    }
    
    // Déplacer tous les ennemis vivants
    for (int i = 0; i < grid->count; i++) {
       //printf("nombre %d ennemie %d \n", i, grid->enemies[i].speed);
        if (!grid->enemies[i].alive) continue;
        
        Enemy *e = &grid->enemies[i];
        e->y += e->speed;  // Descendre verticalement
    }
}

/* ---- Afficher les ennemis ---- */
void enemy_render(EnemyGrid *grid, SDL_Renderer *renderer) {
    for (int i = 0; i < grid->count; i++) {
        if (!grid->enemies[i].alive) continue;
        
        Enemy *e = &grid->enemies[i];
        int render_x = (int)e->x;
        int render_y = (int)e->y;
        
        if (g_sprite_manager) {
            // Choisir le sprite selon le type et l'animation
            SpriteID sprite;
            int scale = 3;  // Par défaut pour petits
            
            switch (e->type) {
                case ENEMY_SMALL_GREEN:
                    sprite = (e->sprite_frame == 0) ? ALIEN_VERT_1 : ALIEN_VERT_2;
                    scale = 3;
                    break;
                case ENEMY_SMALL_ORANGE:
                    sprite = (e->sprite_frame == 0) ? ALIEN_ORANGE_1 : ALIEN_ORANGE_2;
                    scale = 3;
                    break;
                case ENEMY_SMALL_RED:
                    sprite = (e->sprite_frame == 0) ? ALIEN_ROUGE_1 : ALIEN_ROUGE_2;
                    scale = 3;
                    break;
                case ENEMY_SMALL_YELLOW:
                    sprite = (e->sprite_frame == 0) ? ALIEN_JAUNE_1 : ALIEN_JAUNE_2;
                    scale = 3;
                    break;
                case ENEMY_BIG_GREEN:
                    sprite = GROS_ALIEN_VERT;
                    scale = 3; 
                    break;
                case ENEMY_BIG_ORANGE:
                    sprite = GROS_ALIEN_ORANGE;
                    scale = 3;
                    break;
                case ENEMY_BIG_GREY:
                    sprite = GROS_ALIEN_GRIS;
                    scale = 3; 
                    break;
                case ENEMY_BOSS:
                    sprite = BOSS;
                    scale = 3; 
                    break;
                case ENEMY_ASTEROID_SMALL:
                    // Alterner entre ASTEROID_3 et ASTEROID_4 (8x8)
                    sprite = (e->sprite_frame == 0) ? ASTEROID_3 : ASTEROID_4;
                    scale = 3;
                    break;
                case ENEMY_ASTEROID_BIG:
                    // Alterner entre ASTEROID_1 et ASTEROID_2 (16x16)
                    sprite = (e->sprite_frame == 0) ? ASTEROID_1 : ASTEROID_2;
                    scale = 3;
                    break;
                default:
                    sprite = ALIEN_VERT_1;
                    scale = 3;
            }
            
            renderSprite(g_sprite_manager, renderer, sprite, render_x, render_y, scale);
            
            if (e->hp < e->max_hp) {
                int bar_width = e->width;
                int bar_height = 6;  // Barre plus épaisse
                int bar_x = render_x;
                int bar_y = render_y - 10;  // 10px au-dessus
                
                // Barre de fond (rouge)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_Rect bg = {bar_x, bar_y, bar_width, bar_height};
                SDL_RenderFillRect(renderer, &bg);
                
                // Barre de vie (vert)
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                int hp_width = (e->hp * bar_width) / e->max_hp;
                SDL_Rect hp_bar = {bar_x, bar_y, hp_width, bar_height};
                SDL_RenderFillRect(renderer, &hp_bar);
                
                // Contour noir pour mieux voir
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &bg);
            }
            
        } else {
            // Fallback: rectangles colorés
            if (e->type <= ENEMY_SMALL_YELLOW) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Jaune
            } else if (e->type <= ENEMY_BIG_GREY) {
                SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);  // Orange
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);    // Rouge
            }
            
            SDL_Rect rect = {render_x, render_y, e->width, e->height};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

/* ---- Vérifier collision avec dégâts ---- */
int enemy_check_collision(EnemyGrid *grid, int x, int y, int width, int height, int damage) {
    for (int i = 0; i < grid->count; i++) {
        if (!grid->enemies[i].alive) continue;
        
        Enemy *e = &grid->enemies[i];

        int enemy_x = (int)e->x;
        int enemy_y = (int)e->y;
        
        // AABB collision detection
        if (x < enemy_x+ e->width &&
            x + width > enemy_x &&
            y < enemy_y + e->height &&
            y + height > enemy_y) {
            
            // Infliger des dégâts
            e->hp -= damage;
            
            // Mort si HP <= 0
            if (e->hp <= 0) {
                e->alive = 0;
                grid->alive_count--;
                
                // NOUVEAU : Spawn de pickups
                int random = rand() % 100;
                
                if (random < 10) {
                    // 10% : Pièce
                    int coin = (rand() % 2 == 0) ? PICKUP_COIN_GREEN : PICKUP_COIN_YELLOW;
                    grid->pending_pickup_x = e->x + e->width / 2;
                    grid->pending_pickup_y = e->y;
                    grid->pending_pickup_type = coin;
                    grid->has_pending_pickup = 1;
                } else if (random < 25) {
                    // 15% : Bonus ou malus
                    int bonus_random = rand() % 100;
                    int bonus;
                    
                    if (bonus_random < 20) {
                        bonus = PICKUP_BONUS_HEART;      // 20% coeur
                    } else if (bonus_random < 40) {
                        bonus = PICKUP_BONUS_GREEN;      // 20% balles
                    } else if (bonus_random < 55) {
                        bonus = PICKUP_BONUS_BLUE;       // 15% vitesse
                    } else if (bonus_random < 70) {
                        bonus = PICKUP_BONUS_ORANGE;     // 15% ralentir ennemis
                    } else if (bonus_random < 85) {
                        bonus = PICKUP_BONUS_BROWN;      // 15% MALUS lenteur
                    } else {
                        bonus = PICKUP_BOMB;             // 15% MALUS bombe
                    }
                    
                    grid->pending_pickup_x = e->x + e->width / 2;
                    grid->pending_pickup_y = e->y;
                    grid->pending_pickup_type = bonus;
                    grid->has_pending_pickup = 1;
                }
                
                return e->score_value;  // Retourner le score
            }
            
            return 0;  // Touché mais pas mort
        }
    }
    return -1;  // Aucune collision
}

/* ---- Vérifier si un ennemi atteint le fond ---- */
int enemy_check_reached_bottom(EnemyGrid *grid, int screenHeight) {
    for (int i = 0; i < grid->count; i++) {
        if (!grid->enemies[i].alive) continue;
        
        if (grid->enemies[i].y >= screenHeight - 60) {
            return 1;  // Game over!
        }
    }
    return 0;
}