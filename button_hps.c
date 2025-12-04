#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

// Adresses physiques pour le HPS
#define HPS_LW_BRIDGE_BASE 0xFF200000  // Lightweight HPS-to-FPGA bridge base
#define HPS_LW_BRIDGE_SPAN 0x00200000  // Taille de la bridge

// Offset pour les boutons HPS (KEY)
#define KEY_BASE 0x00000050  // Offset relatif à la bridge

// Masques pour les boutons (KEY0-KEY3)
#define KEY0_MASK 0x01
#define KEY1_MASK 0x02
#define KEY2_MASK 0x04
#define KEY3_MASK 0x08

int main() {
    int fd;
    void *lw_bridge_map;
    volatile uint32_t *key_ptr;
    uint32_t key_value, previous_value = 0;
    
    // Ouvrir /dev/mem pour accéder à la mémoire physique
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
        perror("Erreur: impossible d'ouvrir /dev/mem");
        printf("Assurez-vous d'exécuter avec sudo\n");
        return 1;
    }
    
    // Mapper la lightweight bridge en mémoire virtuelle
    lw_bridge_map = mmap(NULL, HPS_LW_BRIDGE_SPAN, 
                         PROT_READ | PROT_WRITE, MAP_SHARED, 
                         fd, HPS_LW_BRIDGE_BASE);
    
    if (lw_bridge_map == MAP_FAILED) {
        perror("Erreur: mmap a échoué");
        close(fd);
        return 1;
    }
    
    // Pointer vers le registre des boutons
    key_ptr = (uint32_t *)(lw_bridge_map + KEY_BASE);
    
    printf("=== Lecture des boutons HPS DE1-SoC ===\n");
    printf("Appuyez sur les boutons KEY0-KEY3\n");
    printf("Appuyez sur Ctrl+C pour quitter\n\n");
    
    // Boucle de lecture des boutons
    while (1) {
        // Lire la valeur des boutons
        key_value = *key_ptr;
        
        // Les boutons sont actifs à 0 (appuyés = 0, relâchés = 1)
        // On inverse pour avoir une logique plus intuitive
        key_value = ~key_value & 0x0F;
        
        // Afficher uniquement si l'état a changé
        if (key_value != previous_value) {
            printf("État des boutons: ");
            
            if (key_value & KEY0_MASK) printf("KEY0 ");
            if (key_value & KEY1_MASK) printf("KEY1 ");
            if (key_value & KEY2_MASK) printf("KEY2 ");
            if (key_value & KEY3_MASK) printf("KEY3 ");
            
            if (key_value == 0) {
                printf("Aucun bouton appuyé");
            }
            
            printf("\n");
            previous_value = key_value;
        }
        
        // Petit délai pour éviter une utilisation CPU excessive
        usleep(10000); // 10ms
    }
    
    // Nettoyage (code jamais atteint dans cette version)
    munmap(lw_bridge_map, HPS_LW_BRIDGE_SPAN);
    close(fd);
    
    return 0;
}