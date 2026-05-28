#include "../.H/effects.h"
#include "../.H/game_types.h"
#include "../.H/player.h"
#include "../.H/scene.h"
#include "../.H/editor.h"
#include <math.h>
#include <stdio.h>

#define CAMINHO_PROGRESSO_FASES "fases/progresso.dat"
#define MENU_MARGIN 22
#define MENU_CANTOS_TAMANHO 48

static int MedirTextoEspacado(const char *texto, int tamanhoFonte, int espacamento)
{
    int largura = 0;

    for (int i = 0; texto[i] != '\0'; i++)
    {
        char caractere[2] = { texto[i], '\0' };
        largura += MeasureText(caractere, tamanhoFonte);

        if (texto[i + 1] != '\0')
        {
            largura += espacamento;
        }
    }

    return largura;
}

static void DesenharTextoEspacado(const char *texto, int centroX, int y, int tamanhoFonte, int espacamento, Color cor)
{
    int x = centroX - MedirTextoEspacado(texto, tamanhoFonte, espacamento) / 2;

    for (int i = 0; texto[i] != '\0'; i++)
    {
        char caractere[2] = { texto[i], '\0' };
        DrawText(caractere, x, y, tamanhoFonte, cor);
        x += MeasureText(caractere, tamanhoFonte) + espacamento;
    }
}

static void DesenharCantosMenu(int larguraTela, int alturaTela, Color corLinha)
{
    DrawLine(MENU_MARGIN, MENU_MARGIN, MENU_MARGIN + MENU_CANTOS_TAMANHO, MENU_MARGIN, corLinha);
    DrawLine(MENU_MARGIN, MENU_MARGIN, MENU_MARGIN, MENU_MARGIN + MENU_CANTOS_TAMANHO, corLinha);
    DrawLine(larguraTela - MENU_MARGIN - MENU_CANTOS_TAMANHO, MENU_MARGIN, larguraTela - MENU_MARGIN, MENU_MARGIN, corLinha);
    DrawLine(larguraTela - MENU_MARGIN, MENU_MARGIN, larguraTela - MENU_MARGIN, MENU_MARGIN + MENU_CANTOS_TAMANHO, corLinha);
    DrawLine(MENU_MARGIN, alturaTela - MENU_MARGIN, MENU_MARGIN + MENU_CANTOS_TAMANHO, alturaTela - MENU_MARGIN, corLinha);
    DrawLine(MENU_MARGIN, alturaTela - MENU_MARGIN - MENU_CANTOS_TAMANHO, MENU_MARGIN, alturaTela - MENU_MARGIN, corLinha);
    DrawLine(larguraTela - MENU_MARGIN - MENU_CANTOS_TAMANHO, alturaTela - MENU_MARGIN, larguraTela - MENU_MARGIN, alturaTela - MENU_MARGIN, corLinha);
    DrawLine(larguraTela - MENU_MARGIN, alturaTela - MENU_MARGIN - MENU_CANTOS_TAMANHO, larguraTela - MENU_MARGIN, alturaTela - MENU_MARGIN, corLinha);
}

static void DesenharLinhaHorizontalGlow(float x, float y, float largura, Color cor)
{
    DrawLineEx((Vector2){ x, y }, (Vector2){ x + largura, y }, 8.0f, Fade(cor, 0.05f));
    DrawLineEx((Vector2){ x, y }, (Vector2){ x + largura, y }, 3.0f, Fade(cor, 0.14f));
    DrawLineEx((Vector2){ x, y }, (Vector2){ x + largura, y }, 1.0f, Fade(cor, 0.48f));
}

static void DesenharPulsoTitulo(int centroX, int y, Color cor)
{
    int larguraLateral = 250;
    int metadePulso = 18;

    DesenharLinhaHorizontalGlow((float)(centroX - larguraLateral - 28), (float)y, (float)larguraLateral, cor);
    DesenharLinhaHorizontalGlow((float)(centroX + 28), (float)y, (float)larguraLateral, cor);

    DrawLineEx((Vector2){ centroX - 28.0f, y }, (Vector2){ centroX - 12.0f, y }, 1.5f, Fade(cor, 0.90f));
    DrawLineEx((Vector2){ centroX - 12.0f, y }, (Vector2){ centroX - 6.0f, y - metadePulso }, 1.5f, Fade(cor, 0.90f));
    DrawLineEx((Vector2){ centroX - 6.0f, y - metadePulso }, (Vector2){ centroX + 2.0f, y + metadePulso }, 1.5f, Fade(cor, 0.90f));
    DrawLineEx((Vector2){ centroX + 2.0f, y + metadePulso }, (Vector2){ centroX + 9.0f, y - 8.0f }, 1.5f, Fade(cor, 0.90f));
    DrawLineEx((Vector2){ centroX + 9.0f, y - 8.0f }, (Vector2){ centroX + 15.0f, y + 8.0f }, 1.5f, Fade(cor, 0.90f));
    DrawLineEx((Vector2){ centroX + 15.0f, y + 8.0f }, (Vector2){ centroX + 24.0f, y }, 1.5f, Fade(cor, 0.90f));
}

static void DesenharIconeMenu(int indice, Vector2 centro, Color cor)
{
    if (indice == 0)
    {
        DrawTriangle(
            (Vector2){ centro.x - 7.0f, centro.y - 12.0f },
            (Vector2){ centro.x - 7.0f, centro.y + 12.0f },
            (Vector2){ centro.x + 12.0f, centro.y },
            cor
        );
    }
    else if (indice == 1)
    {
        DrawRectangleLinesEx((Rectangle){ centro.x - 10.0f, centro.y - 11.0f, 20.0f, 22.0f }, 2.0f, cor);
        DrawLineEx((Vector2){ centro.x - 5.0f, centro.y - 5.0f }, (Vector2){ centro.x + 6.0f, centro.y - 5.0f }, 2.0f, cor);
        DrawLineEx((Vector2){ centro.x - 5.0f, centro.y + 1.0f }, (Vector2){ centro.x + 6.0f, centro.y + 1.0f }, 2.0f, cor);
        DrawLineEx((Vector2){ centro.x - 5.0f, centro.y + 7.0f }, (Vector2){ centro.x + 6.0f, centro.y + 7.0f }, 2.0f, cor);
    }
    else
    {
        DrawRectangleLinesEx((Rectangle){ centro.x - 10.0f, centro.y - 10.0f, 15.0f, 20.0f }, 2.0f, cor);
        DrawLineEx((Vector2){ centro.x - 2.0f, centro.y }, (Vector2){ centro.x + 13.0f, centro.y }, 2.0f, cor);
        DrawLineEx((Vector2){ centro.x + 8.0f, centro.y - 6.0f }, (Vector2){ centro.x + 14.0f, centro.y }, 2.0f, cor);
        DrawLineEx((Vector2){ centro.x + 8.0f, centro.y + 6.0f }, (Vector2){ centro.x + 14.0f, centro.y }, 2.0f, cor);
    }
}

static void DesenharDetalhesFundoMenu(int larguraTela, int alturaTela, Color cor)
{
    Color corFraca = Fade(cor, 0.18f);

    DrawLineEx((Vector2){ larguraTela * 0.07f, alturaTela * 0.28f }, (Vector2){ larguraTela * 0.16f, alturaTela * 0.28f }, 1.0f, corFraca);
    DrawLineEx((Vector2){ larguraTela * 0.16f, alturaTela * 0.28f }, (Vector2){ larguraTela * 0.22f, alturaTela * 0.35f }, 1.0f, corFraca);
    DrawLineEx((Vector2){ larguraTela * 0.22f, alturaTela * 0.35f }, (Vector2){ larguraTela * 0.46f, alturaTela * 0.35f }, 1.0f, Fade(cor, 0.10f));

    DrawLineEx((Vector2){ larguraTela * 0.76f, alturaTela * 0.27f }, (Vector2){ larguraTela * 0.84f, alturaTela * 0.27f }, 1.0f, corFraca);
    DrawLineEx((Vector2){ larguraTela * 0.84f, alturaTela * 0.27f }, (Vector2){ larguraTela * 0.87f, alturaTela * 0.20f }, 1.0f, corFraca);
    DrawLineEx((Vector2){ larguraTela * 0.87f, alturaTela * 0.20f }, (Vector2){ larguraTela * 0.94f, alturaTela * 0.20f }, 1.0f, Fade(cor, 0.10f));

    DrawLineEx((Vector2){ larguraTela * 0.31f, alturaTela * 0.88f }, (Vector2){ larguraTela * 0.93f, alturaTela * 0.88f }, 1.0f, Fade(cor, 0.16f));
    DrawRectangle((int)(larguraTela * 0.26f), (int)(alturaTela * 0.77f), 8, 8, Fade(cor, 0.12f));
    DrawRectangleLines((int)(larguraTela * 0.13f), (int)(alturaTela * 0.59f), 16, 16, Fade(cor, 0.13f));
    DrawRectangle((int)(larguraTela * 0.88f), (int)(alturaTela * 0.70f), 5, 5, Fade(cor, 0.70f));
    DrawRectangle((int)(larguraTela * 0.21f), (int)(alturaTela * 0.31f), 5, 5, Fade(cor, 0.40f));

    for (int i = 0; i < 6; i++)
    {
        DrawRectangle(52, (int)(alturaTela * 0.44f) + i * 16, 4, 4, Fade(cor, 0.36f));
        DrawRectangle(larguraTela - 56, (int)(alturaTela * 0.44f) + i * 16, 4, 4, Fade(cor, 0.36f));
    }
}

static Color ObterCorMenuFase(int numeroFase)
{
    switch (numeroFase)
    {
        case 2: return (Color){ 32, 235, 92, 255 };
        case 3: return (Color){ 255, 55, 72, 255 };
        default: return (Color){ 22, 150, 255, 255 };
    }
}

static void DesenharPreenchimentoPainelAngular(Rectangle area, float corte, Color cor)
{
    DrawRectangle((int)(area.x + corte), (int)area.y, (int)(area.width - corte * 2.0f), (int)area.height, cor);
    DrawRectangle((int)area.x, (int)(area.y + corte), (int)area.width, (int)(area.height - corte * 2.0f), cor);

    DrawTriangle(
        (Vector2){ area.x + corte, area.y },
        (Vector2){ area.x, area.y + corte },
        (Vector2){ area.x + corte, area.y + corte },
        cor
    );
    DrawTriangle(
        (Vector2){ area.x + area.width - corte, area.y },
        (Vector2){ area.x + area.width, area.y + corte },
        (Vector2){ area.x + area.width - corte, area.y + corte },
        cor
    );
    DrawTriangle(
        (Vector2){ area.x, area.y + area.height - corte },
        (Vector2){ area.x + corte, area.y + area.height },
        (Vector2){ area.x + corte, area.y + area.height - corte },
        cor
    );
    DrawTriangle(
        (Vector2){ area.x + area.width, area.y + area.height - corte },
        (Vector2){ area.x + area.width - corte, area.y + area.height },
        (Vector2){ area.x + area.width - corte, area.y + area.height - corte },
        cor
    );
}

static void DesenharContornoPainelAngular(Rectangle area, float corte, float espessura, Color cor)
{
    Vector2 pontos[] = {
        { area.x + corte, area.y },
        { area.x + area.width - corte, area.y },
        { area.x + area.width, area.y + corte },
        { area.x + area.width, area.y + area.height - corte },
        { area.x + area.width - corte, area.y + area.height },
        { area.x + corte, area.y + area.height },
        { area.x, area.y + area.height - corte },
        { area.x, area.y + corte }
    };

    for (int i = 0; i < 8; i++)
    {
        DrawLineEx(pontos[i], pontos[(i + 1) % 8], espessura, cor);
    }
}

static void DesenharPainelAngular(Rectangle area, Color cor, bool selecionado)
{
    float corte = 16.0f;

    DesenharPreenchimentoPainelAngular(area, corte, Fade((Color){ 1, 8, 20, 255 }, selecionado ? 0.92f : 0.72f));
    DesenharPreenchimentoPainelAngular(
        (Rectangle){ area.x - 7.0f, area.y - 6.0f, area.width + 14.0f, area.height + 12.0f },
        corte + 3.0f,
        Fade(cor, selecionado ? 0.07f : 0.025f)
    );
    DesenharContornoPainelAngular(area, corte, selecionado ? 2.0f : 1.4f, Fade(cor, selecionado ? 0.98f : 0.72f));
    DrawLineEx(
        (Vector2){ area.x + area.width - 9.0f, area.y + 10.0f },
        (Vector2){ area.x + area.width - 9.0f, area.y + area.height - 10.0f },
        3.0f,
        Fade(cor, selecionado ? 0.32f : 0.10f)
    );
}

static void DesenharControleRodape(Rectangle caixa, const char *atalho, const char *linha1, const char *linha2, Color cor)
{
    DrawRectangleLinesEx(caixa, 1.4f, Fade(cor, 0.95f));
    DesenharTextoEspacado(atalho, (int)(caixa.x + caixa.width * 0.5f), (int)(caixa.y + 10.0f), 12, 1, cor);
    DrawText(linha1, (int)(caixa.x + caixa.width + 20.0f), (int)(caixa.y + 3.0f), 14, Fade((Color){ 190, 215, 240, 255 }, 0.92f));
    DrawText(linha2, (int)(caixa.x + caixa.width + 20.0f), (int)(caixa.y + 22.0f), 12, Fade((Color){ 160, 190, 225, 255 }, 0.78f));
}

// Funcao que desenha o menu principal com opcoes de jogar ou editar.
void DesenharMenu(int larguraTela, int alturaTela, int opcaoSelecionada)
{
    Color corFundo = (Color){ 1, 7, 18, 255 };
    Color azulEletrico = (Color){ 22, 150, 255, 255 };
    Color brancoFrio = (Color){ 228, 242, 255, 255 };
    Color linhaFraca = Fade((Color){ 92, 153, 210, 255 }, 0.45f);
    const char *opcoes[] = { "JOGAR", "EDITOR", "SAIR" };
    int quantidadeOpcoes = 3;
    int centroX = larguraTela / 2;
    int larguraBotao = 378;
    int alturaBotao = 52;
    int inicioY = (int)(alturaTela * 0.44f);

    ClearBackground(corFundo);

    DrawRectangleGradientV(0, 0, larguraTela, alturaTela, Fade((Color){ 2, 18, 42, 255 }, 0.36f), Fade(corFundo, 1.0f));
    DrawRectangle(0, 0, larguraTela, alturaTela, Fade(BLACK, 0.18f));
    DesenharDetalhesFundoMenu(larguraTela, alturaTela, azulEletrico);
    DesenharCantosMenu(larguraTela, alturaTela, linhaFraca);

    DrawText("v1.0.0", larguraTela - 112, 42, 18, azulEletrico);
    DrawRectangle(larguraTela - 112, 76, 4, 4, azulEletrico);
    DrawRectangle(larguraTela - 94, 76, 4, 4, azulEletrico);
    DrawRectangle(larguraTela - 76, 76, 4, 4, Fade(azulEletrico, 0.12f));

    DesenharTextoEspacado("PULSE DASH", centroX + 4, (int)(alturaTela * 0.20f), 56, 18, Fade(BLACK, 0.62f));
    DesenharTextoEspacado("PULSE DASH", centroX, (int)(alturaTela * 0.195f), 56, 18, brancoFrio);
    DesenharPulsoTitulo(centroX, (int)(alturaTela * 0.295f), azulEletrico);

    for (int i = 0; i < quantidadeOpcoes; i++)
    {
        bool selecionada = (i == opcaoSelecionada);
        int posY = inicioY + i * 70;
        Rectangle areaOpcao = {
            centroX - larguraBotao * 0.5f,
            (float)posY,
            (float)larguraBotao,
            (float)alturaBotao
        };
        Color corTexto = selecionada ? brancoFrio : Fade(brancoFrio, 0.92f);
        Color corIcone = selecionada ? azulEletrico : Fade(azulEletrico, 0.88f);

        if (selecionada)
        {
            DrawRectangleRounded(
                (Rectangle){ areaOpcao.x - 10.0f, areaOpcao.y - 9.0f, areaOpcao.width + 20.0f, areaOpcao.height + 18.0f },
                0.10f,
                8,
                Fade(azulEletrico, 0.06f)
            );
            DrawRectangleRounded(areaOpcao, 0.08f, 8, Fade((Color){ 3, 14, 33, 255 }, 0.86f));
            DrawRectangleRoundedLines(areaOpcao, 0.08f, 8, Fade(azulEletrico, 0.98f));
            DrawLineEx(
                (Vector2){ areaOpcao.x + areaOpcao.width - 8.0f, areaOpcao.y + 2.0f },
                (Vector2){ areaOpcao.x + areaOpcao.width - 8.0f, areaOpcao.y + areaOpcao.height - 2.0f },
                3.0f,
                Fade(azulEletrico, 0.38f)
            );
            DrawRectangleGradientH(
                (int)(areaOpcao.x + areaOpcao.width - 68.0f),
                (int)(areaOpcao.y + 1.0f),
                68,
                (int)(areaOpcao.height - 2.0f),
                Fade(azulEletrico, 0.00f),
                Fade(azulEletrico, 0.16f)
            );
        }
        else
        {
            DrawLineEx(
                (Vector2){ areaOpcao.x, areaOpcao.y + areaOpcao.height + 15.0f },
                (Vector2){ areaOpcao.x + areaOpcao.width, areaOpcao.y + areaOpcao.height + 15.0f },
                1.0f,
                Fade((Color){ 125, 170, 220, 255 }, 0.22f)
            );
        }

        DesenharIconeMenu(i, (Vector2){ areaOpcao.x + 42.0f, areaOpcao.y + areaOpcao.height * 0.5f }, corIcone);
        DesenharTextoEspacado(opcoes[i], centroX, posY + 18, 22, 7, Fade(BLACK, selecionada ? 0.60f : 0.28f));
        DesenharTextoEspacado(opcoes[i], centroX, posY + 16, 22, 7, corTexto);
    }

    DrawLineEx(
        (Vector2){ larguraTela * 0.31f, alturaTela - 86.0f },
        (Vector2){ larguraTela * 0.69f, alturaTela - 86.0f },
        7.0f,
        Fade(azulEletrico, 0.05f)
    );
    DesenharLinhaHorizontalGlow(larguraTela * 0.39f, alturaTela - 84.0f, larguraTela * 0.22f, azulEletrico);

    DrawText("[ SETA CIMA / BAIXO ]", 68, alturaTela - 50, 14, azulEletrico);
    DrawText("SELECIONAR", 230, alturaTela - 50, 14, Fade(brancoFrio, 0.92f));
    DrawText("|", 332, alturaTela - 50, 14, Fade(brancoFrio, 0.68f));
    DrawText("[ ENTER ]", 356, alturaTela - 50, 14, azulEletrico);
    DrawText("CONFIRMAR", 438, alturaTela - 50, 14, Fade(brancoFrio, 0.92f));
}

// Funcao que implementa a logica do menu.
ModoAplicacao AtualizarMenu(int *opcaoSelecionada)
{
    // Movimento entre opcoes com setas.
    if (IsKeyPressed(KEY_UP))
    {
        *opcaoSelecionada = (*opcaoSelecionada - 1 + 3) % 3;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        *opcaoSelecionada = (*opcaoSelecionada + 1) % 3;
    }

    // Confirmacao com Enter.
    if (IsKeyPressed(KEY_ENTER))
    {
        if (*opcaoSelecionada == 0)
        {
            return MODO_SELECAO_FASE;
        }
        else if (*opcaoSelecionada == 1)
        {
            return MODO_EDITOR;
        }
        else
        {
            return MODO_SAIR;
        }
    }

    return MODO_MENU;
}

static void DesenharMenuSelecaoFase(int larguraTela, int alturaTela, int opcaoSelecionada, const float progressoFases[TOTAL_FASES])
{
    if (progressoFases == NULL)
    {
        fprintf(stderr, "Erro: progressoFases eh NULL em DesenharMenuSelecaoFase\n");
        return;
    }

    Color corFundo = (Color){ 1, 7, 18, 255 };
    Color azulEletrico = (Color){ 22, 150, 255, 255 };
    Color brancoFrio = (Color){ 228, 242, 255, 255 };
    Color linhaFraca = Fade((Color){ 92, 153, 210, 255 }, 0.45f);
    int centroX = larguraTela / 2;

    int quantidadeOpcoes = TOTAL_FASES + 1;
    int larguraCartao = 580;
    int alturaCartao = 90;
    int posY = 176;

    ClearBackground(corFundo);

    DrawRectangleGradientV(0, 0, larguraTela, alturaTela, Fade((Color){ 2, 18, 42, 255 }, 0.34f), Fade(corFundo, 1.0f));
    DrawRectangle(0, 0, larguraTela, alturaTela, Fade(BLACK, 0.18f));
    DesenharDetalhesFundoMenu(larguraTela, alturaTela, azulEletrico);
    DesenharCantosMenu(larguraTela, alturaTela, linhaFraca);

    DrawRectangle(60, 60, 4, 4, Fade(azulEletrico, 0.45f));
    DrawRectangle(76, 60, 4, 4, Fade(azulEletrico, 0.75f));
    DrawRectangle(92, 60, 4, 4, Fade(azulEletrico, 0.95f));
    DrawRectangle(108, 60, 4, 4, Fade(azulEletrico, 0.75f));
    DrawRectangle(larguraTela - 116, 56, 5, 5, azulEletrico);
    DrawRectangle(larguraTela - 100, 56, 5, 5, azulEletrico);
    DrawRectangle(larguraTela - 84, 56, 5, 5, Fade(azulEletrico, 0.75f));
    DrawRectangle(larguraTela - 68, 56, 5, 5, Fade(azulEletrico, 0.55f));

    DesenharTextoEspacado("SELECIONAR FASE", centroX + 3, 80, 38, 8, Fade(BLACK, 0.65f));
    DesenharTextoEspacado("SELECIONAR FASE", centroX, 76, 38, 8, brancoFrio);
    DesenharPulsoTitulo(centroX, 132, azulEletrico);

    for (int i = 0; i < quantidadeOpcoes; i++)
    {
        bool selecionada = (i == opcaoSelecionada);
        Rectangle areaOpcao = {
            centroX - larguraCartao * 0.5f,
            (float)posY,
            (float)larguraCartao,
            (float)alturaCartao
        };

        if (i < TOTAL_FASES)
        {
            int numeroFase = i + 1;
            Color corFase = ObterCorMenuFase(numeroFase);
            float progresso = fmaxf(0.0f, fminf(progressoFases[i], 1.0f));
            int percentual = (int)roundf(progresso * 100.0f);
            float larguraBarra = areaOpcao.width - 72.0f;
            float xBarra = areaOpcao.x + 36.0f;
            float yBarra = areaOpcao.y + 64.0f;

            DesenharPainelAngular(areaOpcao, corFase, selecionada);

            DrawText(TextFormat("FASE %d", numeroFase), (int)areaOpcao.x + 36, posY + 28, 28, brancoFrio);
            DrawText(TextFormat("%03d%%", percentual), (int)(areaOpcao.x + areaOpcao.width - 100.0f), posY + 27, 28, corFase);

            DrawRectangleRounded((Rectangle){ xBarra, yBarra, larguraBarra, 7.0f }, 0.40f, 8, Fade(BLACK, 0.62f));
            DrawRectangleRounded((Rectangle){ xBarra, yBarra, larguraBarra * progresso, 7.0f }, 0.40f, 8, Fade(corFase, 0.95f));
            DrawLineEx((Vector2){ xBarra, yBarra + 3.0f }, (Vector2){ xBarra + larguraBarra * progresso, yBarra + 3.0f }, 7.0f, Fade(corFase, 0.18f));
        }
        else
        {
            Rectangle areaVoltar = {
                areaOpcao.x,
                areaOpcao.y - 2.0f,
                areaOpcao.width,
                70.0f
            };

            DesenharPainelAngular(areaVoltar, azulEletrico, selecionada);
            DrawLineEx((Vector2){ areaVoltar.x + 42.0f, areaVoltar.y + 35.0f }, (Vector2){ areaVoltar.x + 54.0f, areaVoltar.y + 23.0f }, 2.4f, azulEletrico);
            DrawLineEx((Vector2){ areaVoltar.x + 42.0f, areaVoltar.y + 35.0f }, (Vector2){ areaVoltar.x + 54.0f, areaVoltar.y + 47.0f }, 2.4f, azulEletrico);
            DesenharTextoEspacado("VOLTAR", centroX, (int)areaVoltar.y + 24, 22, 8, brancoFrio);
        }

        posY += (i < TOTAL_FASES) ? 116 : 92;
    }

    DesenharControleRodape((Rectangle){ larguraTela * 0.28f, alturaTela - 62.0f, 22.0f, 28.0f }, "^v", "SETA CIMA / BAIXO", "SELECIONAR", azulEletrico);
    DrawText("|", (int)(larguraTela * 0.44f), alturaTela - 52, 18, Fade(brancoFrio, 0.38f));
    DesenharControleRodape((Rectangle){ larguraTela * 0.48f, alturaTela - 62.0f, 48.0f, 28.0f }, "<-->", "ENTER", "CONFIRMAR", azulEletrico);
    DrawText("|", (int)(larguraTela * 0.61f), alturaTela - 52, 18, Fade(brancoFrio, 0.38f));
    DesenharControleRodape((Rectangle){ larguraTela * 0.65f, alturaTela - 62.0f, 38.0f, 28.0f }, "ESC", "VOLTAR", "", azulEletrico);
}

static int AtualizarMenuSelecaoFase(int *opcaoSelecionada)
{
    int quantidadeOpcoes = TOTAL_FASES + 1;

    if (IsKeyPressed(KEY_UP))
    {
        *opcaoSelecionada = (*opcaoSelecionada - 1 + quantidadeOpcoes) % quantidadeOpcoes;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        *opcaoSelecionada = (*opcaoSelecionada + 1) % quantidadeOpcoes;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        return -1; // -1 significa voltar ao menu
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (*opcaoSelecionada < TOTAL_FASES)
        {
            return *opcaoSelecionada + 1; // Retorna numero da fase (1-3)
        }

        return -1; // Botao VOLTAR selecionado - voltar ao menu
    }

    return 0; // 0 = nenhuma acao, continua neste modo
}

// Desenha o menu de pausa durante o jogo.
void DesenharMenuPausa(int larguraTela, int alturaTela, EstiloCena estilo, int opcaoSelecionada)
{
    // Overlay escuro para destaque do menu.
    DrawRectangle(0, 0, larguraTela, alturaTela, Fade(BLACK, 0.6f));

    // Desenha o titulo do menu de pausa.
    const char *titulo = "PAUSA";
    int larguraTitulo = MeasureText(titulo, 48);
    DrawText(titulo, larguraTela / 2 - larguraTitulo / 2, 150, 48, estilo.azulNeon);

    // Desenha as opcoes do menu de pausa.
    const char *opcoes[] = { "CONTINUAR", "VOLTAR AO MENU" };
    int quantidadeOpcoes = 2;
    int espacoVertical = 100;
    int posY = 300;

    for (int i = 0; i < quantidadeOpcoes; i++)
    {
        Color corTexto = (i == opcaoSelecionada) ? (Color){ 255, 255, 255, 255 } : estilo.azulNeon;
        int larguraOpcao = MeasureText(opcoes[i], 32);

        // Desenha um retangulo realcado para a opcao selecionada.
        if (i == opcaoSelecionada)
        {
            DrawRectangle(larguraTela / 2 - larguraOpcao / 2 - 20, posY - 10, 
                          larguraOpcao + 40, 50, Fade((Color){ 52, 182, 255, 255 }, 0.3f));
        }

        DrawText(opcoes[i], larguraTela / 2 - larguraOpcao / 2, posY, 32, corTexto);
        posY += espacoVertical;
    }

    // Instrucoes na parte inferior.
    const char *instrucoes = "[SETA CIMA/BAIXO] Selecionar | [ENTER] Confirmar | [ESC] Continuar";
    int larguraInstrucoes = MeasureText(instrucoes, 16);
    DrawText(instrucoes, larguraTela / 2 - larguraInstrucoes / 2, alturaTela - 50, 16, Fade(estilo.azulNeon, 0.7f));
}

// Atualiza a logica do menu de pausa.
int AtualizarMenuPausa(int *opcaoSelecionada)
{
    // Movimento entre opcoes com setas.
    if (IsKeyPressed(KEY_UP))
    {
        *opcaoSelecionada = (*opcaoSelecionada - 1 + 2) % 2;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        *opcaoSelecionada = (*opcaoSelecionada + 1) % 2;
    }

    // Confirmacao com Enter.
    if (IsKeyPressed(KEY_ENTER))
    {
        return *opcaoSelecionada; // 0 = continuar, 1 = voltar ao menu
    }

    // ESC para voltar ao jogo (selecionando continuar).
    if (IsKeyPressed(KEY_ESCAPE))
    {
        return 0; // Continuar o jogo
    }

    return -1; // Nenhuma acao
}

static const char *ObterCaminhoFase(int numeroFase)
{
    switch (numeroFase)
    {
        case 1: return CAMINHO_FASE_1;
        case 2: return CAMINHO_FASE_2;
        case 3: return CAMINHO_FASE_3;
        default: return CAMINHO_FASE_1;
    }
}

static void PrepararEditorParaFase(EditorFase *editor, int numeroFase)
{
    if (editor == NULL)
    {
        return;
    }

    editor->faseEdicao = numeroFase;
    editor->cameraX = 0.0f;
    editor->mostrandoPreview = false;
    snprintf(editor->nomeFase, sizeof(editor->nomeFase), "Fase %d", numeroFase);
}

static bool CarregarFasePorNumero(EditorFase *editor, int numeroFase)
{
    if (editor == NULL)
    {
        return false;
    }

    PrepararEditorParaFase(editor, numeroFase);
    LimparEspinhosEditor(editor);

    if (CarregarFaseEditor(editor, ObterCaminhoFase(numeroFase)))
    {
        editor->faseEdicao = numeroFase;
        return true;
    }

    if (numeroFase == 1 && CarregarFaseEditor(editor, CAMINHO_FASE_CUSTOMIZADA))
    {
        editor->faseEdicao = numeroFase;
        return true;
    }

    PrepararEditorParaFase(editor, numeroFase);
    return false;
}

static bool SalvarFaseAtual(EditorFase *editor)
{
    if (editor == NULL)
    {
        return false;
    }

    return SalvarFaseEditor(editor, ObterCaminhoFase(editor->faseEdicao));
}

static void ZerarProgressosFases(float progressoFases[TOTAL_FASES])
{
    for (int i = 0; i < TOTAL_FASES; i++)
    {
        progressoFases[i] = 0.0f;
    }
}

static void CarregarProgressosFases(float progressoFases[TOTAL_FASES])
{
    if (progressoFases == NULL)
    {
        return;
    }

    ZerarProgressosFases(progressoFases);

    FILE *arquivo = fopen(CAMINHO_PROGRESSO_FASES, "rb");

    if (arquivo == NULL)
    {
        return;
    }

    size_t lidos = fread(progressoFases, sizeof(float), TOTAL_FASES, arquivo);
    fclose(arquivo);

    if (lidos != TOTAL_FASES)
    {
        fprintf(stderr, "Aviso: Lidos %zu de %d elementos de progresso\n", lidos, TOTAL_FASES);
    }

    for (int i = 0; i < TOTAL_FASES; i++)
    {
        if (!isnan(progressoFases[i]) && !isinf(progressoFases[i]))
        {
            progressoFases[i] = fmaxf(0.0f, fminf(progressoFases[i], 1.0f));
        }
        else
        {
            progressoFases[i] = 0.0f;
        }
    }
}

static bool SalvarProgressosFases(const float progressoFases[TOTAL_FASES])
{
    if (progressoFases == NULL)
    {
        fprintf(stderr, "Erro: progressoFases eh NULL em SalvarProgressosFases\n");
        return false;
    }

    FILE *arquivo = fopen(CAMINHO_PROGRESSO_FASES, "wb");

    if (arquivo == NULL)
    {
        fprintf(stderr, "Erro: Nao foi possivel abrir arquivo de progresso para escrita\n");
        return false;
    }

    size_t escritos = fwrite(progressoFases, sizeof(float), TOTAL_FASES, arquivo);
    fclose(arquivo);

    if (escritos != TOTAL_FASES)
    {
        fprintf(stderr, "Erro: Escritos %zu de %d elementos de progresso\n", escritos, TOTAL_FASES);
        return false;
    }

    return true;
}

static void DesenharMensagemConclusaoFase(int larguraTela, int alturaTela, EstiloCena estilo, int numeroFase)
{
    const char *titulo = TextFormat("FASE %d CONCLUIDA", numeroFase);
    const char *subtitulo = "O CUBO ENTROU NA PORTA";
    int larguraTitulo = MeasureText(titulo, 44);
    int larguraSubtitulo = MeasureText(subtitulo, 20);

    DrawRectangleRounded(
        (Rectangle){ larguraTela * 0.5f - 250.0f, alturaTela * 0.5f - 82.0f, 500.0f, 132.0f },
        0.12f,
        10,
        Fade((Color){ 4, 8, 22, 255 }, 0.82f)
    );
    DrawText(titulo, larguraTela / 2 - larguraTitulo / 2 + 4, alturaTela / 2 - 26, 44, Fade(BLACK, 0.50f));
    DrawText(titulo, larguraTela / 2 - larguraTitulo / 2, alturaTela / 2 - 30, 44, Fade((Color){ 120, 235, 255, 255 }, 0.98f));
    DrawText(subtitulo, larguraTela / 2 - larguraSubtitulo / 2 + 3, alturaTela / 2 + 28, 20, Fade(BLACK, 0.45f));
    DrawText(subtitulo, larguraTela / 2 - larguraSubtitulo / 2, alturaTela / 2 + 24, 20, Fade(estilo.azulNeon, 0.92f));
}

// Versao do LoopJogo que suporta fases customizadas.
bool LoopJogoComFase(int larguraTela, int alturaTela, EstiloCena estilo, int numeroFase, int totalFases,
                     float alturaChao, 
                     float chaoY, float larguraJogador, float alturaJogador, float velocidadeJogador,
                     float gravidade, float forcaPulo, float larguraBlocoChao, float inicioJogadorX,
                     DadosEspinho *espinhosCustomizados, int quantidadeEspinhosCustomizados,
                     float *progressoMaximoFase)
{
    if (espinhosCustomizados == NULL && quantidadeEspinhosCustomizados > 0)
    {
        fprintf(stderr, "Erro: espinhosCustomizados eh NULL mas quantidadeEspinhosCustomizados = %d\n", quantidadeEspinhosCustomizados);
        return false;
    }

    Rectangle portaSaida = CriarPortaSaidaFase(espinhosCustomizados, quantidadeEspinhosCustomizados, chaoY);
    float limiteDireitoUltimoEspinho = ObterLimiteDireitoUltimoEspinho(espinhosCustomizados, quantidadeEspinhosCustomizados);

    // Armazena todas as particulas dos efeitos de corrida e pouso.
    ParticulaPoeira particulasPoeira[MAX_PARTICULAS_POEIRA] = { 0 };

    // Controla o intervalo entre os pequenos sopros de energia da corrida.
    float temporizadorPoeiraCorrida = 0.0f;

    // Guarda quantas vezes o jogador colidiu com espinhos nesta execucao.
    int contadorColisoes = 0;

    // Mantem um pequeno flash de aviso depois de uma colisao.
    float temporizadorFlashDano = 0.0f;

    // Evita colisao repetida logo apos o respawn do personagem.
    float temporizadorProtecaoRespawn = 0.0f;

    // Marca quando a saida pode ser usada e quando a fase esta encerrando.
    bool portaLiberada = false;
    bool faseConcluida = false;
    float temporizadorConclusaoFase = 0.0f;
    float progressoFase = 0.0f;
    float tempoAnimacaoPorta = 0.0f;

    // Cria o personagem principal com fisica e animacao.
    Jogador jogador = CriarJogador(
        inicioJogadorX,
        chaoY,
        larguraJogador,
        alturaJogador
    );

    // Configura uma camera 2D para acompanhar o personagem horizontalmente.
    Camera2D camera = { 0 };

    // Mantem o boneco mais para a esquerda da tela, como em jogos de corrida lateral.
    camera.offset = (Vector2){ larguraTela * 0.30f, alturaTela * 0.55f };

    // A camera olha para a posicao central do personagem no mundo.
    camera.target = (Vector2){ jogador.limites.x + jogador.limites.width / 2.0f, jogador.limites.y + jogador.limites.height / 2.0f };

    // Afasta levemente a camera para mostrar mais da fase.
    camera.rotation = 0.0f;
    camera.zoom = 0.86f;

    // Controla se o jogo esta em pausa.
    bool emPausa = false;
    int opcaoPausa = 0;

    // Controla se deve retornar ao menu principal.
    bool voltarAoMenu = false;
    bool faseFinalizadaComSucesso = false;
    float melhorProgressoFase = progressoMaximoFase != NULL ? *progressoMaximoFase : 0.0f;

    // Loop de jogo: executa enquanto o jogador nao fecha a janela.
    while (!WindowShouldClose() && !voltarAoMenu)
    {
        // Se estiver em pausa, atualiza o menu de pausa.
        if (emPausa)
        {
            int resultado = AtualizarMenuPausa(&opcaoPausa);
            if (resultado == 0)
            {
                // Continuar o jogo.
                emPausa = false;
            }
            else if (resultado == 1)
            {
                // Voltar ao menu principal.
                voltarAoMenu = true;
                break;
            }
        }
        else
        {
            // Trata a pausa com a tecla P apenas enquanto a fase ainda nao acabou.
            if (!faseConcluida && IsKeyPressed(KEY_P))
            {
                emPausa = true;
                opcaoPausa = 0;
            }

            // Calcula o tempo do frame atual para o movimento ficar suave.
            float tempoFrame = GetFrameTime();
            tempoAnimacaoPorta += tempoFrame;

            // Atualiza o tempo restante do flash de dano e da protecao de respawn.
            temporizadorFlashDano = fmaxf(0.0f, temporizadorFlashDano - tempoFrame);
            temporizadorProtecaoRespawn = fmaxf(0.0f, temporizadorProtecaoRespawn - tempoFrame);

            if (faseConcluida)
            {
                temporizadorConclusaoFase = fmaxf(0.0f, temporizadorConclusaoFase - tempoFrame);
                AtualizarJogadorEntrandoNaPorta(&jogador, portaSaida, tempoFrame);
                AtualizarParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA, tempoFrame);
                progressoFase = 1.0f;
                melhorProgressoFase = 1.0f;

                if (temporizadorConclusaoFase <= 0.0f)
                {
                    faseFinalizadaComSucesso = true;
                    voltarAoMenu = true;
                }
            }
            else
            {
                // Atualiza o personagem com movimento automatico, pulo e gravidade.
                AtualizarJogador(&jogador, tempoFrame, velocidadeJogador, gravidade, forcaPulo, chaoY);

                // Cria o rastro de energia enquanto o personagem corre no chao.
                if (jogador.estaNoChao)
                {
                    CriarPoeiraCorrida(particulasPoeira, MAX_PARTICULAS_POEIRA, jogador.limites, tempoFrame, &temporizadorPoeiraCorrida);
                }
                else
                {
                    // Zera o temporizador no ar para o rastro voltar bem sincronizado ao pousar.
                    temporizadorPoeiraCorrida = 0.0f;
                }

                // Dispara um efeito mais forte exatamente no frame do pouso.
                if (JogadorAcabouDePousar(&jogador))
                {
                    CriarPoeiraPouso(particulasPoeira, MAX_PARTICULAS_POEIRA, jogador.limites);
                }

                // Testa colisao com os espinhos customizados da fase.
                if (temporizadorProtecaoRespawn <= 0.0f &&
                    VerificarColisaoComEspinhos(jogador.limites, espinhosCustomizados, quantidadeEspinhosCustomizados, chaoY))
                {
                    // Conta a colisao para facilitar os testes de gameplay.
                    contadorColisoes++;

                    // Ativa um aviso visual curto na interface.
                    temporizadorFlashDano = 0.65f;

                    // Da um pequeno tempo de invulnerabilidade para evitar varias colisoes seguidas no mesmo frame.
                    temporizadorProtecaoRespawn = 0.80f;

                    // Limpa as particulas antigas para o respawn comecar visualmente limpo.
                    LimparParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA);

                    // Zera o temporizador do rastro para ele voltar sincronizado apos o respawn.
                    temporizadorPoeiraCorrida = 0.0f;

                    // Reposiciona o personagem no comeco da fase.
                    ReiniciarJogador(&jogador, inicioJogadorX, chaoY);
                }

                portaLiberada = (jogador.limites.x + jogador.limites.width) >= (limiteDireitoUltimoEspinho + 12.0f);
                progressoFase = CalcularProgressoFase(
                    jogador.limites.x + jogador.limites.width * 0.5f,
                    inicioJogadorX,
                    portaSaida
                );
                melhorProgressoFase = fmaxf(melhorProgressoFase, progressoFase);

                if (portaLiberada && VerificarJogadorEntrouNaPorta(jogador.limites, portaSaida))
                {
                    faseConcluida = true;
                    temporizadorConclusaoFase = 1.05f;
                    progressoFase = 1.0f;
                    LimparParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA);
                    temporizadorPoeiraCorrida = 0.0f;
                }

                // Atualiza a simulacao das particulas depois da logica principal do frame.
                AtualizarParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA, tempoFrame);
            }

            // Atualiza o alvo da camera para seguir o centro do personagem no eixo X.
            camera.target = (Vector2){ jogador.limites.x + jogador.limites.width / 2.0f, alturaTela / 2.0f };
        }

        // Inicia a etapa de desenho na tela.
        BeginDrawing();

        // Pinta o fundo com o azul escuro principal do estilo da logo.
        ClearBackground(estilo.corFundo);

        // Entra no modo de desenho com camera para que o mundo "ande" junto com o personagem.
        BeginMode2D(camera);

        // Desenha os blocos geometricos e detalhes luminosos do fundo.
        DesenharFundoEstiloLogo(camera.target.x - camera.offset.x, chaoY, estilo);

        // Desenha o piso completo da fase com blocos, linhas e espinhos decorativos.
        DesenharChaoMundo(camera.target.x, chaoY, alturaChao, larguraBlocoChao, estilo);

        // Desenha os espinhos customizados se existirem.
        DesenharEspinhos(espinhosCustomizados, quantidadeEspinhosCustomizados, chaoY, estilo);

        // Desenha as particulas antes do personagem para a energia ficar parcialmente atras dele.
        DesenharParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA);

        // Desenha o personagem.
        DesenharJogador(&jogador);

        // Desenha a porta depois do personagem para reforcar a entrada visual no fim da fase.
        DesenharPortaSaida(portaSaida, estilo, tempoAnimacaoPorta, portaLiberada || faseConcluida);

        // Sai do modo de camera para desenhar elementos fixos da interface.
        EndMode2D();

        // Desenha a interface fixa com moldura e textos.
        DesenharHudCena(larguraTela, alturaTela, estilo, contadorColisoes, temporizadorFlashDano,
                        progressoFase, portaLiberada, faseConcluida);

        DrawText(TextFormat("FASE %d / %d", numeroFase, totalFases), 34, 156, 20, Fade(BLACK, 0.50f));
        DrawText(TextFormat("FASE %d / %d", numeroFase, totalFases), 30, 152, 20, Fade(estilo.azulNeon, 0.95f));

        if (faseConcluida)
        {
            DesenharMensagemConclusaoFase(larguraTela, alturaTela, estilo, numeroFase);
        }

        // Desenha o menu de pausa se o jogo estiver pausado.
        if (emPausa)
        {
            DesenharMenuPausa(larguraTela, alturaTela, estilo, opcaoPausa);
        }

        // Finaliza o desenho do frame atual.
        EndDrawing();
    }

    if (progressoMaximoFase != NULL)
    {
        *progressoMaximoFase = fmaxf(0.0f, fminf(melhorProgressoFase, 1.0f));
    }

    return faseFinalizadaComSucesso;
}

// Funcao principal do editor de fases.
static bool CarregarFaseCustomizada(EditorFase *editor)
{
    return CarregarFasePorNumero(editor, editor != NULL ? editor->faseEdicao : 1);
}

void LoopEditor(int larguraTela, int alturaTela)
{
    EditorFase *editor = CriarEditorFase();
    
    if (editor == NULL)
    {
        return;
    }

    if (CarregarFaseCustomizada(editor))
    {
        printf("Fase %d carregada automaticamente de: %s\n", editor->faseEdicao, ObterCaminhoFase(editor->faseEdicao));
    }

    // Loop do editor: executa enquanto o jogador nao fecha a janela ou pressiona ESC.
    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE))
    {
        int faseSolicitada = 0;

        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1))
        {
            faseSolicitada = 1;
        }
        else if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2))
        {
            faseSolicitada = 2;
        }
        else if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3))
        {
            faseSolicitada = 3;
        }

        if (faseSolicitada > 0 && faseSolicitada != editor->faseEdicao)
        {
            int faseAnterior = editor->faseEdicao;

            if (SalvarFaseAtual(editor))
            {
                printf("Fase %d salva em: %s\n", faseAnterior, ObterCaminhoFase(faseAnterior));
            }
            else
            {
                printf("Falha ao salvar a Fase %d em: %s\n", faseAnterior, ObterCaminhoFase(faseAnterior));
            }

            if (CarregarFasePorNumero(editor, faseSolicitada))
            {
                printf("Fase %d carregada de: %s\n", faseSolicitada, ObterCaminhoFase(faseSolicitada));
            }
            else
            {
                printf("Fase %d ainda vazia. Edite e salve em: %s\n", faseSolicitada, ObterCaminhoFase(faseSolicitada));
            }
        }

        // Atualiza a logica do editor (entrada do mouse).
        AtualizarEditor(editor);

        // Atalho para salvar.
        if (IsKeyPressed(KEY_S))
        {
            if (SalvarFaseAtual(editor))
            {
                printf("Fase %d salva em: %s\n", editor->faseEdicao, ObterCaminhoFase(editor->faseEdicao));
            }
            else
            {
                printf("Falha ao salvar a Fase %d em: %s\n", editor->faseEdicao, ObterCaminhoFase(editor->faseEdicao));
            }
        }

        // Atalho para carregar.
        if (IsKeyPressed(KEY_L))
        {
            if (CarregarFaseCustomizada(editor))
            {
                printf("Fase %d carregada de: %s\n", editor->faseEdicao, ObterCaminhoFase(editor->faseEdicao));
            }
            else
            {
                printf("Falha ao carregar a Fase %d de: %s\n", editor->faseEdicao, ObterCaminhoFase(editor->faseEdicao));
            }
        }

        // Atalho para limpar.
        if (IsKeyPressed(KEY_C))
        {
            LimparEspinhosEditor(editor);
            printf("Espinhos removidos!\n");
        }

        // Atalho para testar a fase.
        if (IsKeyPressed(KEY_P))
        {
            if (editor->quantidadeEspinhos > 0)
            {
                printf("Testando fase do editor...\n");
                TestarFaseEditor(editor, larguraTela, alturaTela);
            }
            else
            {
                printf("Nenhum espinho para testar!\n");
            }
        }

        // Desenha o editor.
        BeginDrawing();
        DesenharEditor(editor, larguraTela);
        EndDrawing();
    }

    if (SalvarFaseAtual(editor))
    {
        printf("Fase %d salva automaticamente em: %s\n", editor->faseEdicao, ObterCaminhoFase(editor->faseEdicao));
    }
    else
    {
        printf("Falha ao salvar automaticamente a Fase %d em: %s\n", editor->faseEdicao, ObterCaminhoFase(editor->faseEdicao));
    }

    DestruirEditorFase(editor);
}

int main(void)
{
    // Define o tamanho base da janela do jogo.
    const int larguraTela = 1280;
    const int alturaTela = 720;

    // Define a altura do chao e a posicao vertical dele no mundo.
    const float alturaChao = 140.0f;
    const float chaoY = alturaTela - alturaChao;

    // Define o tamanho da area de colisao do personagem.
    const float larguraJogador = 46.0f;
    const float alturaJogador = 46.0f;

    // Define a velocidade horizontal do boneco em pixels por segundo.
    const float velocidadeJogador = 600.0f;

    // Define a forca da gravidade e a velocidade inicial do pulo.
    const float gravidade = 1800.0f;
    const float forcaPulo = -700.0f;

    // Define a largura de cada bloco visual do chao.
    const float larguraBlocoChao = 160.0f;

    // Define a posicao inicial usada quando o jogador precisa voltar apos uma colisao.
    const float inicioJogadorX = 120.0f;

    // Inicializa a janela principal do projeto.
    InitWindow(larguraTela, alturaTela, "Pulse Dash");
    SetExitKey(0);

    // Mantem a atualizacao e o desenho em 60 quadros por segundo.
    SetTargetFPS(60);

    // Loop principal: menu, jogo e editor.
    ModoAplicacao modoAtual = MODO_MENU;
    int opcaoMenuSelecionada = 0;
    int opcaoFaseSelecionada = 0;
    int faseSelecionadaParaJogar = 1;
    float progressoFases[TOTAL_FASES] = { 0 };
    bool aplicacaoRodando = true;

    CarregarProgressosFases(progressoFases);

    while (!WindowShouldClose() && aplicacaoRodando)
    {
        if (modoAtual == MODO_MENU)
        {
            BeginDrawing();
            DesenharMenu(larguraTela, alturaTela, opcaoMenuSelecionada);
            EndDrawing();

            ModoAplicacao novoModo = AtualizarMenu(&opcaoMenuSelecionada);

            if (novoModo == MODO_SAIR)
            {
                aplicacaoRodando = false;
            }
            else if (novoModo != MODO_MENU)
            {
                modoAtual = novoModo;
            }
        }
        else if (modoAtual == MODO_SELECAO_FASE)
        {
            BeginDrawing();
            DesenharMenuSelecaoFase(larguraTela, alturaTela, opcaoFaseSelecionada, progressoFases);
            EndDrawing();

            int resultadoSelecao = AtualizarMenuSelecaoFase(&opcaoFaseSelecionada);

            if (resultadoSelecao > 0)
            {
                faseSelecionadaParaJogar = resultadoSelecao;
                modoAtual = MODO_JOGO;
            }
            else if (resultadoSelecao < 0) // -1 = voltar ao menu
            {
                modoAtual = MODO_MENU;
                opcaoMenuSelecionada = 0;
            }
            // Se resultadoSelecao == 0, continua neste modo
        }
        else if (modoAtual == MODO_JOGO)
        {
            // Carrega e toca a fase escolhida no menu de fases.
            EditorFase faseCarregada = { 0 };
            int numeroFase = faseSelecionadaParaJogar;

            if (CarregarFasePorNumero(&faseCarregada, numeroFase) && faseCarregada.quantidadeEspinhos > 0)
            {
                EstiloCena estiloFase = ObterEstiloCenaFase(numeroFase);
                bool concluiuFase = LoopJogoComFase(larguraTela, alturaTela, estiloFase, numeroFase, TOTAL_FASES,
                                                    alturaChao, chaoY,
                                                    larguraJogador, alturaJogador, velocidadeJogador, gravidade, forcaPulo,
                                                    larguraBlocoChao, inicioJogadorX,
                                                    faseCarregada.espinhos, faseCarregada.quantidadeEspinhos,
                                                    &progressoFases[numeroFase - 1]);

                if (concluiuFase)
                {
                    progressoFases[numeroFase - 1] = 1.0f;
                }

                SalvarProgressosFases(progressoFases);
            }
            else
            {
                printf("Fase %d nao encontrada ou vazia. Crie essa fase no editor.\n", numeroFase);
            }
            
            modoAtual = MODO_SELECAO_FASE;
            opcaoFaseSelecionada = numeroFase - 1;
        }
        else if (modoAtual == MODO_EDITOR)
        {
            LoopEditor(larguraTela, alturaTela);
            
            modoAtual = MODO_MENU;
            opcaoMenuSelecionada = 1;
        }
        else
        {
            // Modo invalido - sair do loop
            fprintf(stderr, "Erro: Modo invalido detectado\n");
            aplicacaoRodando = false;
        }
    }

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
#include "editor.c"
