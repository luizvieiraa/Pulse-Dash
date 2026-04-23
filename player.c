#include "player.h"
#include <math.h>
#include <stddef.h>

// Desenha um boneco estilizado dentro de um frame do spritesheet.
static void DrawCharacterSpriteFrame(Rectangle frameArea, int frameIndex)
{
    // Define a paleta do boneco minimalista no estilo neon da logo.
    const Color silhouetteColor = (Color){ 2, 5, 14, 255 };
    const Color glowOuterColor = (Color){ 0, 98, 255, 75 };
    const Color glowMidColor = (Color){ 0, 186, 255, 130 };
    const Color glowCoreColor = (Color){ 120, 235, 255, 255 };
    const Color trailColor = (Color){ 35, 140, 255, 95 };

    // Cria um ponto central para posicionar a silhueta dentro do sprite.
    float centerX = frameArea.x + frameArea.width * 0.54f;

    // Define a base do personagem dentro do frame.
    float groundY = frameArea.y + frameArea.height - 16.0f;

    // Controla pequenas variacoes de pose entre corrida e pulo.
    float bodyBob = 0.0f;
    float torsoLean = 0.0f;
    float frontArmShift = 0.0f;
    float backArmShift = 0.0f;
    float frontLegShift = 0.0f;
    float backLegShift = 0.0f;
    float jumpLift = 0.0f;

    // Escolhe pequenas variacoes de pose para cada frame da corrida e do pulo.
    switch (frameIndex)
    {
        case 0:
            bodyBob = -2.0f;
            torsoLean = 8.0f;
            frontArmShift = 9.0f;
            backArmShift = -7.0f;
            frontLegShift = 10.0f;
            backLegShift = -7.0f;
            break;

        case 1:
            bodyBob = 1.0f;
            torsoLean = 5.0f;
            frontArmShift = -5.0f;
            backArmShift = 7.0f;
            frontLegShift = -8.0f;
            backLegShift = 8.0f;
            break;

        case 2:
            bodyBob = 0.0f;
            torsoLean = 7.0f;
            frontArmShift = 5.0f;
            backArmShift = -4.0f;
            frontLegShift = 6.0f;
            backLegShift = -3.0f;
            break;

        default:
            bodyBob = -5.0f;
            torsoLean = 1.5f;
            frontArmShift = 1.0f;
            backArmShift = 4.0f;
            frontLegShift = -10.0f;
            backLegShift = 7.0f;
            jumpLift = 13.0f;
            break;
    }

    // Posiciona os principais pontos de uma silhueta limpa e geometrica.
    Vector2 headCenter = { centerX + 6.0f, groundY - 79.0f + bodyBob - jumpLift };
    Vector2 neckPoint = { centerX + 1.0f, groundY - 60.0f + bodyBob - jumpLift };
    Vector2 chestPoint = { centerX - 6.0f + torsoLean * 0.25f, groundY - 49.0f + bodyBob - jumpLift };
    Vector2 hipPoint = { centerX - 11.0f, groundY - 23.0f + bodyBob - jumpLift };

    // Define os bracos com poucos segmentos para manter a leitura minimalista.
    Vector2 frontElbow = { centerX + 14.0f + frontArmShift, groundY - 42.0f + bodyBob - jumpLift };
    Vector2 frontHand = { centerX + 18.0f + frontArmShift, groundY - 27.0f + bodyBob - jumpLift };
    Vector2 backElbow = { centerX - 20.0f + backArmShift, groundY - 40.0f + bodyBob - jumpLift };
    Vector2 backHand = { centerX - 24.0f + backArmShift, groundY - 26.0f + bodyBob - jumpLift };

    // Define as pernas em linhas simples com apoio forte no chao.
    Vector2 frontKnee = { centerX + 8.0f + frontLegShift, groundY - 4.0f + bodyBob - jumpLift };
    Vector2 frontFoot = { centerX - 2.0f + frontLegShift, groundY + 7.0f };
    Vector2 backKnee = { centerX - 22.0f + backLegShift, groundY + 1.0f + bodyBob - jumpLift };
    Vector2 backFoot = { centerX - 29.0f + backLegShift, groundY + 8.0f };

    // Desenha rastros triangulares discretos para lembrar energia da logo.
    for (int trailIndex = 0; trailIndex < 3; trailIndex++)
    {
        float trailShift = 13.0f + trailIndex * 12.0f;
        float alphaFactor = 1.0f - trailIndex * 0.25f;

        DrawTriangle(
            (Vector2){ chestPoint.x - trailShift, chestPoint.y + 3.0f },
            (Vector2){ hipPoint.x - 24.0f - trailShift, hipPoint.y + 11.0f },
            (Vector2){ hipPoint.x - 6.0f - trailShift, hipPoint.y + 4.0f },
            Fade(trailColor, alphaFactor)
        );
    }

    // Desenha a sombra de apoio do personagem.
    DrawEllipse((int)(centerX - 5.0f), (int)(groundY + 6.0f), 20.0f, 5.0f, Fade(BLACK, 0.22f));

    // Desenha o brilho externo de cada parte para simular o contorno luminoso.
    DrawCircleV(headCenter, 16.0f, glowOuterColor);
    DrawLineEx(neckPoint, chestPoint, 16.0f, glowOuterColor);
    DrawLineEx(chestPoint, hipPoint, 16.0f, glowOuterColor);
    DrawLineEx(chestPoint, frontElbow, 13.0f, glowOuterColor);
    DrawLineEx(frontElbow, frontHand, 13.0f, glowOuterColor);
    DrawLineEx(chestPoint, backElbow, 13.0f, glowOuterColor);
    DrawLineEx(backElbow, backHand, 13.0f, glowOuterColor);
    DrawLineEx(hipPoint, frontKnee, 14.0f, glowOuterColor);
    DrawLineEx(frontKnee, frontFoot, 14.0f, glowOuterColor);
    DrawLineEx(hipPoint, backKnee, 14.0f, glowOuterColor);
    DrawLineEx(backKnee, backFoot, 14.0f, glowOuterColor);

    // Desenha um brilho intermediario mais forte perto da silhueta.
    DrawCircleV(headCenter, 11.0f, glowMidColor);
    DrawLineEx(neckPoint, chestPoint, 10.0f, glowMidColor);
    DrawLineEx(chestPoint, hipPoint, 10.0f, glowMidColor);
    DrawLineEx(chestPoint, frontElbow, 8.0f, glowMidColor);
    DrawLineEx(frontElbow, frontHand, 8.0f, glowMidColor);
    DrawLineEx(chestPoint, backElbow, 8.0f, glowMidColor);
    DrawLineEx(backElbow, backHand, 8.0f, glowMidColor);
    DrawLineEx(hipPoint, frontKnee, 9.0f, glowMidColor);
    DrawLineEx(frontKnee, frontFoot, 9.0f, glowMidColor);
    DrawLineEx(hipPoint, backKnee, 9.0f, glowMidColor);
    DrawLineEx(backKnee, backFoot, 9.0f, glowMidColor);

    // Desenha o nucleo escuro da silhueta usando formas simples.
    DrawCircleV(headCenter, 8.0f, silhouetteColor);
    DrawLineEx(neckPoint, chestPoint, 6.0f, silhouetteColor);
    DrawLineEx(chestPoint, hipPoint, 6.0f, silhouetteColor);
    DrawLineEx(chestPoint, frontElbow, 5.0f, silhouetteColor);
    DrawLineEx(frontElbow, frontHand, 5.0f, silhouetteColor);
    DrawLineEx(chestPoint, backElbow, 5.0f, silhouetteColor);
    DrawLineEx(backElbow, backHand, 5.0f, silhouetteColor);
    DrawLineEx(hipPoint, frontKnee, 6.0f, silhouetteColor);
    DrawLineEx(frontKnee, frontFoot, 6.0f, silhouetteColor);
    DrawLineEx(hipPoint, backKnee, 6.0f, silhouetteColor);
    DrawLineEx(backKnee, backFoot, 6.0f, silhouetteColor);

    // Desenha pequenas pontas na cabeca para lembrar a marca sem exagerar no detalhe.
    DrawTriangle(
        (Vector2){ headCenter.x - 1.0f, headCenter.y - 9.0f },
        (Vector2){ headCenter.x - 10.0f, headCenter.y - 3.0f },
        (Vector2){ headCenter.x - 3.0f, headCenter.y + 1.0f },
        silhouetteColor
    );
    DrawTriangle(
        (Vector2){ headCenter.x + 2.0f, headCenter.y - 9.0f },
        (Vector2){ headCenter.x - 4.0f, headCenter.y - 13.0f },
        (Vector2){ headCenter.x + 5.0f, headCenter.y - 5.0f },
        silhouetteColor
    );

    // Desenha o contorno neon mais nitido para finalizar o personagem.
    DrawCircleLines((int)headCenter.x, (int)headCenter.y, 8.0f, glowCoreColor);
    DrawLineEx(neckPoint, chestPoint, 2.8f, glowCoreColor);
    DrawLineEx(chestPoint, hipPoint, 2.8f, glowCoreColor);
    DrawLineEx(chestPoint, frontElbow, 2.6f, glowCoreColor);
    DrawLineEx(frontElbow, frontHand, 2.6f, glowCoreColor);
    DrawLineEx(chestPoint, backElbow, 2.6f, glowCoreColor);
    DrawLineEx(backElbow, backHand, 2.6f, glowCoreColor);
    DrawLineEx(hipPoint, frontKnee, 2.8f, glowCoreColor);
    DrawLineEx(frontKnee, frontFoot, 2.8f, glowCoreColor);
    DrawLineEx(hipPoint, backKnee, 2.8f, glowCoreColor);
    DrawLineEx(backKnee, backFoot, 2.8f, glowCoreColor);
}

// Monta um spritesheet simples em memoria para o personagem.
static RenderTexture2D BuildCharacterSpriteSheet(int frameWidth, int frameHeight, int frameCount)
{
    // Cria uma textura grande o bastante para guardar todos os frames lado a lado.
    RenderTexture2D spriteSheet = LoadRenderTexture(frameWidth * frameCount, frameHeight);

    // Desenha cada frame diretamente dentro do spritesheet.
    BeginTextureMode(spriteSheet);
    ClearBackground(BLANK);

    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
    {
        Rectangle frameArea = {
            frameIndex * (float)frameWidth,
            0.0f,
            (float)frameWidth,
            (float)frameHeight
        };

        DrawCharacterSpriteFrame(frameArea, frameIndex);
    }

    EndTextureMode();

    return spriteSheet;
}

Player CreatePlayer(
    float startX,
    float groundY,
    float playerWidth,
    float playerHeight,
    int spriteFrameWidth,
    int spriteFrameHeight,
    int spriteFrameCount
)
{
    // Cria a estrutura do personagem com os estados iniciais de movimento.
    Player player = { 0 };
    player.bounds = (Rectangle){ startX, groundY - playerHeight, playerWidth, playerHeight };
    player.velocityY = 0.0f;
    player.isOnGround = true;
    player.wasOnGround = true;
    player.runAnimationTime = 0.0f;

    // Gera o spritesheet usado para a animacao de corrida e pulo.
    player.spriteSheet = BuildCharacterSpriteSheet(spriteFrameWidth, spriteFrameHeight, spriteFrameCount);

    return player;
}

void DestroyPlayer(Player *player)
{
    // Libera a textura do personagem apenas se ela tiver sido criada.
    if (player != NULL && player->spriteSheet.id != 0)
    {
        UnloadRenderTexture(player->spriteSheet);
        player->spriteSheet.id = 0;
    }
}

void UpdatePlayer(Player *player, float deltaTime, float playerSpeed, float gravity, float jumpForce, float groundY)
{
    // Ignora a atualizacao caso o ponteiro seja invalido.
    if (player == NULL)
    {
        return;
    }

    // Guarda o estado anterior para detectar pouso.
    player->wasOnGround = player->isOnGround;

    // Move o personagem continuamente para a direita.
    player->bounds.x += playerSpeed * deltaTime;

    // Avanca o relogio da animacao enquanto ele corre no chao.
    if (player->isOnGround)
    {
        player->runAnimationTime += deltaTime;
    }

    // Permite pular com espaco, seta para cima ou clique esquerdo.
    if (player->isOnGround && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
    {
        player->velocityY = jumpForce;
        player->isOnGround = false;
    }

    // Aplica a gravidade continuamente.
    player->velocityY += gravity * deltaTime;

    // Atualiza a posicao vertical do personagem.
    player->bounds.y += player->velocityY * deltaTime;

    // Impede que ele atravesse o chao e restaura o estado de apoio.
    if (player->bounds.y >= groundY - player->bounds.height)
    {
        player->bounds.y = groundY - player->bounds.height;
        player->velocityY = 0.0f;
        player->isOnGround = true;
    }
}

void ResetPlayer(Player *player, float startX, float groundY)
{
    // Ignora a operacao caso o ponteiro seja invalido.
    if (player == NULL)
    {
        return;
    }

    // Reposiciona o personagem no inicio da fase e limpa o estado de movimento.
    player->bounds.x = startX;
    player->bounds.y = groundY - player->bounds.height;
    player->velocityY = 0.0f;
    player->isOnGround = true;
    player->wasOnGround = true;
    player->runAnimationTime = 0.0f;
}

bool DidPlayerJustLand(const Player *player)
{
    // Retorna verdadeiro apenas quando o personagem estava no ar e acabou de tocar o chao.
    return (player != NULL) && !player->wasOnGround && player->isOnGround;
}

int GetPlayerSpriteFrameIndex(const Player *player)
{
    // Mantem o frame de pulo enquanto o personagem esta no ar.
    if (player == NULL || !player->isOnGround)
    {
        return 3;
    }

    // Alterna entre os tres frames de corrida enquanto ele permanece no chao.
    return ((int)(player->runAnimationTime * 12.0f)) % 3;
}

void DrawPlayer(const Player *player, int spriteFrameWidth, int spriteFrameHeight)
{
    // Interrompe o desenho caso o personagem nao exista.
    if (player == NULL)
    {
        return;
    }

    // Seleciona o frame atual da animacao do personagem.
    int spriteFrameIndex = GetPlayerSpriteFrameIndex(player);

    // Calcula a area da textura que sera usada para o frame atual.
    Rectangle sourceRectangle = {
        spriteFrameIndex * (float)spriteFrameWidth,
        0.0f,
        (float)spriteFrameWidth,
        -(float)spriteFrameHeight
    };

    // Define onde o sprite sera desenhado no mundo.
    Rectangle destinationRectangle = {
        player->bounds.x - 18.0f,
        player->bounds.y - 42.0f,
        96.0f,
        128.0f
    };

    // Desenha o personagem no mundo com o frame atual.
    DrawTexturePro(
        player->spriteSheet.texture,
        sourceRectangle,
        destinationRectangle,
        (Vector2){ 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}
