# Compilateur et flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

#ajoute en dessous stv mettre + d'argument dans la compilation des fichiers (gcc)
LDFLAGS = $(shell sdl2-config --cflags --libs) -lSDL2_ttf -lSDL2_image 

# Fichiers
# --- MODIFIER/AJOUTER EN DESSOUS LES NOUVEAUX FICHIERS .c !!!!
SRC = main.c game.c player.c enemy.c bullet.c score.c sprites.c
OBJ = $(SRC:.c=.o)
TARGET = space_invaders

# Règle principale
all: $(TARGET)

# Création de l'exécutable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Compilation réussie! Lance avec: ./$(TARGET)"

# Compilation des .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJ) $(TARGET)
	@echo "✓ Fichiers nettoyés"

# Recompilation complète
rebuild: clean all

# Exécuter directement
run: all
	./$(TARGET)

.PHONY: all clean rebuild run