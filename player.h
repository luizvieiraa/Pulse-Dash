#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

// Reune todos os dados do personagem principal em um unico lugar.
typedef struct Player
{
    Rectangle bounds;
    float velocityY;
    bool isOnGround;
    bool wasOnGround;
    float runAnimationTime;
    RenderTexture2D spriteSheet;
} Player;

// Cria o personagem com fisica basica e gera o spritesheet usado na animacao.
Player CreatePlayer(
    float startX,
    float groundY,
    float playerWidth,
    float playerHeight,
    int spriteFrameWidth,
    int spriteFrameHeight,
    int spriteFrameCount
);

// Libera os recursos graficos associados ao personagem.
void DestroyPlayer(Player *player);

// Atualiza o movimento automatico, o pulo e o estado geral do personagem.
void UpdatePlayer(Player *player, float deltaTime, float playerSpeed, float gravity, float jumpForce, float groundY);

// Restaura o personagem para a posicao inicial depois de uma colisao ou reinicio de fase.
void ResetPlayer(Player *player, float startX, float groundY);

// Informa se o personagem acabou de tocar o chao neste frame.
bool DidPlayerJustLand(const Player *player);

// Retorna o frame atual do sprite de acordo com o estado da animacao.
int GetPlayerSpriteFrameIndex(const Player *player);

// Desenha o frame atual do personagem no mundo.
void DrawPlayer(const Player *player, int spriteFrameWidth, int spriteFrameHeight);

#endif
