#ifndef TIPOS_JOGO_H
#define TIPOS_JOGO_H

#include "raylib.h"
#include <stdbool.h>

// Define a quantidade maxima de particulas simultaneas para os efeitos visuais.
#define MAX_PARTICULAS_POEIRA 96

// Define a quantidade maxima de espinhos em uma fase editavel.
#define MAX_ESPINHOS_FASE 256

// Guarda os dados de uma particula individual de poeira.
typedef struct ParticulaPoeira
{
    bool ativa;
    Vector2 posicao;
    Vector2 velocidade;
    float vida;
    float vidaMaxima;
    float raio;
    Color cor;
} ParticulaPoeira;

// Guarda os dados de um espinho individual na fase.
typedef struct
{
    float posicaoX;
    float variacaoAltura;
} DadosEspinho;

// Define os modos possiveis da aplicacao.
typedef enum
{
    MODO_MENU,
    MODO_JOGO,
    MODO_EDITOR
} ModoAplicacao;

// Guarda o estado do editor de fases.
typedef struct
{
    DadosEspinho espinhos[MAX_ESPINHOS_FASE];
    int quantidadeEspinhos;
    Vector2 mouseUltimo;
    bool mostrandoPreview;
    int faseEdicao;
    char nomeFase[64];
    float cameraX;  // Posicao horizontal da camera para scroll
    bool testando;  // Se esta testando a fase
} EditorFase;

#endif
