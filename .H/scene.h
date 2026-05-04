#ifndef PULSE_DASH_SCENE_H
#define PULSE_DASH_SCENE_H

#include "game_types.h"
#include "raylib.h"
#include <stdbool.h>

// Guarda a paleta principal usada pelo cenario e interface.
typedef struct EstiloCena
{
    Color corFundo;
    Color azulNeon;
    Color azulProfundo;
} EstiloCena;

// Retorna as cores principais do estilo visual inspirado na logo.
EstiloCena ObterEstiloCena(void);

// Desenha o fundo com blocos e pequenos detalhes luminosos inspirados na logo do jogo.
void DesenharFundoEstiloLogo(float cameraX, int alturaTela, float chaoY);

// Desenha o chao, as marcas de energia e os detalhes do piso.
void DesenharChaoMundo(float alvoCameraX, float chaoY, float alturaChao, float larguraBlocoChao, EstiloCena estilo);

// Verifica se o personagem encostou em algum espinho real da fase.
bool VerificarColisaoJogadorEspinho(Rectangle limitesJogador, float chaoY);

// Retorna a borda direita do ultimo espinho da fase para servir de referencia de progresso.
float ObterLimiteDireitoUltimoEspinho(const DadosEspinho *espinhos, int quantidadeEspinhos);

// Cria a area da porta que finaliza a fase depois do ultimo obstaculo.
Rectangle CriarPortaSaidaFase(const DadosEspinho *espinhos, int quantidadeEspinhos, float chaoY);

// Calcula o percentual de progresso do jogador entre o inicio da fase e a porta.
float CalcularProgressoFase(float posicaoJogadorX, float inicioJogadorX, Rectangle portaSaida);

// Verifica se o jogador entrou na area interna da porta.
bool VerificarJogadorEntrouNaPorta(Rectangle limitesJogador, Rectangle portaSaida);

// Desenha a porta de saida da fase com brilho neon.
void DesenharPortaSaida(Rectangle portaSaida, EstiloCena estilo, float tempoAnimacao, bool portaLiberada);

// Desenha a interface fixa na tela, incluindo moldura e textos.
void DesenharHudCena(int larguraTela, int alturaTela, EstiloCena estilo, int contadorColisoes,
                     float temporizadorFlashDano, float progressoFase, bool portaLiberada,
                     bool faseConcluida);

#endif
