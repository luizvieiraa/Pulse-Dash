#include "effects.h"
#include <stddef.h>

// Ativa uma particula livre com os parametros recebidos.
static void ActivateDustParticle(
    DustParticle particles[],
    int particleCount,
    Vector2 position,
    Vector2 velocity,
    float maxLife,
    float radius,
    Color color
)
{
    // Procura a primeira particula disponivel para reaproveitar o buffer.
    for (int index = 0; index < particleCount; index++)
    {
        if (!particles[index].active)
        {
            particles[index].active = true;
            particles[index].position = position;
            particles[index].velocity = velocity;
            particles[index].life = maxLife;
            particles[index].maxLife = maxLife;
            particles[index].radius = radius;
            particles[index].color = color;
            break;
        }
    }
}

void SpawnRunningDust(DustParticle particles[], int particleCount, Rectangle playerBounds, float deltaTime, float *spawnTimer)
{
    // Ignora a logica caso o temporizador nao exista.
    if (spawnTimer == NULL)
    {
        return;
    }

    // Controla o intervalo para a poeira nascer em pulsos e nao em excesso.
    *spawnTimer += deltaTime;

    if (*spawnTimer < 0.045f)
    {
        return;
    }

    *spawnTimer = 0.0f;

    // Posiciona a poeira perto dos pes do personagem.
    Vector2 basePosition = {
        playerBounds.x + playerBounds.width * 0.20f,
        playerBounds.y + playerBounds.height + 2.0f
    };

    // Cria duas particulas por pulso para dar mais densidade ao rastro.
    for (int particleStep = 0; particleStep < 2; particleStep++)
    {
        float horizontalSpeed = -(80.0f + (float)GetRandomValue(0, 70));
        float verticalSpeed = -(25.0f + (float)GetRandomValue(0, 55));
        float particleRadius = 5.0f + (float)GetRandomValue(0, 5);

        ActivateDustParticle(
            particles,
            particleCount,
            (Vector2){ basePosition.x + (float)GetRandomValue(-6, 8), basePosition.y + (float)GetRandomValue(-2, 4) },
            (Vector2){ horizontalSpeed, verticalSpeed },
            0.40f + (float)GetRandomValue(0, 20) / 100.0f,
            particleRadius,
            (Color){ 84, 205, 255, 185 }
        );
    }
}

void SpawnLandingDust(DustParticle particles[], int particleCount, Rectangle playerBounds)
{
    // Define a origem do pouso no centro dos pes do personagem.
    Vector2 landingCenter = {
        playerBounds.x + playerBounds.width * 0.50f,
        playerBounds.y + playerBounds.height + 2.0f
    };

    // Espalha varias particulas para os lados para comunicar impacto.
    for (int index = 0; index < 18; index++)
    {
        float horizontalDirection = (index % 2 == 0) ? -1.0f : 1.0f;
        float horizontalSpeed = horizontalDirection * (90.0f + (float)GetRandomValue(0, 180));
        float verticalSpeed = -(70.0f + (float)GetRandomValue(0, 110));
        float particleRadius = 6.0f + (float)GetRandomValue(0, 8);

        ActivateDustParticle(
            particles,
            particleCount,
            (Vector2){ landingCenter.x + horizontalDirection * (float)GetRandomValue(0, 18), landingCenter.y },
            (Vector2){ horizontalSpeed, verticalSpeed },
            0.55f + (float)GetRandomValue(0, 25) / 100.0f,
            particleRadius,
            (Color){ 112, 225, 255, 220 }
        );
    }
}

void UpdateDustParticles(DustParticle particles[], int particleCount, float deltaTime)
{
    // Atualiza cada particula ativa aplicando gravidade e perda de vida.
    for (int index = 0; index < particleCount; index++)
    {
        if (!particles[index].active)
        {
            continue;
        }

        particles[index].life -= deltaTime;

        if (particles[index].life <= 0.0f)
        {
            particles[index].active = false;
            continue;
        }

        particles[index].velocity.y += 240.0f * deltaTime;
        particles[index].position.x += particles[index].velocity.x * deltaTime;
        particles[index].position.y += particles[index].velocity.y * deltaTime;
        particles[index].radius *= 0.992f;
    }
}

void DrawDustParticles(const DustParticle particles[], int particleCount)
{
    // Desenha as particulas mais velhas mais transparentes para parecer dissipacao.
    for (int index = 0; index < particleCount; index++)
    {
        if (!particles[index].active)
        {
            continue;
        }

        float lifeRatio = particles[index].life / particles[index].maxLife;
        Color particleColor = Fade(particles[index].color, lifeRatio);

        DrawCircleV(particles[index].position, particles[index].radius, particleColor);
    }
}

void ClearDustParticles(DustParticle particles[], int particleCount)
{
    // Desativa todas as particulas para reiniciar os efeitos imediatamente.
    for (int index = 0; index < particleCount; index++)
    {
        particles[index].active = false;
        particles[index].life = 0.0f;
        particles[index].maxLife = 0.0f;
        particles[index].radius = 0.0f;
        particles[index].position = (Vector2){ 0.0f, 0.0f };
        particles[index].velocity = (Vector2){ 0.0f, 0.0f };
    }
}
