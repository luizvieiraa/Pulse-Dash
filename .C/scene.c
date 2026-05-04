#include "../.H/scene.h"
#include <stdbool.h>
#include <math.h>
#include <stddef.h>

static float LimitarFloat(float valor, float minimo, float maximo)
{
    return fmaxf(minimo, fminf(valor, maximo));
}

static Rectangle ObterAreaInternaPorta(Rectangle portaSaida)
{
    return (Rectangle){
        portaSaida.x + 12.0f,
        portaSaida.y + 14.0f,
        portaSaida.width - 24.0f,
        portaSaida.height - 18.0f
    };
}

EstiloCena ObterEstiloCena(void)
{
    // Centraliza a definicao das cores para o projeto inteiro compartilhar a mesma identidade.
    EstiloCena estilo;
    estilo.corFundo = (Color){ 2, 5, 16, 255 };
    estilo.azulNeon = (Color){ 52, 182, 255, 255 };
    estilo.azulProfundo = (Color){ 10, 32, 110, 255 };

    return estilo;
}

void DesenharFundoEstiloLogo(float cameraX, int alturaTela, float chaoY)
{
    // Define as cores principais do ambiente escuro com brilho neon.
    const Color corPainel = (Color){ 4, 8, 22, 255 };
    const Color corLinha = (Color){ 12, 46, 122, 180 };
    const Color corBrilho = (Color){ 34, 170, 255, 150 };

    // Desenha varios paineis retangulares ao fundo para criar profundidade geometrica.
    for (int indiceBloco = -8; indiceBloco < 42; indiceBloco++)
    {
        float blocoX = floorf(cameraX / 160.0f) * 160.0f + indiceBloco * 160.0f;
        float alturaBloco = 110.0f + (float)((indiceBloco % 4 + 4) % 4) * 60.0f;
        float topoBloco = chaoY - alturaBloco - 40.0f - (float)(((indiceBloco + 1) % 3 + 3) % 3) * 20.0f;

        DrawRectangle((int)blocoX, (int)topoBloco, 100, (int)alturaBloco, Fade(corPainel, 0.55f));
        DrawRectangleLinesEx((Rectangle){ blocoX, topoBloco, 100.0f, alturaBloco }, 2.0f, Fade(corLinha, 0.45f));
    }

    // Desenha pequenos quadrados brilhantes flutuando para lembrar os pixels da logo.
    for (int indicePixel = -10; indicePixel < 70; indicePixel++)
    {
        float pixelX = floorf(cameraX / 90.0f) * 90.0f + indicePixel * 90.0f;
        float pixelY = 100.0f + (float)(((indicePixel * 37) % 260 + 260) % 260);
        float tamanhoPixel = (float)(8 + (((indicePixel * 13) % 10 + 10) % 10));
        float alfa = 0.20f + (float)((indicePixel % 5 + 5) % 5) * 0.10f;

        DrawRectangle((int)pixelX, (int)pixelY, (int)tamanhoPixel, (int)tamanhoPixel, Fade(corBrilho, alfa));
    }

    // Desenha correntes verticais estilizadas parecidas com a arte da logo.
    for (int indiceCorrente = -2; indiceCorrente < 10; indiceCorrente++)
    {
        float correnteX = floorf(cameraX / 420.0f) * 420.0f + 120.0f + indiceCorrente * 420.0f;

        for (int indiceElo = 0; indiceElo < 6; indiceElo++)
        {
            float eloY = 30.0f + indiceElo * 34.0f;
            DrawRectangleRounded((Rectangle){ correnteX, eloY, 16.0f, 26.0f }, 0.45f, 6, Fade(corLinha, 0.55f));
            DrawRectangleRounded((Rectangle){ correnteX + 3.0f, eloY + 3.0f, 10.0f, 20.0f }, 0.45f, 6, Fade(BLACK, 0.45f));
        }
    }

    // Desenha uma linha de energia no meio da tela para reforcar a identidade da logo.
    for (int indiceOnda = -8; indiceOnda < 34; indiceOnda++)
    {
        float segmentoX = floorf(cameraX / 180.0f) * 180.0f + indiceOnda * 180.0f;
        float ondaY = alturaTela * 0.64f;

        DrawLineEx((Vector2){ segmentoX, ondaY }, (Vector2){ segmentoX + 35.0f, ondaY }, 3.0f, Fade(corBrilho, 0.65f));
        DrawLineEx((Vector2){ segmentoX + 35.0f, ondaY }, (Vector2){ segmentoX + 55.0f, ondaY - 42.0f }, 3.0f, Fade(corBrilho, 0.65f));
        DrawLineEx((Vector2){ segmentoX + 55.0f, ondaY - 42.0f }, (Vector2){ segmentoX + 78.0f, ondaY + 38.0f }, 3.0f, Fade(corBrilho, 0.65f));
        DrawLineEx((Vector2){ segmentoX + 78.0f, ondaY + 38.0f }, (Vector2){ segmentoX + 105.0f, ondaY }, 3.0f, Fade(corBrilho, 0.65f));
    }
}

void DesenharChaoMundo(float alvoCameraX, float chaoY, float alturaChao, float larguraBlocoChao, EstiloCena estilo)
{
    // Desenha varios blocos de chao no estilo neon para criar a fase continua.
    for (int indicePiso = -2; indicePiso < 80; indicePiso++)
    {
        float pisoX = indicePiso * larguraBlocoChao;
        Color corPiso = (indicePiso % 2 == 0) ? Fade(estilo.azulProfundo, 0.45f) : Fade((Color){ 16, 52, 150, 255 }, 0.42f);

        DrawRectangle((int)pisoX, (int)chaoY, (int)larguraBlocoChao, (int)alturaChao, corPiso);
        DrawRectangle((int)pisoX, (int)chaoY, (int)larguraBlocoChao, 16, Fade(estilo.azulNeon, 0.18f));
        DrawRectangleLinesEx((Rectangle){ pisoX, chaoY, larguraBlocoChao, alturaChao }, 2.0f, Fade(estilo.azulNeon, 0.30f));
    }

    // Desenha pequenas marcas e pulsos de energia no chao para reforcar a sensacao de velocidade.
    for (int indiceMarca = -2; indiceMarca < 140; indiceMarca++)
    {
        float marcaX = indiceMarca * 100.0f;
        DrawRectangle((int)marcaX, (int)chaoY + 34, 26, 10, Fade(estilo.azulNeon, 0.40f));
        DrawTriangle(
            (Vector2){ marcaX + 44.0f, chaoY + 14.0f },
            (Vector2){ marcaX + 58.0f, chaoY + 44.0f },
            (Vector2){ marcaX + 72.0f, chaoY + 14.0f },
            Fade(estilo.azulNeon, 0.18f)
        );
    }

    // Desenha a linha principal de neon do piso, que funciona como guia visual da fase.
    DrawLineEx(
        (Vector2){ alvoCameraX - 490.0f, chaoY + 2.0f },
        (Vector2){ alvoCameraX + 810.0f, chaoY + 2.0f },
        4.0f,
        Fade(estilo.azulNeon, 0.85f)
    );
}

bool VerificarColisaoJogadorEspinho(Rectangle limitesJogador, float chaoY)
{
    (void)limitesJogador;
    (void)chaoY;

    // Os espinhos agora vem do editor de fases.
    // Esta funcao nao e mais usada no fluxo principal.
    return false;
}

float ObterLimiteDireitoUltimoEspinho(const DadosEspinho *espinhos, int quantidadeEspinhos)
{
    if (espinhos == NULL || quantidadeEspinhos <= 0)
    {
        return 0.0f;
    }

    float limiteDireito = espinhos[0].posicaoX + 34.0f;

    for (int i = 1; i < quantidadeEspinhos; i++)
    {
        limiteDireito = fmaxf(limiteDireito, espinhos[i].posicaoX + 34.0f);
    }

    return limiteDireito;
}

Rectangle CriarPortaSaidaFase(const DadosEspinho *espinhos, int quantidadeEspinhos, float chaoY)
{
    const float larguraPorta = 84.0f;
    const float alturaPorta = 126.0f;
    const float margemAposUltimoEspinho = 340.0f;
    float limiteDireitoUltimoEspinho = ObterLimiteDireitoUltimoEspinho(espinhos, quantidadeEspinhos);

    return (Rectangle){
        limiteDireitoUltimoEspinho + margemAposUltimoEspinho,
        chaoY - alturaPorta,
        larguraPorta,
        alturaPorta
    };
}

float CalcularProgressoFase(float posicaoJogadorX, float inicioJogadorX, Rectangle portaSaida)
{
    float destinoX = portaSaida.x + portaSaida.width * 0.5f;

    if (destinoX <= inicioJogadorX)
    {
        return 1.0f;
    }

    return LimitarFloat((posicaoJogadorX - inicioJogadorX) / (destinoX - inicioJogadorX), 0.0f, 1.0f);
}

bool VerificarJogadorEntrouNaPorta(Rectangle limitesJogador, Rectangle portaSaida)
{
    Rectangle caixaJogador = {
        limitesJogador.x + 8.0f,
        limitesJogador.y + 8.0f,
        limitesJogador.width - 16.0f,
        limitesJogador.height - 8.0f
    };
    Rectangle areaInternaPorta = ObterAreaInternaPorta(portaSaida);

    return CheckCollisionRecs(caixaJogador, areaInternaPorta);
}

void DesenharPortaSaida(Rectangle portaSaida, EstiloCena estilo, float tempoAnimacao, bool portaLiberada)
{
    float pulso = 0.5f + 0.5f * sinf(tempoAnimacao * 3.6f);
    Rectangle areaInternaPorta = ObterAreaInternaPorta(portaSaida);

    Color corBrilhoExterno = portaLiberada
        ? Fade(estilo.azulNeon, 0.22f + pulso * 0.16f)
        : Fade(estilo.azulNeon, 0.10f + pulso * 0.05f);
    Color corEstrutura = portaLiberada
        ? Fade((Color){ 190, 240, 255, 255 }, 0.95f)
        : Fade(estilo.azulNeon, 0.45f);
    Color corNucleo = portaLiberada
        ? Fade((Color){ 120, 235, 255, 255 }, 0.28f + pulso * 0.18f)
        : Fade((Color){ 52, 182, 255, 255 }, 0.10f + pulso * 0.05f);

    DrawRectangleRounded(
        (Rectangle){ portaSaida.x - 10.0f, portaSaida.y - 12.0f, portaSaida.width + 20.0f, portaSaida.height + 24.0f },
        0.22f,
        10,
        corBrilhoExterno
    );
    DrawRectangleRounded(portaSaida, 0.20f, 10, Fade((Color){ 4, 8, 22, 255 }, 0.90f));
    DrawRectangleRounded(areaInternaPorta, 0.18f, 10, Fade(BLACK, 0.78f));
    DrawRectangleRounded(
        (Rectangle){ areaInternaPorta.x + 6.0f, areaInternaPorta.y + 8.0f, areaInternaPorta.width - 12.0f, areaInternaPorta.height - 14.0f },
        0.18f,
        10,
        corNucleo
    );
    DrawRectangleRoundedLines(portaSaida, 0.20f, 10, corEstrutura);
    DrawLineEx(
        (Vector2){ portaSaida.x + 14.0f, portaSaida.y + 20.0f },
        (Vector2){ portaSaida.x + portaSaida.width - 14.0f, portaSaida.y + 20.0f },
        3.0f,
        Fade(corEstrutura, 0.85f)
    );
    DrawCircle(
        (int)(portaSaida.x + portaSaida.width * 0.77f),
        (int)(portaSaida.y + portaSaida.height * 0.48f),
        4.5f,
        portaLiberada ? Fade((Color){ 255, 220, 110, 255 }, 0.95f) : Fade(corEstrutura, 0.45f)
    );
}

void DesenharHudCena(int larguraTela, int alturaTela, EstiloCena estilo, int contadorColisoes,
                     float temporizadorFlashDano, float progressoFase, bool portaLiberada,
                     bool faseConcluida)
{
    int percentualProgresso = (int)roundf(progressoFase * 100.0f);
    const float larguraBarra = 260.0f;
    const float alturaBarra = 14.0f;
    float larguraPreenchida = larguraBarra * progressoFase;

    // Desenha uma moldura neon ao redor da tela para reforcar a identidade da logo.
    DrawRectangleRoundedLines(
        (Rectangle){ 20.0f, 20.0f, larguraTela - 40.0f, alturaTela - 40.0f },
        0.035f,
        12,
        Fade(estilo.azulNeon, 0.95f)
    );

    // Mostra na tela o comando de pulo no novo esquema de cor.
    DrawText("ESPACO / SETA CIMA / CLIQUE ESQUERDO = PULAR", 34, 34, 24, Fade(BLACK, 0.50f));
    DrawText("ESPACO / SETA CIMA / CLIQUE ESQUERDO = PULAR", 30, 30, 24, estilo.azulNeon);

    // Identifica o estilo visual atual de forma discreta.
    DrawText("ESTILO VISUAL DA LOGO + RUNNER MINIMALISTA", 34, 66, 20, Fade(BLACK, 0.50f));
    DrawText("ESTILO VISUAL DA LOGO + RUNNER MINIMALISTA", 30, 62, 20, Fade((Color){ 190, 240, 255, 255 }, 0.95f));

    // Exibe a contagem de colisoes para dar feedback de progresso durante os testes.
    DrawText(TextFormat("COLISOES: %02i", contadorColisoes), 34, 98, 20, Fade(BLACK, 0.50f));
    DrawText(TextFormat("COLISOES: %02i", contadorColisoes), 30, 94, 20, Fade(estilo.azulNeon, 0.95f));

    // Mostra o andamento da fase no canto superior direito.
    DrawText("PROGRESSO DA FASE", larguraTela - 334, 34, 18, Fade(BLACK, 0.50f));
    DrawText("PROGRESSO DA FASE", larguraTela - 338, 30, 18, Fade((Color){ 190, 240, 255, 255 }, 0.95f));
    DrawText(TextFormat("%03i%%", percentualProgresso), larguraTela - 128, 58, 26, Fade(BLACK, 0.50f));
    DrawText(TextFormat("%03i%%", percentualProgresso), larguraTela - 132, 54, 26, estilo.azulNeon);
    DrawRectangleRounded(
        (Rectangle){ larguraTela - 340.0f, 94.0f, larguraBarra + 18.0f, alturaBarra + 18.0f },
        0.45f,
        10,
        Fade((Color){ 4, 8, 22, 255 }, 0.86f)
    );
    DrawRectangleRounded(
        (Rectangle){ larguraTela - 331.0f, 103.0f, larguraBarra, alturaBarra },
        0.45f,
        10,
        Fade((Color){ 7, 22, 56, 255 }, 0.92f)
    );
    DrawRectangleRounded(
        (Rectangle){ larguraTela - 331.0f, 103.0f, larguraPreenchida, alturaBarra },
        0.45f,
        10,
        Fade(estilo.azulNeon, 0.92f)
    );

    if (faseConcluida)
    {
        DrawText("FASE CONCLUIDA", larguraTela - 336, 128, 20, Fade(BLACK, 0.50f));
        DrawText("FASE CONCLUIDA", larguraTela - 340, 124, 20, Fade((Color){ 120, 235, 255, 255 }, 0.98f));
    }
    else if (portaLiberada)
    {
        DrawText("SAIDA LIBERADA", larguraTela - 316, 128, 20, Fade(BLACK, 0.50f));
        DrawText("SAIDA LIBERADA", larguraTela - 320, 124, 20, Fade((Color){ 255, 220, 110, 255 }, 0.96f));
    }
    else
    {
        DrawText("PASSE PELOS OBSTACULOS", larguraTela - 384, 128, 20, Fade(BLACK, 0.50f));
        DrawText("PASSE PELOS OBSTACULOS", larguraTela - 388, 124, 20, Fade((Color){ 190, 240, 255, 255 }, 0.86f));
    }

    // Desenha um aviso temporario em vermelho quando o personagem bate em um espinho.
    if (temporizadorFlashDano > 0.0f)
    {
        Color corFlash = Fade((Color){ 255, 90, 110, 255 }, temporizadorFlashDano * 2.0f);
        DrawText("COLIDIU COM UM ESPINHO", 34, 130, 22, Fade(BLACK, 0.45f));
        DrawText("COLIDIU COM UM ESPINHO", 30, 126, 22, corFlash);
    }

    // Instrucao para pausar no canto inferior direito.
    {
        const char *textoPausa = "[P] PAUSAR";
        int larguraPausa = MeasureText(textoPausa, 18);
        DrawText(textoPausa, 34 + larguraTela - 40 - larguraPausa, alturaTela - 54, 18, Fade(BLACK, 0.50f));
        DrawText(textoPausa, 30 + larguraTela - 40 - larguraPausa, alturaTela - 58, 18, Fade(estilo.azulNeon, 0.7f));
    }
}
