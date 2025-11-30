#include "sprites.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

static void initSprites(Sprite* sprites) {
    // Petits aliens et vaisseau
    sprites[VAISSEAU_1] = (Sprite){"Vaisseau joueur 1", {0, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[VAISSEAU_2] = (Sprite){"Vaisseau joueur 2", {8, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[VAISSEAU_3] = (Sprite){"Vaisseau joueur 3", {16, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_VERT_1] = (Sprite){"Alien vert 1", {24, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_ORANGE_1] = (Sprite){"Alien orange 1", {32, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_ROUGE_1] = (Sprite){"Alien rouge 1", {40, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_VERT_FONCE_1] = (Sprite){"Alien vert foncé 1", {48, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_JAUNE_1] = (Sprite){"Alien jaune 1", {56, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_ROSE_1] = (Sprite){"Alien rose 1", {64, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_GRIS_1] = (Sprite){"Alien gris 1", {72, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_BLEU_1] = (Sprite){"Alien bleu 1", {80, 0, SPRITE_SIZE, SPRITE_SIZE}};

    // Partie 2
    sprites[FLAMME_1] = (Sprite){"Flamme 1", {0, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[FLAMME_2] = (Sprite){"Flamme 2", {8, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[FLAMME_3] = (Sprite){"Flamme 3", {16, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_VERT_2] = (Sprite){"Alien vert 2", {24, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_ORANGE_2] = (Sprite){"Alien orange 2", {32, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_ROUGE_2] = (Sprite){"Alien rouge 2", {40, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_VERT_FONCE_2] = (Sprite){"Alien vert foncé 2", {48, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_JAUNE_2] = (Sprite){"Alien jaune 2", {56, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_ROSE_2] = (Sprite){"Alien rose 2", {64, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_GRIS_2] = (Sprite){"Alien gris 2", {72, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ALIEN_BLEU_2] = (Sprite){"Alien bleu 2", {80, 8, SPRITE_SIZE, SPRITE_SIZE}};

    // Gros aliens
    sprites[GROS_ALIEN_VERT] = (Sprite){"Gros alien vert", {24, 16, SPRITE_SIZE*2, SPRITE_SIZE*2}};
    sprites[GROS_ALIEN_ORANGE] = (Sprite){"Gros alien orange", {40, 16, SPRITE_SIZE*2, SPRITE_SIZE*2}};
    sprites[GROS_ALIEN_VERT_FONCE] = (Sprite){"Gros alien vert foncé", {56, 16, SPRITE_SIZE*2, SPRITE_SIZE*2}};
    sprites[GROS_ALIEN_GRIS] = (Sprite){"Gros alien gris", {72, 16, SPRITE_SIZE*2, SPRITE_SIZE*2}};
    sprites[BOSS] = (Sprite){"Boss", {40, 40, SPRITE_SIZE*4, SPRITE_SIZE*3}};

    // Projectiles Aliens
    sprites[PA_BULLES] = (Sprite){"Projectile alien bulles", {32, 32, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PA_JAUNE_1] = (Sprite){"Projectile alien jaune 1", {40, 32, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PA_JAUNE_2] = (Sprite){"Projectile alien jaune 2", {48, 32, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PA_VERT] = (Sprite){"Projectile alien vert", {56, 32, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PA_ROUGE] = (Sprite){"Projectile alien rouge", {64, 32, SPRITE_SIZE, SPRITE_SIZE}};

    // Astéroïdes
    sprites[ASTEROID_1] = (Sprite){"Astéroïde 1", {88, 0, SPRITE_SIZE*2, SPRITE_SIZE*2}};
    sprites[ASTEROID_2] = (Sprite){"Astéroïde 2", {104, 0, SPRITE_SIZE*2, SPRITE_SIZE*2}};
    sprites[ASTEROID_3] = (Sprite){"Astéroïde 3", {120, 0, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ASTEROID_4] = (Sprite){"Astéroïde 4", {120, 8, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[ASTEROID_5] = (Sprite){"Astéroïde 5", {88, 16, SPRITE_SIZE*2, SPRITE_SIZE}};

    // Balles
    sprites[BALLES_1] = (Sprite){"Balle 1", {0, 16, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[BALLES_2] = (Sprite){"Balle 2", {0, 24, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[BALLES_3] = (Sprite){"Balle 3", {0, 32, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PROJECTILE_BLEU_1] = (Sprite){"Projectile bleu 1", {8, 16, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PROJECTILE_BLEU_2] = (Sprite){"Projectile bleu 2", {8, 24, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PROJECTILE_BLEU_3] = (Sprite){"Projectile bleu 3", {8, 32, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PROJECTILE_JAUNE_1] = (Sprite){"Projectile jaune 1", {16, 16, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PROJECTILE_JAUNE_2] = (Sprite){"Projectile jaune 2", {16, 24, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PROJECTILE_JAUNE_3] = (Sprite){"Projectile jaune 3", {16, 32, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[LASER_1] = (Sprite){"Laser 1", {0, 40, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[LASER_2] = (Sprite){"Laser 2", {0, 48, SPRITE_SIZE, SPRITE_SIZE}};

    // Pièces
    sprites[PIECES_VERTE_1] = (Sprite){"Pièce verte 1", {8, 40, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PIECES_VERTE_2] = (Sprite){"Pièce verte 2", {8, 48, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PIECES_VERTE_3] = (Sprite){"Pièce verte 3", {8, 56, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PIECES_JAUNE_1] = (Sprite){"Pièce jaune 1", {16, 40, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PIECES_JAUNE_2] = (Sprite){"Pièce jaune 2", {16, 48, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[PIECES_JAUNE_3] = (Sprite){"Pièce jaune 3", {16, 56, SPRITE_SIZE, SPRITE_SIZE}};

    // Bonus
    sprites[BONUS_VERT] = (Sprite){"Bonus vert", {24, 40, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[BONUS_BLEU] = (Sprite){"Bonus bleu", {24, 48, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[BONUS_ORANGE] = (Sprite){"Bonus orange", {24, 56, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[BONUS_MARRON] = (Sprite){"Bonus marron", {32, 40, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[BOMBE] = (Sprite){"Bombe", {32, 48, SPRITE_SIZE, SPRITE_SIZE}};
    sprites[COEUR] = (Sprite){"Cœur", {32, 56, SPRITE_SIZE, SPRITE_SIZE}};

    // Texte
    sprites[SPACE_EATERS] = (Sprite){"Space Eaters", {72, 40, SPRITE_SIZE*6, SPRITE_SIZE*4}};
    sprites[WARNING] = (Sprite){"Warning", {0, 96, SPRITE_SIZE*8, SPRITE_SIZE}};
    sprites[BOITE_1] = (Sprite){"Boîte 1", {0, 104, SPRITE_SIZE*4, SPRITE_SIZE*3}};
    sprites[BOITE_2] = (Sprite){"Boîte 2", {32, 104, SPRITE_SIZE*8, SPRITE_SIZE*3}};
    sprites[BOITE_3] = (Sprite){"Boîte 3", {96, 104, SPRITE_SIZE*4, SPRITE_SIZE*3}};
}

SpriteManager* createSpriteManager(SDL_Renderer* renderer, const char* sprite_sheet_path) {
    SpriteManager* manager = (SpriteManager*)malloc(sizeof(SpriteManager));
    if (!manager) {
        printf("Erreur allocation mémoire pour SpriteManager\n");
        return NULL;
    }

    // Charger la sprite sheet
    SDL_Surface* image = IMG_Load(sprite_sheet_path);
    if (!image) {
        printf("Erreur chargement sprite sheet: %s\n", IMG_GetError());
        free(manager);
        return NULL;
    }

    manager->texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    if (!manager->texture) {
        printf("Erreur création texture: %s\n", SDL_GetError());
        free(manager);
        return NULL;
    }

    // Initialiser les sprites
    initSprites(manager->sprites);

    printf("SpriteManager créé avec succès! (%d sprites chargés)\n", TOTAL_SPRITES);
    return manager;
}

void destroySpriteManager(SpriteManager* manager) {
    if (manager) {
        if (manager->texture) {
            SDL_DestroyTexture(manager->texture);
        }
        free(manager);
        printf("SpriteManager détruit\n");
    }
}

SDL_Rect* getSprite(SpriteManager* manager, SpriteID id) {
    if (!manager || id < 0 || id >= TOTAL_SPRITES) {
        return NULL;
    }
    return &manager->sprites[id].rect;
}

void renderSprite(SpriteManager* manager, SDL_Renderer* renderer, SpriteID id, int x, int y, int scale) {
    if (!manager || id < 0 || id >= TOTAL_SPRITES) {
        return;
    }

    SDL_Rect src = manager->sprites[id].rect;
    SDL_Rect dest = {
        x,
        y,
        src.w * scale,
        src.h * scale
    };

    SDL_RenderCopy(renderer, manager->texture, &src, &dest);
}

const char* getSpriteName(SpriteManager* manager, SpriteID id) {
    if (!manager || id < 0 || id >= TOTAL_SPRITES) {
        return NULL;
    }
    return manager->sprites[id].name;
}




/*
// 1. Créer le gestionnaire au début du programme
SpriteManager* sprites = createSpriteManager(renderer, "sprite.png");

// 2. Dessiner un sprite facilement
renderSprite(sprites, renderer, VAISSEAU_1, 100, 100, 3);
//                               ^^^^^^^^^^  ^^^  ^^^  ^
//                               ID sprite    x    y   échelle

// 3. Nettoyer à la fin
destroySpriteManager(sprites);
*/