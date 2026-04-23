#include "scene.h"
#include <stdbool.h>
#include <math.h>

// Define o primeiro espinho real da fase para garantir um trecho inicial sem perigo.
static const float gSpikeStartX = 720.0f;

// Define o espaco base entre grupos de espinhos reais.
static const float gSpikeSpacing = 170.0f;

// Monta um espinho deterministico da fase com tamanho e posicao previsiveis.
static bool TryBuildSpikeObstacle(int spikeIndex, float groundY, SpikeObstacle *obstacle)
{
    // Define um padrao fixo de presenca para criar buracos e sequencias de salto.
    int pattern = ((spikeIndex % 9) + 9) % 9;
    bool shouldPlaceSpike = (pattern == 1) || (pattern == 2) || (pattern == 5) || (pattern == 7);

    if (!shouldPlaceSpike || obstacle == NULL)
    {
        return false;
    }

    // Varia levemente a largura e altura para a pista nao parecer totalmente repetitiva.
    float width = 34.0f + (float)((spikeIndex % 3 + 3) % 3) * 8.0f;
    float height = 42.0f + (float)(((spikeIndex + 1) % 2 + 2) % 2) * 10.0f;
    float baseX = gSpikeStartX + spikeIndex * gSpikeSpacing;

    // Posiciona os tres pontos do espinho diretamente sobre o chao.
    obstacle->leftPoint = (Vector2){ baseX, groundY };
    obstacle->topPoint = (Vector2){ baseX + width * 0.5f, groundY - height };
    obstacle->rightPoint = (Vector2){ baseX + width, groundY };

    // Define uma hitbox mais estreita que o triangulo para deixar a colisao mais justa.
    obstacle->hitbox = (Rectangle){
        baseX + width * 0.18f,
        groundY - height + 6.0f,
        width * 0.64f,
        height - 6.0f
    };

    return true;
}

SceneStyle GetSceneStyle(void)
{
    // Centraliza a definicao das cores para o projeto inteiro compartilhar a mesma identidade.
    SceneStyle style = {
        (Color){ 2, 5, 16, 255 },
        (Color){ 52, 182, 255, 255 },
        (Color){ 10, 32, 110, 255 }
    };

    return style;
}

void DrawLogoStyleBackground(float cameraX, int screenHeight, float groundY)
{
    // Define as cores principais do ambiente escuro com brilho neon.
    const Color panelColor = (Color){ 4, 8, 22, 255 };
    const Color lineColor = (Color){ 12, 46, 122, 180 };
    const Color glowColor = (Color){ 34, 170, 255, 150 };

    // Desenha varios paines retangulares ao fundo para criar profundidade geometrica.
    for (int blockIndex = -8; blockIndex < 42; blockIndex++)
    {
        float blockX = floorf(cameraX / 160.0f) * 160.0f + blockIndex * 160.0f;
        float blockHeight = 110.0f + (float)((blockIndex % 4 + 4) % 4) * 60.0f;
        float blockTop = groundY - blockHeight - 40.0f - (float)(((blockIndex + 1) % 3 + 3) % 3) * 20.0f;

        DrawRectangle((int)blockX, (int)blockTop, 100, (int)blockHeight, Fade(panelColor, 0.55f));
        DrawRectangleLinesEx((Rectangle){ blockX, blockTop, 100.0f, blockHeight }, 2.0f, Fade(lineColor, 0.45f));
    }

    // Desenha pequenos quadrados brilhantes flutuando para lembrar os pixels da logo.
    for (int pixelIndex = -10; pixelIndex < 70; pixelIndex++)
    {
        float pixelX = floorf(cameraX / 90.0f) * 90.0f + pixelIndex * 90.0f;
        float pixelY = 100.0f + (float)(((pixelIndex * 37) % 260 + 260) % 260);
        float pixelSize = (float)(8 + (((pixelIndex * 13) % 10 + 10) % 10));
        float alpha = 0.20f + (float)((pixelIndex % 5 + 5) % 5) * 0.10f;

        DrawRectangle((int)pixelX, (int)pixelY, (int)pixelSize, (int)pixelSize, Fade(glowColor, alpha));
    }

    // Desenha correntes verticais estilizadas parecidas com a arte da logo.
    for (int chainIndex = -2; chainIndex < 10; chainIndex++)
    {
        float chainX = floorf(cameraX / 420.0f) * 420.0f + 120.0f + chainIndex * 420.0f;

        for (int linkIndex = 0; linkIndex < 6; linkIndex++)
        {
            float linkY = 30.0f + linkIndex * 34.0f;
            DrawRectangleRounded((Rectangle){ chainX, linkY, 16.0f, 26.0f }, 0.45f, 6, Fade(lineColor, 0.55f));
            DrawRectangleRounded((Rectangle){ chainX + 3.0f, linkY + 3.0f, 10.0f, 20.0f }, 0.45f, 6, Fade(BLACK, 0.45f));
        }
    }

    // Desenha uma linha de energia no meio da tela para reforcar a identidade da logo.
    for (int waveIndex = -8; waveIndex < 34; waveIndex++)
    {
        float segmentX = floorf(cameraX / 180.0f) * 180.0f + waveIndex * 180.0f;
        float waveY = screenHeight * 0.64f;

        DrawLineEx((Vector2){ segmentX, waveY }, (Vector2){ segmentX + 35.0f, waveY }, 3.0f, Fade(glowColor, 0.65f));
        DrawLineEx((Vector2){ segmentX + 35.0f, waveY }, (Vector2){ segmentX + 55.0f, waveY - 42.0f }, 3.0f, Fade(glowColor, 0.65f));
        DrawLineEx((Vector2){ segmentX + 55.0f, waveY - 42.0f }, (Vector2){ segmentX + 78.0f, waveY + 38.0f }, 3.0f, Fade(glowColor, 0.65f));
        DrawLineEx((Vector2){ segmentX + 78.0f, waveY + 38.0f }, (Vector2){ segmentX + 105.0f, waveY }, 3.0f, Fade(glowColor, 0.65f));
    }
}

void DrawWorldFloor(float cameraTargetX, float groundY, float groundHeight, float groundTileWidth, SceneStyle style)
{
    // Desenha varios blocos de chao no estilo neon para criar a fase continua.
    for (int tileIndex = -2; tileIndex < 80; tileIndex++)
    {
        float tileX = tileIndex * groundTileWidth;
        Color tileColor = (tileIndex % 2 == 0) ? Fade(style.deepBlue, 0.45f) : Fade((Color){ 16, 52, 150, 255 }, 0.42f);

        DrawRectangle((int)tileX, (int)groundY, (int)groundTileWidth, (int)groundHeight, tileColor);
        DrawRectangle((int)tileX, (int)groundY, (int)groundTileWidth, 16, Fade(style.neonBlue, 0.18f));
        DrawRectangleLinesEx((Rectangle){ tileX, groundY, groundTileWidth, groundHeight }, 2.0f, Fade(style.neonBlue, 0.30f));
    }

    // Desenha pequenas marcas e pulsos de energia no chao para reforcar a sensacao de velocidade.
    for (int markIndex = -2; markIndex < 140; markIndex++)
    {
        float markX = markIndex * 100.0f;
        DrawRectangle((int)markX, (int)groundY + 34, 26, 10, Fade(style.neonBlue, 0.40f));
        DrawTriangle(
            (Vector2){ markX + 44.0f, groundY + 14.0f },
            (Vector2){ markX + 58.0f, groundY + 44.0f },
            (Vector2){ markX + 72.0f, groundY + 14.0f },
            Fade(style.neonBlue, 0.18f)
        );
    }

    // Desenha a linha principal de neon do piso, que funciona como guia visual da fase.
    DrawLineEx(
        (Vector2){ cameraTargetX - 490.0f, groundY + 2.0f },
        (Vector2){ cameraTargetX + 810.0f, groundY + 2.0f },
        4.0f,
        Fade(style.neonBlue, 0.85f)
    );

    // Calcula a faixa de indices de espinhos visiveis ao redor da camera atual.
    int firstSpikeIndex = (int)floorf((cameraTargetX - 760.0f - gSpikeStartX) / gSpikeSpacing) - 1;
    int lastSpikeIndex = (int)floorf((cameraTargetX + 760.0f - gSpikeStartX) / gSpikeSpacing) + 1;

    // Desenha os espinhos reais da fase com preenchimento e contorno neon.
    for (int spikeIndex = firstSpikeIndex; spikeIndex <= lastSpikeIndex; spikeIndex++)
    {
        SpikeObstacle obstacle = { 0 };

        if (!TryBuildSpikeObstacle(spikeIndex, groundY, &obstacle))
        {
            continue;
        }

        DrawTriangle(obstacle.leftPoint, obstacle.topPoint, obstacle.rightPoint, Fade(style.neonBlue, 0.18f));
        DrawTriangleLines(obstacle.leftPoint, obstacle.topPoint, obstacle.rightPoint, Fade(style.neonBlue, 0.92f));
        DrawLineEx(obstacle.leftPoint, obstacle.topPoint, 2.0f, Fade((Color){ 190, 240, 255, 255 }, 0.90f));
        DrawLineEx(obstacle.topPoint, obstacle.rightPoint, 2.0f, Fade((Color){ 190, 240, 255, 255 }, 0.90f));
    }
}

bool CheckPlayerSpikeCollision(Rectangle playerBounds, float groundY)
{
    // Encolhe a caixa do personagem para uma colisao mais justa com os espinhos.
    Rectangle playerHitbox = {
        playerBounds.x + 10.0f,
        playerBounds.y + 10.0f,
        playerBounds.width - 20.0f,
        playerBounds.height - 12.0f
    };

    // Calcula apenas os indices proximos ao personagem para manter a verificacao leve.
    int firstSpikeIndex = (int)floorf((playerBounds.x - 120.0f - gSpikeStartX) / gSpikeSpacing) - 1;
    int lastSpikeIndex = (int)floorf((playerBounds.x + playerBounds.width + 120.0f - gSpikeStartX) / gSpikeSpacing) + 1;

    // Testa colisao apenas com os espinhos reais que estao proximos da posicao atual.
    for (int spikeIndex = firstSpikeIndex; spikeIndex <= lastSpikeIndex; spikeIndex++)
    {
        SpikeObstacle obstacle = { 0 };

        if (!TryBuildSpikeObstacle(spikeIndex, groundY, &obstacle))
        {
            continue;
        }

        if (CheckCollisionRecs(playerHitbox, obstacle.hitbox))
        {
            return true;
        }
    }

    return false;
}

void DrawSceneHud(int screenWidth, int screenHeight, SceneStyle style, int deathCount, float hitFlashTimer)
{
    // Desenha uma moldura neon ao redor da tela para reforcar a identidade da logo.
    DrawRectangleRoundedLines(
        (Rectangle){ 20.0f, 20.0f, screenWidth - 40.0f, screenHeight - 40.0f },
        0.035f,
        12,
        Fade(style.neonBlue, 0.95f)
    );

    // Mostra na tela o comando de pulo no novo esquema de cor.
    DrawText("ESPACO / SETA CIMA / CLIQUE ESQUERDO = PULAR", 34, 34, 24, Fade(BLACK, 0.50f));
    DrawText("ESPACO / SETA CIMA / CLIQUE ESQUERDO = PULAR", 30, 30, 24, style.neonBlue);

    // Identifica o estilo visual atual de forma discreta.
    DrawText("ESTILO VISUAL DA LOGO + RUNNER MINIMALISTA", 34, 66, 20, Fade(BLACK, 0.50f));
    DrawText("ESTILO VISUAL DA LOGO + RUNNER MINIMALISTA", 30, 62, 20, Fade((Color){ 190, 240, 255, 255 }, 0.95f));

    // Exibe a contagem de colisões para dar feedback de progresso durante os testes.
    DrawText(TextFormat("COLISOES: %02i", deathCount), 34, 98, 20, Fade(BLACK, 0.50f));
    DrawText(TextFormat("COLISOES: %02i", deathCount), 30, 94, 20, Fade(style.neonBlue, 0.95f));

    // Desenha um aviso temporario em vermelho quando o personagem bate em um espinho.
    if (hitFlashTimer > 0.0f)
    {
        Color flashColor = Fade((Color){ 255, 90, 110, 255 }, hitFlashTimer * 2.0f);
        DrawText("COLIDIU COM UM ESPINHO", 34, 130, 22, Fade(BLACK, 0.45f));
        DrawText("COLIDIU COM UM ESPINHO", 30, 126, 22, flashColor);
    }
}
