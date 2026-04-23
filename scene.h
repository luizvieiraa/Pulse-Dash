#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"

// Guarda a paleta principal usada pelo cenario e interface.
typedef struct SceneStyle
{
    Color backgroundColor;
    Color neonBlue;
    Color deepBlue;
} SceneStyle;

// Reune os dados geometricos de um espinho para desenho e colisao.
typedef struct SpikeObstacle
{
    Vector2 leftPoint;
    Vector2 topPoint;
    Vector2 rightPoint;
    Rectangle hitbox;
} SpikeObstacle;

// Retorna as cores principais do estilo visual inspirado na logo.
SceneStyle GetSceneStyle(void);

// Desenha o fundo com blocos e pequenos detalhes luminosos inspirados na logo do jogo.
void DrawLogoStyleBackground(float cameraX, int screenHeight, float groundY);

// Desenha o chao, as marcas de energia e os detalhes do piso.
void DrawWorldFloor(float cameraTargetX, float groundY, float groundHeight, float groundTileWidth, SceneStyle style);

// Verifica se o personagem encostou em algum espinho real da fase.
bool CheckPlayerSpikeCollision(Rectangle playerBounds, float groundY);

// Desenha a interface fixa na tela, incluindo moldura e textos.
void DrawSceneHud(int screenWidth, int screenHeight, SceneStyle style, int deathCount, float hitFlashTimer);

#endif
