#ifndef EFFECTS_H
#define EFFECTS_H

#include "game_types.h"

// Cria pequenas particulas continuas de corrida atras do personagem.
void SpawnRunningDust(DustParticle particles[], int particleCount, Rectangle playerBounds, float deltaTime, float *spawnTimer);

// Cria um efeito mais forte quando o personagem toca o chao.
void SpawnLandingDust(DustParticle particles[], int particleCount, Rectangle playerBounds);

// Atualiza o movimento e o tempo de vida das particulas.
void UpdateDustParticles(DustParticle particles[], int particleCount, float deltaTime);

// Desenha todas as particulas ativas com transparencia baseada no tempo de vida.
void DrawDustParticles(const DustParticle particles[], int particleCount);

// Limpa todas as particulas para reiniciar a cena visualmente.
void ClearDustParticles(DustParticle particles[], int particleCount);

#endif
