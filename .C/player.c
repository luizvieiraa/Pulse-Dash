#include "../.H/player.h"
#include <math.h>
#include <stddef.h>

static float InterpolarFloat(float atual, float alvo, float fator)
{
    return atual + (alvo - atual) * fator;
}

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

Jogador CriarJogador(
    float inicioX,
    float chaoY,
    float larguraJogador,
    float alturaJogador
)
{
    // Cria a estrutura do personagem com os estados iniciais de movimento.
    Jogador jogador = { 0 };
    jogador.limites = (Rectangle){ inicioX, chaoY - alturaJogador, larguraJogador, alturaJogador };
    jogador.velocidadeY = 0.0f;
    jogador.estaNoChao = true;
    jogador.estavaNoChao = true;
    jogador.rotacaoGraus = 0.0f;

    return jogador;
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
    jogador->rotacaoGraus = 0.0f;
}

void AtualizarJogadorEntrandoNaPorta(Jogador *jogador, Rectangle portaSaida, float tempoFrame)
{
    if (jogador == NULL)
    {
        return;
    }

    float fatorInterpolacao = fminf(1.0f, tempoFrame * 5.5f);
    float alvoX = portaSaida.x + portaSaida.width * 0.5f - jogador->limites.width * 0.5f;
    float alvoY = portaSaida.y + portaSaida.height - jogador->limites.height - 10.0f;

    jogador->limites.x = InterpolarFloat(jogador->limites.x, alvoX, fatorInterpolacao);
    jogador->limites.y = InterpolarFloat(jogador->limites.y, alvoY, fatorInterpolacao);
    jogador->velocidadeY = 0.0f;
    jogador->estaNoChao = true;
    jogador->estavaNoChao = true;
    jogador->rotacaoGraus = InterpolarFloat(jogador->rotacaoGraus, 0.0f, fminf(1.0f, tempoFrame * 8.0f));
}

bool JogadorAcabouDePousar(const Jogador *jogador)
{
    // Retorna verdadeiro apenas quando o personagem estava no ar e acabou de tocar o chao.
    return (jogador != NULL) && !jogador->estavaNoChao && jogador->estaNoChao;
}

void DesenharJogador(const Jogador *jogador)
{
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
