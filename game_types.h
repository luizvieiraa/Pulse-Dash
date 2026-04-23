#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include "raylib.h"
#include <stdbool.h>

// Define a quantidade maxima de particulas simultaneas para os efeitos visuais.
#define MAX_DUST_PARTICLES 96

// Guarda os dados de uma particula individual de poeira.
typedef struct DustParticle
{
    bool active;
    Vector2 position;
    Vector2 velocity;
    float life;
    float maxLife;
    float radius;
    Color color;
} DustParticle;

#endif
