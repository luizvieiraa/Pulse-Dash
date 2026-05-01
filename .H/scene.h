#ifndef CENA_H
#define CENA_H

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

// Desenha a interface fixa na tela, incluindo moldura e textos.
void DesenharHudCena(int larguraTela, int alturaTela, EstiloCena estilo, int contadorColisoes, float temporizadorFlashDano);

#endif
