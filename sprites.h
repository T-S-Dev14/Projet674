#ifndef SPRITES_H
#define SPRITES_H

#include <SDL2/SDL.h>

#define SPRITE_SIZE 8

// Énumération pour identifier facilement chaque sprite
typedef enum {
    // Petits aliens et vaisseau 
    VAISSEAU_1,
    VAISSEAU_2,
    VAISSEAU_3,
    ALIEN_VERT_1,
    ALIEN_ORANGE_1,
    ALIEN_ROUGE_1,
    ALIEN_VERT_FONCE_1,
    ALIEN_JAUNE_1,
    ALIEN_ROSE_1,
    ALIEN_GRIS_1,
    ALIEN_BLEU_1,

    //Partie 2
    FLAMME_1,
    FLAMME_2,
    FLAMME_3,
    ALIEN_VERT_2,
    ALIEN_ORANGE_2,
    ALIEN_ROUGE_2,
    ALIEN_VERT_FONCE_2,
    ALIEN_JAUNE_2,
    ALIEN_ROSE_2,
    ALIEN_GRIS_2,
    ALIEN_BLEU_2,

    //Gros aliens
    GROS_ALIEN_VERT,
    GROS_ALIEN_ORANGE,
    GROS_ALIEN_VERT_FONCE,
    GROS_ALIEN_GRIS,
    BOSS,

    //Projectiles Aliens
    PA_BULLES,
    PA_JAUNE_1,
    PA_JAUNE_2,
    PA_VERT,
    PA_ROUGE,

    // Asteroide
    ASTEROID_1,
    ASTEROID_2,
    ASTEROID_3,
    ASTEROID_4,
    ASTEROID_5,

    //Balles
    BALLES_1,
    BALLES_2,
    BALLES_3,
    PROJECTILE_BLEU_1,
    PROJECTILE_BLEU_2,  
    PROJECTILE_BLEU_3, 
    PROJECTILE_JAUNE_1,
    PROJECTILE_JAUNE_2,  
    PROJECTILE_JAUNE_3, 
    LASER_1,
    LASER_2,

    //Pièces
    PIECES_VERTE_1,
    PIECES_VERTE_2,
    PIECES_VERTE_3,
    PIECES_JAUNE_1,
    PIECES_JAUNE_2,
    PIECES_JAUNE_3,

    //Bonus
    BONUS_VERT,
    BONUS_BLEU,
    BONUS_ORANGE,
    BONUS_MARRON,
    BOMBE,
    COEUR,

    //Texte
    SPACE_EATERS,
    WARNING,
    BOITE_1,
    BOITE_2,
    BOITE_3,
    
    TOTAL_SPRITES
} SpriteID;

typedef struct Sprite {
    const char* name;
    SDL_Rect rect;
} Sprite;

// Gestionnaire de sprites
typedef struct {
    SDL_Texture* texture;
    Sprite sprites[TOTAL_SPRITES];
} SpriteManager;

// Fonctions publiques
SpriteManager* createSpriteManager(SDL_Renderer* renderer, const char* sprite_sheet_path);
void destroySpriteManager(SpriteManager* manager);
SDL_Rect* getSprite(SpriteManager* manager, SpriteID id);
void renderSprite(SpriteManager* manager, SDL_Renderer* renderer, SpriteID id, int x, int y, int scale);
const char* getSpriteName(SpriteManager* manager, SpriteID id);

#endif // SPRITES_H
