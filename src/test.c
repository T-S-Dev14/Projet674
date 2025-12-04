#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

// Adresse de base du pont HPS-to-FPGA Lightweight
#define HPS_TO_FPGA_LW_BASE 0xFF200000
#define HPS_TO_FPGA_LW_SPAN 0x00200000

// Offsets typiques pour les périphériques (à ajuster selon ton design FPGA)
#define BUTTON_BASE_OFFSET  0x00000050  // Offset des boutons (KEY)
#define LED_BASE_OFFSET     0x00000010  // Offset des LEDs (LEDR)
#define SWITCH_BASE_OFFSET  0x00000040  // Offset des switches (SW)

// Structure pour gérer l'accès aux périphériques FPGA
typedef struct {
    int fd;
    void *virtual_base;
    void *button_addr;
    void *led_addr;
    void *switch_addr;
} fpga_dev_t;

/**
 * Initialise l'accès aux périphériques FPGA
 */
int fpga_init(fpga_dev_t *dev) {
    // Ouvrir /dev/mem
    dev->fd = open("/dev/mem", (O_RDWR | O_SYNC));
    if (dev->fd == -1) {
        perror("Erreur: impossible d'ouvrir /dev/mem");
        printf("Assure-toi d'exécuter avec sudo!\n");
        return -1;
    }

    // Mapper la région mémoire du pont lightweight
    dev->virtual_base = mmap(NULL, HPS_TO_FPGA_LW_SPAN, 
                             (PROT_READ | PROT_WRITE), MAP_SHARED, 
                             dev->fd, HPS_TO_FPGA_LW_BASE);
    
    if (dev->virtual_base == MAP_FAILED) {
        perror("Erreur: mmap a échoué");
        close(dev->fd);
        return -1;
    }

    // Calculer les adresses virtuelles des périphériques
    dev->button_addr = dev->virtual_base + BUTTON_BASE_OFFSET;
    dev->led_addr = dev->virtual_base + LED_BASE_OFFSET;
    dev->switch_addr = dev->virtual_base + SWITCH_BASE_OFFSET;

    printf("Initialisation FPGA réussie!\n");
    printf("Base virtuelle: %p\n", dev->virtual_base);
    
    return 0;
}

/**
 * Lit l'état des boutons
 * Retourne un masque de bits (bit 0 = KEY0, bit 1 = KEY1, etc.)
 */
uint32_t fpga_read_buttons(fpga_dev_t *dev) {
    volatile uint32_t *button_ptr = (uint32_t *)dev->button_addr;
    return *button_ptr;
}

/**
 * Lit l'état des switches
 */
uint32_t fpga_read_switches(fpga_dev_t *dev) {
    volatile uint32_t *switch_ptr = (uint32_t *)dev->switch_addr;
    return *switch_ptr;
}

/**
 * Écrit sur les LEDs
 */
void fpga_write_leds(fpga_dev_t *dev, uint32_t value) {
    volatile uint32_t *led_ptr = (uint32_t *)dev->led_addr;
    *led_ptr = value;
}

/**
 * Ferme l'accès aux périphériques FPGA
 */
void fpga_close(fpga_dev_t *dev) {
    if (dev->virtual_base != NULL && dev->virtual_base != MAP_FAILED) {
        munmap(dev->virtual_base, HPS_TO_FPGA_LW_SPAN);
    }
    if (dev->fd != -1) {
        close(dev->fd);
    }
}

/**
 * Programme simple pour détecter les boutons FPGA
 */
int main() {
    fpga_dev_t fpga;
    uint32_t buttons, previous_buttons = 0;

    printf("=== Détection des boutons FPGA ===\n");
    
    // Initialiser
    if (fpga_init(&fpga) != 0) {
        return 1;
    }

    printf("\n✓ Appuie sur les boutons de la carte!\n");
    printf("  (Ctrl+C pour quitter)\n\n");

    // Boucle principale
    while (1) {
        // Lire l'état actuel des boutons
        buttons = fpga_read_buttons(&fpga);
        
        // Détecter si un bouton a été pressé (front montant)
        if (buttons != previous_buttons && buttons != 0) {
            printf(">>> Bouton détecté! Valeur brute: 0x%02X\n", buttons);
            
            // Identifier quel(s) bouton(s) est/sont pressé(s)
            if (buttons & 0x01) printf("    → KEY0 est pressé\n");
            if (buttons & 0x02) printf("    → KEY1 est pressé\n");
            if (buttons & 0x04) printf("    → KEY2 est pressé\n");
            if (buttons & 0x08) printf("    → KEY3 est pressé\n");
            
            printf("\n");
        }
        
        // Sauvegarder l'état précédent
        previous_buttons = buttons;

        // Petit délai pour éviter de saturer le CPU
        usleep(10000); // 10ms
    }

    // Nettoyer
    fpga_close(&fpga);
    return 0;
}