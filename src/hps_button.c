#include "hps_button.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define HPS_GPIO1_BASE 0xFF709000
#define GPIO_SPAN 0x1000
#define GPIO_SWPORTA_DDR 0x04
#define GPIO_EXT_PORTA   0x50
#define HPS_KEY_MASK (1 << 25)

static int fd = -1;
static void *gpio_map = NULL;
static volatile uint32_t *gpio_ext_ptr = NULL;

int hps_button_init() {
    // Ouvrir /dev/mem
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
        perror("Erreur: impossible d'ouvrir /dev/mem pour le bouton");
        return -1;
    }
    
    // Mapper GPIO1 en mémoire
    gpio_map = mmap(NULL, GPIO_SPAN, 
                    PROT_READ | PROT_WRITE, MAP_SHARED, 
                    fd, HPS_GPIO1_BASE);
    
    if (gpio_map == MAP_FAILED) {
        perror("Erreur: mmap a échoué pour le bouton");
        close(fd);
        fd = -1;
        return -1;
    }
    
    // Pointeurs vers les registres
    volatile uint32_t *gpio_ddr_ptr = (uint32_t *)(gpio_map + GPIO_SWPORTA_DDR);
    gpio_ext_ptr = (uint32_t *)(gpio_map + GPIO_EXT_PORTA);
    
    // Configurer le pin en entrée (bit à 0 dans DDR)
    uint32_t ddr_value = *gpio_ddr_ptr;
    ddr_value &= ~HPS_KEY_MASK;
    *gpio_ddr_ptr = ddr_value;
    
    printf("Bouton HPS initialisé avec succès\n");
    return 0;
}

int hps_button_is_pressed() {
    if (gpio_ext_ptr == NULL) {
        return 0;
    }
    
    uint32_t gpio_value = *gpio_ext_ptr;
    uint32_t button_state = (gpio_value & HPS_KEY_MASK) >> 25;
    
    // Le bouton est actif à 0 (appuyé = 0, relâché = 1)
    // On inverse pour retourner 1 quand appuyé
    return (button_state == 0) ? 1 : 0;
}

void hps_button_cleanup() {
    if (gpio_map != NULL) {
        munmap(gpio_map, GPIO_SPAN);
        gpio_map = NULL;
        gpio_ext_ptr = NULL;
    }
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}