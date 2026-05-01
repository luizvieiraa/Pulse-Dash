#include "../.H/effects.h"
#include <stddef.h>

// Ativa uma particula livre com os parametros recebidos.
static void AtivarParticulaPoeira(
    ParticulaPoeira particulas[],
    int quantidadeParticulas,
    Vector2 posicao,
    Vector2 velocidade,
    float vidaMaxima,
    float raio,
    Color cor
)
{
    // Procura a primeira particula disponivel para reaproveitar o buffer.
    for (int indice = 0; indice < quantidadeParticulas; indice++)
    {
        if (!particulas[indice].ativa)
        {
            particulas[indice].ativa = true;
            particulas[indice].posicao = posicao;
            particulas[indice].velocidade = velocidade;
            particulas[indice].vida = vidaMaxima;
            particulas[indice].vidaMaxima = vidaMaxima;
            particulas[indice].raio = raio;
            particulas[indice].cor = cor;
            break;
        }
    }
}

void CriarPoeiraCorrida(ParticulaPoeira particulas[], int quantidadeParticulas, Rectangle limitesJogador, float tempoFrame, float *temporizadorCriacao)
{
    // Ignora a logica caso o temporizador nao exista.
    if (temporizadorCriacao == NULL)
    {
        return;
    }

    // Controla o intervalo para a poeira nascer em pulsos e nao em excesso.
    *temporizadorCriacao += tempoFrame;

    if (*temporizadorCriacao < 0.045f)
    {
        return;
    }

    *temporizadorCriacao = 0.0f;

    // Posiciona a poeira perto dos pes do personagem.
    Vector2 posicaoBase;
    posicaoBase.x = limitesJogador.x + limitesJogador.width * 0.20f;
    posicaoBase.y = limitesJogador.y + limitesJogador.height + 2.0f;

    // Cria duas particulas por pulso para dar mais densidade ao rastro.
    for (int passoParticula = 0; passoParticula < 2; passoParticula++)
    {
        float velocidadeHorizontal = -(80.0f + (float)GetRandomValue(0, 70));
        float velocidadeVertical = -(25.0f + (float)GetRandomValue(0, 55));
        float raioParticula = 5.0f + (float)GetRandomValue(0, 5);

        Vector2 posParticula;
        posParticula.x = posicaoBase.x + (float)GetRandomValue(-6, 8);
        posParticula.y = posicaoBase.y + (float)GetRandomValue(-2, 4);
        
        Vector2 velParticula;
        velParticula.x = velocidadeHorizontal;
        velParticula.y = velocidadeVertical;
        
        AtivarParticulaPoeira(
            particulas,
            quantidadeParticulas,
            posParticula,
            velParticula,
            0.40f + (float)GetRandomValue(0, 20) / 100.0f,
            raioParticula,
            (Color){ 84, 205, 255, 185 }
        );
    }
}

void CriarPoeiraPouso(ParticulaPoeira particulas[], int quantidadeParticulas, Rectangle limitesJogador)
{
    // Define a origem do pouso no centro dos pes do personagem.
    Vector2 centroPouso;
    centroPouso.x = limitesJogador.x + limitesJogador.width * 0.50f;
    centroPouso.y = limitesJogador.y + limitesJogador.height + 2.0f;

    // Espalha varias particulas para os lados para comunicar impacto.
    for (int indice = 0; indice < 18; indice++)
    {
        float direcaoHorizontal = (indice % 2 == 0) ? -1.0f : 1.0f;
        float velocidadeHorizontal = direcaoHorizontal * (90.0f + (float)GetRandomValue(0, 180));
        float velocidadeVertical = -(70.0f + (float)GetRandomValue(0, 110));
        float raioParticula = 6.0f + (float)GetRandomValue(0, 8);

        Vector2 posPousoParticula;
        posPousoParticula.x = centroPouso.x + direcaoHorizontal * (float)GetRandomValue(0, 18);
        posPousoParticula.y = centroPouso.y;
        
        Vector2 velPousoParticula;
        velPousoParticula.x = velocidadeHorizontal;
        velPousoParticula.y = velocidadeVertical;
        
        AtivarParticulaPoeira(
            particulas,
            quantidadeParticulas,
            posPousoParticula,
            velPousoParticula,
            0.55f + (float)GetRandomValue(0, 25) / 100.0f,
            raioParticula,
            (Color){ 112, 225, 255, 220 }
        );
    }
}

void AtualizarParticulasPoeira(ParticulaPoeira particulas[], int quantidadeParticulas, float tempoFrame)
{
    // Atualiza cada particula ativa aplicando gravidade e perda de vida.
    for (int indice = 0; indice < quantidadeParticulas; indice++)
    {
        if (!particulas[indice].ativa)
        {
            continue;
        }

        particulas[indice].vida -= tempoFrame;

        if (particulas[indice].vida <= 0.0f)
        {
            particulas[indice].ativa = false;
            continue;
        }

        particulas[indice].velocidade.y += 240.0f * tempoFrame;
        particulas[indice].posicao.x += particulas[indice].velocidade.x * tempoFrame;
        particulas[indice].posicao.y += particulas[indice].velocidade.y * tempoFrame;
        particulas[indice].raio *= 0.992f;
    }
}

void DesenharParticulasPoeira(const ParticulaPoeira particulas[], int quantidadeParticulas)
{
    // Desenha as particulas mais velhas mais transparentes para parecer dissipacao.
    for (int indice = 0; indice < quantidadeParticulas; indice++)
    {
        if (!particulas[indice].ativa)
        {
            continue;
        }

        float proporcaoVida = particulas[indice].vida / particulas[indice].vidaMaxima;
        Color corParticula = Fade(particulas[indice].cor, proporcaoVida);

        DrawCircleV(particulas[indice].posicao, particulas[indice].raio, corParticula);
    }
}

void LimparParticulasPoeira(ParticulaPoeira particulas[], int quantidadeParticulas)
{
    // Desativa todas as particulas para reiniciar os efeitos imediatamente.
    for (int indice = 0; indice < quantidadeParticulas; indice++)
    {
        particulas[indice].ativa = false;
        particulas[indice].vida = 0.0f;
        particulas[indice].vidaMaxima = 0.0f;
        particulas[indice].raio = 0.0f;
        particulas[indice].posicao.x = 0.0f;
        particulas[indice].posicao.y = 0.0f;
        particulas[indice].velocidade.x = 0.0f;
        particulas[indice].velocidade.y = 0.0f;
    }
}
