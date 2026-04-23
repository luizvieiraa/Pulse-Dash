#include "effects.h"
#include "game_types.h"
#include "player.h"
#include "scene.h"
#include <math.h>

int main(void)
{
    // Define o tamanho base da janela do jogo.
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // Define a altura do chao e a posicao vertical dele no mundo.
    const float groundHeight = 140.0f;
    const float groundY = screenHeight - groundHeight;

    // Define o tamanho da area de colisao do personagem.
    const float playerWidth = 60.0f;
    const float playerHeight = 90.0f;

    // Define o tamanho de cada frame do sprite e quantos frames existem.
    const int spriteFrameWidth = 96;
    const int spriteFrameHeight = 128;
    const int spriteFrameCount = 4;

    // Define a velocidade horizontal do boneco em pixels por segundo.
    const float playerSpeed = 440.0f;

    // Define a forca da gravidade e a velocidade inicial do pulo.
    const float gravity = 1800.0f;
    const float jumpForce = -700.0f;

    // Define a largura de cada bloco visual do chao.
    const float groundTileWidth = 160.0f;

    // Define a posicao inicial usada quando o jogador precisa voltar apos uma colisao.
    const float playerStartX = 120.0f;

    // Carrega a paleta principal do cenario para unificar o visual do jogo.
    SceneStyle style = GetSceneStyle();

    // Armazena todas as particulas dos efeitos de corrida e pouso.
    DustParticle dustParticles[MAX_DUST_PARTICLES] = { 0 };

    // Controla o intervalo entre os pequenos sopros de energia da corrida.
    float runningDustTimer = 0.0f;

    // Guarda quantas vezes o jogador colidiu com espinhos nesta execucao.
    int deathCount = 0;

    // Mantem um pequeno flash de aviso depois de uma colisao.
    float hitFlashTimer = 0.0f;

    // Evita colisao repetida logo apos o respawn do personagem.
    float respawnProtectionTimer = 0.0f;

    // Inicializa a janela principal do projeto.
    InitWindow(screenWidth, screenHeight, "Pulse Dash");

    // Mantem a atualizacao e o desenho em 60 quadros por segundo.
    SetTargetFPS(60);

    // Cria o personagem principal com sprite, fisica e animacao.
    Player player = CreatePlayer(
        120.0f,
        groundY,
        playerWidth,
        playerHeight,
        spriteFrameWidth,
        spriteFrameHeight,
        spriteFrameCount
    );

    // Configura uma camera 2D para acompanhar o personagem horizontalmente.
    Camera2D camera = { 0 };

    // Mantem o boneco mais para a esquerda da tela, como em jogos de corrida lateral.
    camera.offset = (Vector2){ screenWidth * 0.30f, screenHeight * 0.55f };

    // A camera olha para a posicao central do personagem no mundo.
    camera.target = (Vector2){ player.bounds.x + player.bounds.width / 2.0f, player.bounds.y + player.bounds.height / 2.0f };

    // Mantem zoom padrao sem rotacao.
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Loop principal: executa enquanto o jogador nao fecha a janela.
    while (!WindowShouldClose())
    {
        // Calcula o tempo do frame atual para o movimento ficar suave.
        float deltaTime = GetFrameTime();

        // Atualiza o tempo restante do flash de dano e da protecao de respawn.
        hitFlashTimer = fmaxf(0.0f, hitFlashTimer - deltaTime);
        respawnProtectionTimer = fmaxf(0.0f, respawnProtectionTimer - deltaTime);

        // Atualiza o personagem com movimento automatico, pulo e gravidade.
        UpdatePlayer(&player, deltaTime, playerSpeed, gravity, jumpForce, groundY);

        // Cria o rastro de energia enquanto o personagem corre no chao.
        if (player.isOnGround)
        {
            SpawnRunningDust(dustParticles, MAX_DUST_PARTICLES, player.bounds, deltaTime, &runningDustTimer);
        }
        else
        {
            // Zera o temporizador no ar para o rastro voltar bem sincronizado ao pousar.
            runningDustTimer = 0.0f;
        }

        // Dispara um efeito mais forte exatamente no frame do pouso.
        if (DidPlayerJustLand(&player))
        {
            SpawnLandingDust(dustParticles, MAX_DUST_PARTICLES, player.bounds);
        }

        // Testa colisao com os espinhos reais apenas quando a protecao de respawn terminou.
        if (respawnProtectionTimer <= 0.0f && CheckPlayerSpikeCollision(player.bounds, groundY))
        {
            // Conta a colisao para facilitar os testes de gameplay.
            deathCount++;

            // Ativa um aviso visual curto na interface.
            hitFlashTimer = 0.65f;

            // Da um pequeno tempo de invulnerabilidade para evitar varias colisoes seguidas no mesmo frame.
            respawnProtectionTimer = 0.80f;

            // Limpa as particulas antigas para o respawn comecar visualmente limpo.
            ClearDustParticles(dustParticles, MAX_DUST_PARTICLES);

            // Zera o temporizador do rastro para ele voltar sincronizado apos o respawn.
            runningDustTimer = 0.0f;

            // Reposiciona o personagem no comeco da fase.
            ResetPlayer(&player, playerStartX, groundY);
        }

        // Atualiza a simulacao das particulas depois da logica principal do frame.
        UpdateDustParticles(dustParticles, MAX_DUST_PARTICLES, deltaTime);

        // Atualiza o alvo da camera para seguir o centro do personagem no eixo X.
        camera.target = (Vector2){ player.bounds.x + player.bounds.width / 2.0f, screenHeight / 2.0f };

        // Inicia a etapa de desenho na tela.
        BeginDrawing();

        // Pinta o fundo com o azul escuro principal do estilo da logo.
        ClearBackground(style.backgroundColor);

        // Entra no modo de desenho com camera para que o mundo "ande" junto com o personagem.
        BeginMode2D(camera);

        // Desenha os blocos geometricos e detalhes luminosos do fundo.
        DrawLogoStyleBackground(camera.target.x - camera.offset.x, screenHeight, groundY);

        // Desenha o piso completo da fase com blocos, linhas e espinhos decorativos.
        DrawWorldFloor(camera.target.x, groundY, groundHeight, groundTileWidth, style);

        // Desenha as particulas antes do personagem para a energia ficar parcialmente atras dele.
        DrawDustParticles(dustParticles, MAX_DUST_PARTICLES);

        // Desenha o personagem com o frame correto da animacao.
        DrawPlayer(&player, spriteFrameWidth, spriteFrameHeight);

        // Sai do modo de camera para desenhar elementos fixos da interface.
        EndMode2D();

        // Desenha a interface fixa com moldura e textos.
        DrawSceneHud(screenWidth, screenHeight, style, deathCount, hitFlashTimer);

        // Finaliza o desenho do frame atual.
        EndDrawing();
    }

    // Libera os recursos do personagem antes de encerrar o jogo.
    DestroyPlayer(&player);

    // Fecha a janela e libera os recursos usados pelo Raylib.
    CloseWindow();

    return 0;
}

// Inclui os modulos de implementacao no fim do arquivo principal.
// Isso preserva a organizacao em arquivos separados para leitura e manutencao,
// mas tambem permite compilar o projeto quando a ferramenta usa apenas pulse_dash.c.
#include "player.c"
#include "effects.c"
#include "scene.c"
