#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

// Adresses pour le HPS
#define HPS_GPIO1_BASE 0xFF709000  // Base GPIO1 (contient HPS_KEY)
#define GPIO_SPAN 0x1000

// Offsets des registres GPIO
#define GPIO_SWPORTA_DR  0x00  // Data register
#define GPIO_SWPORTA_DDR 0x04  // Direction register
#define GPIO_EXT_PORTA   0x50  // External port register (lecture)

// HPS_KEY est connecté à GPIO1[25]
#define HPS_KEY_MASK (1 << 25)

int main() {
    int fd;
    void *gpio_map;
    volatile uint32_t *gpio_ddr_ptr;
    volatile uint32_t *gpio_ext_ptr;
    uint32_t previous_state = 1;
    
    printf("=== Lecture du bouton HPS_KEY (bouton utilisateur) ===\n");
    printf("Localisation: bouton marqué 'HPS_KEY' ou 'KEY' sur la carte\n");
    printf("(à côté des boutons RESET)\n\n");
    
    // Ouvrir /dev/mem
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
        perror("Erreur: impossible d'ouvrir /dev/mem");
        printf("Exécutez avec: sudo ./buttons_hps\n");
        return 1;
    }
    
    // Mapper GPIO1 en mémoire
    gpio_map = mmap(NULL, GPIO_SPAN, 
                    PROT_READ | PROT_WRITE, MAP_SHARED, 
                    fd, HPS_GPIO1_BASE);
    
    if (gpio_map == MAP_FAILED) {
        perror("Erreur: mmap a échoué");
        close(fd);
        return 1;
    }
    
    // Pointeurs vers les registres
    gpio_ddr_ptr = (uint32_t *)(gpio_map + GPIO_SWPORTA_DDR);
    gpio_ext_ptr = (uint32_t *)(gpio_map + GPIO_EXT_PORTA);
    
    // Configurer le pin en entrée (bit à 0 dans DDR)
    uint32_t ddr_value = *gpio_ddr_ptr;
    ddr_value &= ~HPS_KEY_MASK;  // Mettre le bit 25 à 0 (entrée)
    *gpio_ddr_ptr = ddr_value;
    
    printf("Configuration GPIO réussie\n");
    printf("Appuyez sur le bouton HPS_KEY\n");
    printf("Ctrl+C pour quitter\n\n");
    
    int press_count = 0;
    
    // Boucle de lecture
    while (1) {
        // Lire l'état du bouton
        uint32_t gpio_value = *gpio_ext_ptr;
        uint32_t button_state = (gpio_value & HPS_KEY_MASK) >> 25;
        
        // Le bouton est actif à 0 (appuyé = 0, relâché = 1)
        if (button_state == 0 && previous_state == 1) {
            press_count++;
            printf("🔘 BOUTON APPUYÉ ! (pression #%d)\n", press_count);
        } else if (button_state == 1 && previous_state == 0) {
            printf("   Bouton relâché\n");
        }
        
        previous_state = button_state;
        
        // Délai pour éviter une utilisation CPU excessive
        usleep(10000); // 10ms
    }
    
    // Nettoyage
    munmap(gpio_map, GPIO_SPAN);
    close(fd);
    
    return 0;
}