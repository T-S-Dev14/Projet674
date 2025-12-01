# Compilateur et flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

#ajoute en dessous stv mettre + d'argument dans la compilation des fichiers (gcc)
LDFLAGS = $(shell sdl2-config --cflags --libs) -lSDL2_ttf -lSDL2_image 

# Fichiers
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
TARGET = build/space_invaders

# Règle principale
all: $(TARGET)

# Création de l'exécutable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Compilation réussie! Lance avec: ./$(TARGET)"

# Compilation des .c en .o
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@


# Nettoyage
clean:
	rm -rf build/*
	@echo "✓ Fichiers nettoyés"


# Recompilation complète
rebuild: clean all

# Exécuter directement
run: all
	./$(TARGET)

.PHONY: all clean rebuild run