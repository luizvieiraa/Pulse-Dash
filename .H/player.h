#ifndef JOGADOR_H
#define JOGADOR_H

#include "raylib.h"
#include <stdbool.h>

// Reune todos os dados do personagem principal em um unico lugar.
typedef struct Jogador
{
    Rectangle limites;
    float velocidadeY;
    bool estaNoChao;
    bool estavaNoChao;
    float tempoAnimacaoCorrida;
    float rotacaoGraus;
    RenderTexture2D folhaSprites;
} Jogador;

// Cria o personagem com fisica basica e gera o spritesheet usado na animacao.
Jogador CriarJogador(
    float inicioX,
    float chaoY,
    float larguraJogador,
    float alturaJogador,
    int larguraFrameSprite,
    int alturaFrameSprite,
    int quantidadeFramesSprite
);

// Libera os recursos graficos associados ao personagem.
void DestruirJogador(Jogador *jogador);

// Atualiza o movimento automatico, o pulo e o estado geral do personagem.
void AtualizarJogador(Jogador *jogador, float tempoFrame, float velocidadeJogador, float gravidade, float forcaPulo, float chaoY);

// Restaura o personagem para a posicao inicial depois de uma colisao ou reinicio de fase.
void ReiniciarJogador(Jogador *jogador, float inicioX, float chaoY);

// Informa se o personagem acabou de tocar o chao neste frame.
bool JogadorAcabouDePousar(const Jogador *jogador);

// Retorna o frame atual do sprite de acordo com o estado da animacao.
int ObterIndiceFrameSpriteJogador(const Jogador *jogador);

// Desenha o frame atual do personagem no mundo.
void DesenharJogador(const Jogador *jogador, int larguraFrameSprite, int alturaFrameSprite);

#endif
