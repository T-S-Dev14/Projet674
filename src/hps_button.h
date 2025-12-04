#ifndef HPS_BUTTON_H
#define HPS_BUTTON_H

#include <stdint.h>

// Initialiser le bouton HPS
int hps_button_init();

// Lire l'état du bouton (1 = appuyé, 0 = relâché)
int hps_button_is_pressed();

// Fermer/nettoyer les ressources
void hps_button_cleanup();

#endif // HPS_BUTTON_H