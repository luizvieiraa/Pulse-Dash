#include "../.H/player.h"
#include <math.h>
#include <stddef.h>

static Vector2 RotacionarPontoAoRedorDoCentro(Vector2 ponto, Vector2 centro, float rotacaoGraus)
{
    float rotacaoRadianos = rotacaoGraus * DEG2RAD;
    float senoRotacao = sinf(rotacaoRadianos);
    float cossenoRotacao = cosf(rotacaoRadianos);
    Vector2 deslocamento = { ponto.x - centro.x, ponto.y - centro.y };

    return (Vector2){
        centro.x + deslocamento.x * cossenoRotacao - deslocamento.y * senoRotacao,
        centro.y + deslocamento.x * senoRotacao + deslocamento.y * cossenoRotacao
    };
}

static void DesenharContornoRetanguloRotacionado(Rectangle retangulo, float rotacaoGraus, float espessura, Color cor)
{
    Vector2 centro = {
        retangulo.x + retangulo.width * 0.5f,
        retangulo.y + retangulo.height * 0.5f
    };
    Vector2 topoEsquerdo = RotacionarPontoAoRedorDoCentro((Vector2){ retangulo.x, retangulo.y }, centro, rotacaoGraus);
    Vector2 topoDireito = RotacionarPontoAoRedorDoCentro((Vector2){ retangulo.x + retangulo.width, retangulo.y }, centro, rotacaoGraus);
    Vector2 baseDireita = RotacionarPontoAoRedorDoCentro((Vector2){ retangulo.x + retangulo.width, retangulo.y + retangulo.height }, centro, rotacaoGraus);
    Vector2 baseEsquerda = RotacionarPontoAoRedorDoCentro((Vector2){ retangulo.x, retangulo.y + retangulo.height }, centro, rotacaoGraus);

    DrawLineEx(topoEsquerdo, topoDireito, espessura, cor);
    DrawLineEx(topoDireito, baseDireita, espessura, cor);
    DrawLineEx(baseDireita, baseEsquerda, espessura, cor);
    DrawLineEx(baseEsquerda, topoEsquerdo, espessura, cor);
}

// Desenha um cubo neon no estilo Geometry Dash dentro de um frame do spritesheet.
static void DesenharFrameSpriteCubo(Rectangle areaFrame, int indiceFrame)
{
    // Define a paleta do cubo no estilo neon da logo.
    const Color corCubo = (Color){ 2, 8, 24, 255 };
    const Color corInteriorCubo = (Color){ 7, 22, 56, 255 };
    const Color corBrilhoExterno = (Color){ 0, 98, 255, 75 };
    const Color corBrilhoMedio = (Color){ 0, 186, 255, 130 };
    const Color corBrilhoCentral = (Color){ 120, 235, 255, 255 };

    float tamanhoCubo = 56.0f;
    float centroX = areaFrame.x + areaFrame.width * 0.52f;
    float chaoY = areaFrame.y + areaFrame.height - 16.0f;
    float cuboY = chaoY - tamanhoCubo;
    float saltoVisual = 0.0f;

    switch (indiceFrame)
    {
        case 0:
            saltoVisual = -2.0f;
            break;

        case 1:
            saltoVisual = 1.0f;
            break;

        case 2:
            saltoVisual = -1.0f;
            break;

        default:
            saltoVisual = -12.0f;
            break;
    }

    Rectangle retanguloCubo = {
        centroX - tamanhoCubo * 0.5f,
        cuboY + saltoVisual,
        tamanhoCubo,
        tamanhoCubo
    };
    DrawEllipse((int)centroX, (int)(chaoY + 6.0f), 24.0f, 5.0f, Fade(BLACK, 0.25f));

    // Desenha brilho e corpo do cubo.
    Rectangle brilhoExterno = { retanguloCubo.x - 4.0f, retanguloCubo.y - 4.0f, tamanhoCubo + 8.0f, tamanhoCubo + 8.0f };
    Rectangle brilhoMedio = { retanguloCubo.x - 2.0f, retanguloCubo.y - 2.0f, tamanhoCubo + 4.0f, tamanhoCubo + 4.0f };
    DrawRectangleRec(brilhoExterno, corBrilhoExterno);
    DrawRectangleRec(brilhoMedio, corBrilhoMedio);
    DrawRectangleRec(retanguloCubo, corCubo);

    Rectangle retanguloInterior = {
        retanguloCubo.x + 8.0f,
        retanguloCubo.y + 8.0f,
        tamanhoCubo - 16.0f,
        tamanhoCubo - 16.0f
    };
    DrawRectangleRec(retanguloInterior, corInteriorCubo);
    DrawRectangleLinesEx(retanguloCubo, 4.0f, corBrilhoCentral);

}

// Monta um spritesheet simples em memoria para o personagem.
static RenderTexture2D ConstruirFolhaSpritesCubo(int larguraFrame, int alturaFrame, int quantidadeFrames)
{
    // Cria uma textura grande o bastante para guardar todos os frames lado a lado.
    RenderTexture2D folhaSprites = LoadRenderTexture(larguraFrame * quantidadeFrames, alturaFrame);

    // Desenha cada frame diretamente dentro do spritesheet.
    BeginTextureMode(folhaSprites);
    ClearBackground(BLANK);

    for (int indiceFrame = 0; indiceFrame < quantidadeFrames; indiceFrame++)
    {
        Rectangle areaFrame = {
            indiceFrame * (float)larguraFrame,
            0.0f,
            (float)larguraFrame,
            (float)alturaFrame
        };

        DesenharFrameSpriteCubo(areaFrame, indiceFrame);
    }

    EndTextureMode();

    return folhaSprites;
}

Jogador CriarJogador(
    float inicioX,
    float chaoY,
    float larguraJogador,
    float alturaJogador,
    int larguraFrameSprite,
    int alturaFrameSprite,
    int quantidadeFramesSprite
)
{
    // Cria a estrutura do personagem com os estados iniciais de movimento.
    Jogador jogador = { 0 };
    jogador.limites = (Rectangle){ inicioX, chaoY - alturaJogador, larguraJogador, alturaJogador };
    jogador.velocidadeY = 0.0f;
    jogador.estaNoChao = true;
    jogador.estavaNoChao = true;
    jogador.tempoAnimacaoCorrida = 0.0f;
    jogador.rotacaoGraus = 0.0f;

    // Gera o spritesheet usado para a animacao de corrida e pulo.
    jogador.folhaSprites = ConstruirFolhaSpritesCubo(larguraFrameSprite, alturaFrameSprite, quantidadeFramesSprite);

    return jogador;
}

void DestruirJogador(Jogador *jogador)
{
    // Libera a textura do personagem apenas se ela tiver sido criada.
    if (jogador != NULL && jogador->folhaSprites.id != 0)
    {
        UnloadRenderTexture(jogador->folhaSprites);
        jogador->folhaSprites.id = 0;
    }
}

void AtualizarJogador(Jogador *jogador, float tempoFrame, float velocidadeJogador, float gravidade, float forcaPulo, float chaoY)
{
    // Ignora a atualizacao caso o ponteiro seja invalido.
    if (jogador == NULL)
    {
        return;
    }

    // Guarda o estado anterior para detectar pouso.
    jogador->estavaNoChao = jogador->estaNoChao;

    // Move o personagem continuamente para a direita.
    jogador->limites.x += velocidadeJogador * tempoFrame;

    // Avanca o relogio da animacao enquanto ele corre no chao.
    if (jogador->estaNoChao)
    {
        jogador->tempoAnimacaoCorrida += tempoFrame;
    }

    // Permite pular segurando espaco, seta para cima ou clique esquerdo.
    if (jogador->estaNoChao && (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)))
    {
        jogador->velocidadeY = forcaPulo;
        jogador->estaNoChao = false;
        jogador->rotacaoGraus = 0.0f;
    }

    // Aplica a gravidade continuamente.
    jogador->velocidadeY += gravidade * tempoFrame;

    // Atualiza a posicao vertical do personagem.
    jogador->limites.y += jogador->velocidadeY * tempoFrame;

    // Faz o cubo completar um mortal limpo durante o pulo normal.
    if (!jogador->estaNoChao)
    {
        jogador->rotacaoGraus = fmodf(jogador->rotacaoGraus + 465.0f * tempoFrame, 360.0f);
    }

    // Impede que ele atravesse o chao e restaura o estado de apoio.
    if (jogador->limites.y >= chaoY - jogador->limites.height)
    {
        jogador->limites.y = chaoY - jogador->limites.height;
        jogador->velocidadeY = 0.0f;
        jogador->estaNoChao = true;
        jogador->rotacaoGraus = roundf(jogador->rotacaoGraus / 90.0f) * 90.0f;
    }
}

void ReiniciarJogador(Jogador *jogador, float inicioX, float chaoY)
{
    // Ignora a operacao caso o ponteiro seja invalido.
    if (jogador == NULL)
    {
        return;
    }

    // Reposiciona o personagem no inicio da fase e limpa o estado de movimento.
    jogador->limites.x = inicioX;
    jogador->limites.y = chaoY - jogador->limites.height;
    jogador->velocidadeY = 0.0f;
    jogador->estaNoChao = true;
    jogador->estavaNoChao = true;
    jogador->tempoAnimacaoCorrida = 0.0f;
    jogador->rotacaoGraus = 0.0f;
}

bool JogadorAcabouDePousar(const Jogador *jogador)
{
    // Retorna verdadeiro apenas quando o personagem estava no ar e acabou de tocar o chao.
    return (jogador != NULL) && !jogador->estavaNoChao && jogador->estaNoChao;
}

int ObterIndiceFrameSpriteJogador(const Jogador *jogador)
{
    // Mantem o frame de pulo enquanto o personagem esta no ar.
    if (jogador == NULL || !jogador->estaNoChao)
    {
        return 3;
    }

    // Alterna entre os tres frames de corrida enquanto ele permanece no chao.
    return ((int)(jogador->tempoAnimacaoCorrida * 12.0f)) % 3;
}

void DesenharJogador(const Jogador *jogador, int larguraFrameSprite, int alturaFrameSprite)
{
    (void)larguraFrameSprite;
    (void)alturaFrameSprite;

    // Interrompe o desenho caso o personagem nao exista.
    if (jogador == NULL)
    {
        return;
    }

    const Color corBrilhoExterno = (Color){ 0, 98, 255, 75 };
    const Color corBrilhoMedio = (Color){ 0, 186, 255, 130 };
    const Color corBrilhoCentral = (Color){ 120, 235, 255, 255 };

    Rectangle retanguloCubo = jogador->limites;
    Rectangle brilhoExterno = {
        retanguloCubo.x - 4.0f,
        retanguloCubo.y - 4.0f,
        retanguloCubo.width + 8.0f,
        retanguloCubo.height + 8.0f
    };
    Rectangle brilhoMedio = {
        retanguloCubo.x - 2.0f,
        retanguloCubo.y - 2.0f,
        retanguloCubo.width + 4.0f,
        retanguloCubo.height + 4.0f
    };

    DrawEllipse(
        (int)(retanguloCubo.x + retanguloCubo.width * 0.5f),
        (int)(retanguloCubo.y + retanguloCubo.height + 6.0f),
        24.0f,
        5.0f,
        Fade(BLACK, 0.25f)
    );

    DesenharContornoRetanguloRotacionado(brilhoExterno, jogador->rotacaoGraus, 5.0f, corBrilhoExterno);
    DesenharContornoRetanguloRotacionado(brilhoMedio, jogador->rotacaoGraus, 4.0f, corBrilhoMedio);
    DesenharContornoRetanguloRotacionado(retanguloCubo, jogador->rotacaoGraus, 4.0f, corBrilhoCentral);
}
