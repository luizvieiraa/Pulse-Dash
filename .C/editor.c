#include "../.H/editor.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Configuracoes visuais do editor.
#define EDITOR_ALTURA_CHAO 550.0f
#define EDITOR_RAIO_SELECAO 25.0f
#define EDITOR_COR_ESPINHO_NORMAL (Color){ 52, 182, 255, 255 }
#define EDITOR_COR_ESPINHO_HOVER (Color){ 100, 220, 255, 255 }
#define EDITOR_COR_PREVIEW (Color){ 52, 182, 255, 100 }
#define EDITOR_TAMANHO_MAX_CAMINHO 512

static void CopiarTextoSeguro(char *destino, size_t tamanhoDestino, const char *origem)
{
    if (destino == NULL || tamanhoDestino == 0)
    {
        return;
    }

    if (origem == NULL)
    {
        destino[0] = '\0';
        return;
    }

    snprintf(destino, tamanhoDestino, "%s", origem);
}

static bool ExtrairDiretorioArquivo(const char *caminhoArquivo, char *diretorio, size_t tamanhoDiretorio)
{
    if (caminhoArquivo == NULL || diretorio == NULL || tamanhoDiretorio == 0)
    {
        return false;
    }

    const char *ultimaBarra = strrchr(caminhoArquivo, '/');
    const char *ultimaContraBarra = strrchr(caminhoArquivo, '\\');
    const char *ultimaSeparacao = ultimaBarra;

    if (ultimaContraBarra != NULL && (ultimaSeparacao == NULL || ultimaContraBarra > ultimaSeparacao))
    {
        ultimaSeparacao = ultimaContraBarra;
    }

    if (ultimaSeparacao == NULL)
    {
        diretorio[0] = '\0';
        return true;
    }

    size_t tamanho = (size_t)(ultimaSeparacao - caminhoArquivo);

    if (tamanho >= tamanhoDiretorio)
    {
        return false;
    }

    memcpy(diretorio, caminhoArquivo, tamanho);
    diretorio[tamanho] = '\0';
    return true;
}

static bool GarantirDiretorioArquivo(const char *caminhoArquivo)
{
    char diretorio[EDITOR_TAMANHO_MAX_CAMINHO];

    if (!ExtrairDiretorioArquivo(caminhoArquivo, diretorio, sizeof(diretorio)))
    {
        return false;
    }

    if (diretorio[0] == '\0')
    {
        return true;
    }

    return DirectoryExists(diretorio) || MakeDirectory(diretorio) == 0 || DirectoryExists(diretorio);
}

static bool ResolverRaizProjeto(char *raizProjeto, size_t tamanhoRaizProjeto)
{
    if (raizProjeto == NULL || tamanhoRaizProjeto == 0)
    {
        return false;
    }

    const char *diretorioAplicacao = GetApplicationDirectory();

    if (diretorioAplicacao == NULL || diretorioAplicacao[0] == '\0')
    {
        return false;
    }

    char diretorioFases[EDITOR_TAMANHO_MAX_CAMINHO];
    char diretorioHeaders[EDITOR_TAMANHO_MAX_CAMINHO];

    snprintf(raizProjeto, tamanhoRaizProjeto, "%s../../", diretorioAplicacao);
    snprintf(diretorioFases, sizeof(diretorioFases), "%s../../fases", diretorioAplicacao);
    snprintf(diretorioHeaders, sizeof(diretorioHeaders), "%s../../.H", diretorioAplicacao);

    return DirectoryExists(diretorioFases) || DirectoryExists(diretorioHeaders);
}

static bool ResolverCaminhoFase(const char *caminhoArquivo, char *caminhoResolvido, size_t tamanhoCaminho, bool paraEscrita)
{
    if (caminhoArquivo == NULL || caminhoResolvido == NULL || tamanhoCaminho == 0)
    {
        return false;
    }

    // Mantem caminhos absolutos intactos.
    if ((strlen(caminhoArquivo) > 1 && caminhoArquivo[1] == ':') || caminhoArquivo[0] == '/' || caminhoArquivo[0] == '\\')
    {
        CopiarTextoSeguro(caminhoResolvido, tamanhoCaminho, caminhoArquivo);
        return true;
    }

    // Se o arquivo ja existe relativo ao diretorio atual, usa esse caminho.
    if (!paraEscrita && FileExists(caminhoArquivo))
    {
        CopiarTextoSeguro(caminhoResolvido, tamanhoCaminho, caminhoArquivo);
        return true;
    }

    // Quando o executavel roda em .C/output, aponta explicitamente para a raiz do projeto.
    char raizProjeto[EDITOR_TAMANHO_MAX_CAMINHO];

    if (ResolverRaizProjeto(raizProjeto, sizeof(raizProjeto)))
    {
        snprintf(caminhoResolvido, tamanhoCaminho, "%s%s", raizProjeto, caminhoArquivo);

        if (paraEscrita || FileExists(caminhoResolvido))
        {
            return true;
        }
    }

    // Fallback para o caminho relativo atual.
    CopiarTextoSeguro(caminhoResolvido, tamanhoCaminho, caminhoArquivo);
    return paraEscrita || FileExists(caminhoResolvido);
}

EditorFase *CriarEditorFase(void)
{
    EditorFase *editor = (EditorFase *)malloc(sizeof(EditorFase));
    
    if (editor == NULL)
    {
        return NULL;
    }

    memset(editor, 0, sizeof(EditorFase));

    editor->mostrandoPreview = false;
    editor->faseEdicao = 1;
    editor->cameraX = 0.0f;
    editor->testando = false;
    strcpy(editor->nomeFase, "Fase Customizada");
    
    return editor;
}

void DestruirEditorFase(EditorFase *editor)
{
    if (editor != NULL)
    {
        free(editor);
    }
}

void LimparEspinhosEditor(EditorFase *editor)
{
    if (editor != NULL)
    {
        editor->quantidadeEspinhos = 0;
    }
}

void AdicionarEspinhoEditor(EditorFase *editor, Vector2 posicao)
{
    if (editor == NULL || editor->quantidadeEspinhos >= MAX_ESPINHOS_FASE)
    {
        return;
    }
    
    // Converte posicao da tela para posicao do mundo (considerando scroll).
    float posicaoXMundo = posicao.x + editor->cameraX;
    
    // Nao deixa adicionar fora da area do chao.
    if (posicao.y < EDITOR_ALTURA_CHAO - 200.0f || posicao.y > EDITOR_ALTURA_CHAO)
    {
        return;
    }
    
    // Nao deixa adicionar muito proximo de outro.
    if (EncontrarEspinhoProximo(editor, (Vector2){ posicaoXMundo, posicao.y }, EDITOR_RAIO_SELECAO * 1.5f) >= 0)
    {
        return;
    }
    
    // Adiciona o novo espinho.
    editor->espinhos[editor->quantidadeEspinhos].posicaoX = posicaoXMundo;
    editor->espinhos[editor->quantidadeEspinhos].variacaoAltura = 0.0f;
    editor->quantidadeEspinhos++;
}

bool RemoverEspinhoEditor(EditorFase *editor, Vector2 posicao)
{
    if (editor == NULL)
    {
        return false;
    }
    
    // Converte posicao da tela para posicao do mundo (considerando scroll).
    float posicaoXMundo = posicao.x + editor->cameraX;
    
    int indice = EncontrarEspinhoProximo(editor, (Vector2){ posicaoXMundo, posicao.y }, EDITOR_RAIO_SELECAO);
    
    if (indice < 0)
    {
        return false;
    }
    
    // Move o ultimo espinho para preencher o buraco.
    if (indice < editor->quantidadeEspinhos - 1)
    {
        editor->espinhos[indice] = editor->espinhos[editor->quantidadeEspinhos - 1];
    }
    
    editor->quantidadeEspinhos--;
    return true;
}

int EncontrarEspinhoProximo(EditorFase *editor, Vector2 posicao, float distancia)
{
    if (editor == NULL || editor->quantidadeEspinhos == 0)
    {
        return -1;
    }
    
    int indiceProximo = -1;
    float distanciaMinima = distancia;
    
    for (int i = 0; i < editor->quantidadeEspinhos; i++)
    {
        float distanciaX = fabsf(editor->espinhos[i].posicaoX - posicao.x);
        float distanciaY = fabsf(EDITOR_ALTURA_CHAO - posicao.y);
        float distanciaTotal = sqrtf(distanciaX * distanciaX + distanciaY * distanciaY);
        
        if (distanciaTotal < distanciaMinima)
        {
            distanciaMinima = distanciaTotal;
            indiceProximo = i;
        }
    }
    
    return indiceProximo;
}

void AtualizarEditor(EditorFase *editor, int alturaTela)
{
    (void)alturaTela; // Parametro nao usado
    
    if (editor == NULL)
    {
        return;
    }
    
    Vector2 mousePos = GetMousePosition();
    editor->mouseUltimo = mousePos;
    
    // Clique esquerdo = adicionar espinho.
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        AdicionarEspinhoEditor(editor, mousePos);
    }
    
    // Clique direito = remover espinho.
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        RemoverEspinhoEditor(editor, mousePos);
    }
    
    // Scroll horizontal com setas ou WASD.
    float velocidadeScroll = 300.0f * GetFrameTime();
    
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        editor->cameraX += velocidadeScroll;
    }
    
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        editor->cameraX -= velocidadeScroll;
    }
    
    // Limita a camera para nao ir muito para tras.
    if (editor->cameraX < 0.0f)
    {
        editor->cameraX = 0.0f;
    }
    
    // Mostrar preview se mouse está na area valida (considerando scroll).
    float mousePosXMundo = mousePos.x + editor->cameraX;
    editor->mostrandoPreview = (mousePos.y >= EDITOR_ALTURA_CHAO - 200.0f && 
                                 mousePos.y <= EDITOR_ALTURA_CHAO &&
                                 mousePosXMundo >= 0.0f && 
                                 mousePosXMundo <= (float)GetScreenWidth() + editor->cameraX);
}

void DesenharEditor(EditorFase *editor, int larguraTela, int alturaTela)
{
    (void)alturaTela; // Parametro nao usado
    
    if (editor == NULL)
    {
        return;
    }
    
    // Fundo com grid.
    ClearBackground((Color){ 2, 5, 16, 255 });
    
    // Desenha linhas de grade horizontal para referencia.
    for (int y = (int)EDITOR_ALTURA_CHAO - 200; y < (int)EDITOR_ALTURA_CHAO; y += 50)
    {
        DrawLineEx((Vector2){ 0, (float)y }, (Vector2){ (float)larguraTela, (float)y }, 
                   1.0f, Fade((Color){ 52, 182, 255, 255 }, 0.2f));
    }
    
    // Desenha linhas de grade vertical para referencia.
    for (int x = 0; x < larguraTela; x += 100)
    {
        DrawLineEx((Vector2){ (float)x, EDITOR_ALTURA_CHAO - 200.0f }, 
                   (Vector2){ (float)x, EDITOR_ALTURA_CHAO }, 
                   1.0f, Fade((Color){ 52, 182, 255, 255 }, 0.2f));
    }
    
    // Desenha a linha do chao.
    DrawLineEx((Vector2){ 0, EDITOR_ALTURA_CHAO }, (Vector2){ (float)larguraTela, EDITOR_ALTURA_CHAO }, 
               4.0f, (Color){ 52, 182, 255, 255 });
    
    // Desenha os espinhos existentes.
    for (int i = 0; i < editor->quantidadeEspinhos; i++)
    {
        // Ajusta a posicao pelo scroll da camera.
        float posX = editor->espinhos[i].posicaoX - editor->cameraX;
        
        // Desenha apenas se está visivel na tela.
        if (posX >= -50.0f && posX <= (float)larguraTela + 50.0f)
        {
            Vector2 centro = { posX, EDITOR_ALTURA_CHAO };
            DrawCircleV(centro, 12.0f, EDITOR_COR_ESPINHO_NORMAL);
            DrawCircleLines((int)centro.x, (int)centro.y, 12.0f, EDITOR_COR_ESPINHO_NORMAL);
            
            // Desenha um triangulo representando o espinho.
            float altura = 30.0f + editor->espinhos[i].variacaoAltura;
            DrawTriangle(
                (Vector2){ centro.x - 10.0f, centro.y },
                (Vector2){ centro.x, centro.y - altura },
                (Vector2){ centro.x + 10.0f, centro.y },
                Fade(EDITOR_COR_ESPINHO_NORMAL, 0.6f)
            );
        }
    }
    
    // Desenha preview onde o mouse esta.
    if (editor->mostrandoPreview)
    {
        Vector2 centro = editor->mouseUltimo;
        DrawCircleV(centro, 12.0f, EDITOR_COR_PREVIEW);
        
        float altura = 30.0f;
        DrawTriangle(
            (Vector2){ centro.x - 10.0f, centro.y },
            (Vector2){ centro.x, centro.y - altura },
            (Vector2){ centro.x + 10.0f, centro.y },
            EDITOR_COR_PREVIEW
        );
    }
    
    // Interface fixa.
    DrawRectangle(0, 0, larguraTela, 80, Fade((Color){ 10, 32, 110, 255 }, 0.8f));
    
    DrawText("EDITOR DE FASES", 20, 10, 24, (Color){ 52, 182, 255, 255 });
    DrawText(TextFormat("Espinhos: %d / %d", editor->quantidadeEspinhos, MAX_ESPINHOS_FASE), 
             20, 40, 16, (Color){ 190, 240, 255, 255 });
    
    DrawText("[CLIQUE ESQ] Colocar | [CLIQUE DIR] Remover", 300, 15, 16, (Color){ 190, 240, 255, 255 });
    DrawText("[S] Salvar | [L] Carregar | [C] Limpar | [P] Testar | [ESC] Voltar", 300, 35, 16, (Color){ 190, 240, 255, 255 });
    DrawText("[SETAS/WASD] Scroll", 300, 55, 14, Fade((Color){ 190, 240, 255, 255 }, 0.7f));
    
    // Mostra quantidade maxima se estiver perto do limite.
    if (editor->quantidadeEspinhos >= MAX_ESPINHOS_FASE - 10)
    {
        DrawText("LIMITE DE ESPINHOS PROXIMO!", larguraTela - 300, 40, 16, (Color){ 255, 90, 110, 255 });
    }
}

bool SalvarFaseEditor(EditorFase *editor, const char *caminhoArquivo)
{
    if (editor == NULL || caminhoArquivo == NULL)
    {
        return false;
    }

    char caminhoResolvido[EDITOR_TAMANHO_MAX_CAMINHO];

    if (!ResolverCaminhoFase(caminhoArquivo, caminhoResolvido, sizeof(caminhoResolvido), true))
    {
        return false;
    }

    if (!GarantirDiretorioArquivo(caminhoResolvido))
    {
        return false;
    }

    FILE *arquivo = fopen(caminhoResolvido, "wb");
    
    if (arquivo == NULL)
    {
        return false;
    }

    bool salvouComSucesso = true;
    char nomeFase[64] = { 0 };
    CopiarTextoSeguro(nomeFase, sizeof(nomeFase), editor->nomeFase);

    // Salva quantidade de espinhos.
    if (fwrite(&editor->quantidadeEspinhos, sizeof(int), 1, arquivo) != 1)
    {
        salvouComSucesso = false;
    }

    // Salva nome da fase.
    if (salvouComSucesso && fwrite(nomeFase, sizeof(char), sizeof(nomeFase), arquivo) != sizeof(nomeFase))
    {
        salvouComSucesso = false;
    }

    // Salva todos os espinhos.
    if (salvouComSucesso &&
        editor->quantidadeEspinhos > 0 &&
        fwrite(editor->espinhos, sizeof(DadosEspinho), (size_t)editor->quantidadeEspinhos, arquivo) != (size_t)editor->quantidadeEspinhos)
    {
        salvouComSucesso = false;
    }

    fclose(arquivo);

    if (!salvouComSucesso)
    {
        remove(caminhoResolvido);
    }

    return salvouComSucesso;
}

bool CarregarFaseEditor(EditorFase *editor, const char *caminhoArquivo)
{
    if (editor == NULL || caminhoArquivo == NULL)
    {
        return false;
    }

    char caminhoResolvido[EDITOR_TAMANHO_MAX_CAMINHO];

    if (!ResolverCaminhoFase(caminhoArquivo, caminhoResolvido, sizeof(caminhoResolvido), false))
    {
        return false;
    }

    FILE *arquivo = fopen(caminhoResolvido, "rb");
    
    if (arquivo == NULL)
    {
        return false;
    }

    int quantidadeEspinhos = 0;
    char nomeFase[64] = { 0 };
    DadosEspinho espinhosLidos[MAX_ESPINHOS_FASE] = { 0 };

    // Carrega quantidade de espinhos.
    if (fread(&quantidadeEspinhos, sizeof(int), 1, arquivo) != 1)
    {
        fclose(arquivo);
        return false;
    }

    if (quantidadeEspinhos < 0 || quantidadeEspinhos > MAX_ESPINHOS_FASE)
    {
        fclose(arquivo);
        return false;
    }

    // Carrega nome da fase.
    if (fread(nomeFase, sizeof(char), sizeof(nomeFase), arquivo) != sizeof(nomeFase))
    {
        fclose(arquivo);
        return false;
    }

    // Carrega espinhos.
    if (quantidadeEspinhos > 0 &&
        fread(espinhosLidos, sizeof(DadosEspinho), (size_t)quantidadeEspinhos, arquivo) != (size_t)quantidadeEspinhos)
    {
        fclose(arquivo);
        return false;
    }

    fclose(arquivo);

    // So aplica o carregamento se a leitura terminou sem erros.
    LimparEspinhosEditor(editor);
    editor->quantidadeEspinhos = quantidadeEspinhos;
    memcpy(editor->espinhos, espinhosLidos, sizeof(DadosEspinho) * (size_t)quantidadeEspinhos);
    memcpy(editor->nomeFase, nomeFase, sizeof(editor->nomeFase));
    editor->nomeFase[sizeof(editor->nomeFase) - 1] = '\0';

    return true;
}

// Verifica colisao do jogador com os espinhos da fase editada.
bool VerificarColisaoComEspinhos(Rectangle limitesJogador, DadosEspinho *espinhos, int quantidadeEspinhos, float chaoY)
{
    if (espinhos == NULL || quantidadeEspinhos == 0)
    {
        return false;
    }

    // Encolhe a caixa do personagem para uma colisao mais justa.
    Rectangle caixaColisaoJogador = {
        limitesJogador.x + 10.0f,
        limitesJogador.y + 10.0f,
        limitesJogador.width - 20.0f,
        limitesJogador.height - 12.0f
    };

    // Testa colisao com cada espinho da fase.
    for (int i = 0; i < quantidadeEspinhos; i++)
    {
        float largura = 34.0f;
        float altura = 42.0f + espinhos[i].variacaoAltura;
        float baseX = espinhos[i].posicaoX;

        Rectangle caixaEspinho = {
            baseX + largura * 0.18f,
            chaoY - altura + 6.0f,
            largura * 0.64f,
            altura - 6.0f
        };

        if (CheckCollisionRecs(caixaColisaoJogador, caixaEspinho))
        {
            return true;
        }
    }

    return false;
}

// Desenha os espinhos da fase editada.
static void DesenharEspinhosFase(DadosEspinho *espinhos, int quantidadeEspinhos, float chaoY, Camera2D camera)
{
    (void)camera;
    
    if (espinhos == NULL || quantidadeEspinhos == 0)
    {
        return;
    }

    EstiloCena estilo = ObterEstiloCena();

    for (int i = 0; i < quantidadeEspinhos; i++)
    {
        float largura = 34.0f;
        float altura = 42.0f + espinhos[i].variacaoAltura;
        float baseX = espinhos[i].posicaoX;

        Vector2 pontoEsquerdo;
        pontoEsquerdo.x = baseX;
        pontoEsquerdo.y = chaoY;

        Vector2 pontoTopo;
        pontoTopo.x = baseX + largura * 0.5f;
        pontoTopo.y = chaoY - altura;

        Vector2 pontoDireito;
        pontoDireito.x = baseX + largura;
        pontoDireito.y = chaoY;

        DrawTriangle(pontoEsquerdo, pontoTopo, pontoDireito, Fade(estilo.azulNeon, 0.18f));
        DrawTriangleLines(pontoEsquerdo, pontoTopo, pontoDireito, Fade(estilo.azulNeon, 0.92f));
        DrawLineEx(pontoEsquerdo, pontoTopo, 2.0f, Fade((Color){ 190, 240, 255, 255 }, 0.90f));
        DrawLineEx(pontoTopo, pontoDireito, 2.0f, Fade((Color){ 190, 240, 255, 255 }, 0.90f));
    }
}

// Funcao para testar a fase criada no editor.
bool TestarFaseEditor(EditorFase *editor, int larguraTela, int alturaTela)
{
    if (editor == NULL || editor->quantidadeEspinhos == 0)
    {
        return true;
    }

    const float alturaChao = 140.0f;
    const float chaoY = alturaTela - alturaChao;
    const float larguraJogador = 46.0f;
    const float alturaJogador = 46.0f;
    const int larguraFrameSprite = 96;
    const int alturaFrameSprite = 128;
    const int quantidadeFramesSprite = 4;
    const float velocidadeJogador = 600.0f;
    const float gravidade = 1800.0f;
    const float forcaPulo = -700.0f;
    const float larguraBlocoChao = 160.0f;
    const float inicioJogadorX = 120.0f;
    Rectangle portaSaida = CriarPortaSaidaFase(editor->espinhos, editor->quantidadeEspinhos, chaoY);
    float limiteDireitoUltimoEspinho = ObterLimiteDireitoUltimoEspinho(editor->espinhos, editor->quantidadeEspinhos);

    EstiloCena estilo = ObterEstiloCena();

    ParticulaPoeira particulasPoeira[MAX_PARTICULAS_POEIRA] = { 0 };
    float temporizadorPoeiraCorrida = 0.0f;
    int contadorColisoes = 0;
    float temporizadorFlashDano = 0.0f;
    float temporizadorProtecaoRespawn = 0.0f;
    bool portaLiberada = false;
    bool faseConcluida = false;
    float temporizadorConclusaoFase = 0.0f;
    float progressoFase = 0.0f;
    float tempoAnimacaoPorta = 0.0f;
    bool continuarEditando = true;

    Jogador jogador = CriarJogador(
        inicioJogadorX,
        chaoY,
        larguraJogador,
        alturaJogador,
        larguraFrameSprite,
        alturaFrameSprite,
        quantidadeFramesSprite
    );

    Camera2D camera = { 0 };
    camera.offset = (Vector2){ larguraTela * 0.30f, alturaTela * 0.55f };
    camera.target = (Vector2){ jogador.limites.x + jogador.limites.width / 2.0f, jogador.limites.y + jogador.limites.height / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 0.86f;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }

        float tempoFrame = GetFrameTime();
        tempoAnimacaoPorta += tempoFrame;

        temporizadorFlashDano = fmaxf(0.0f, temporizadorFlashDano - tempoFrame);
        temporizadorProtecaoRespawn = fmaxf(0.0f, temporizadorProtecaoRespawn - tempoFrame);

        if (faseConcluida)
        {
            temporizadorConclusaoFase = fmaxf(0.0f, temporizadorConclusaoFase - tempoFrame);
            AtualizarJogadorEntrandoNaPorta(&jogador, portaSaida, tempoFrame);
            AtualizarParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA, tempoFrame);
            progressoFase = 1.0f;

            if (temporizadorConclusaoFase <= 0.0f)
            {
                break;
            }
        }
        else
        {
            AtualizarJogador(&jogador, tempoFrame, velocidadeJogador, gravidade, forcaPulo, chaoY);

            if (jogador.estaNoChao)
            {
                CriarPoeiraCorrida(particulasPoeira, MAX_PARTICULAS_POEIRA, jogador.limites, tempoFrame, &temporizadorPoeiraCorrida);
            }
            else
            {
                temporizadorPoeiraCorrida = 0.0f;
            }

            if (JogadorAcabouDePousar(&jogador))
            {
                CriarPoeiraPouso(particulasPoeira, MAX_PARTICULAS_POEIRA, jogador.limites);
            }

            if (temporizadorProtecaoRespawn <= 0.0f &&
                VerificarColisaoComEspinhos(jogador.limites, editor->espinhos, editor->quantidadeEspinhos, chaoY))
            {
                contadorColisoes++;
                temporizadorFlashDano = 0.65f;
                temporizadorProtecaoRespawn = 0.80f;
                LimparParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA);
                temporizadorPoeiraCorrida = 0.0f;
                ReiniciarJogador(&jogador, inicioJogadorX, chaoY);
            }

            portaLiberada = (jogador.limites.x + jogador.limites.width) >= (limiteDireitoUltimoEspinho + 12.0f);
            progressoFase = CalcularProgressoFase(
                jogador.limites.x + jogador.limites.width * 0.5f,
                inicioJogadorX,
                portaSaida
            );

            if (portaLiberada && VerificarJogadorEntrouNaPorta(jogador.limites, portaSaida))
            {
                faseConcluida = true;
                temporizadorConclusaoFase = 1.05f;
                progressoFase = 1.0f;
                LimparParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA);
                temporizadorPoeiraCorrida = 0.0f;
            }

            AtualizarParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA, tempoFrame);
        }

        camera.target = (Vector2){ jogador.limites.x + jogador.limites.width / 2.0f, alturaTela / 2.0f };

        BeginDrawing();
        ClearBackground(estilo.corFundo);

        BeginMode2D(camera);

        DesenharFundoEstiloLogo(camera.target.x - camera.offset.x, alturaTela, chaoY);
        DesenharChaoMundo(camera.target.x, chaoY, alturaChao, larguraBlocoChao, estilo);
        DesenharEspinhosFase(editor->espinhos, editor->quantidadeEspinhos, chaoY, camera);
        DesenharParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA);
        DesenharJogador(&jogador, larguraFrameSprite, alturaFrameSprite);
        DesenharPortaSaida(portaSaida, estilo, tempoAnimacaoPorta, portaLiberada || faseConcluida);

        EndMode2D();

        DrawRectangle(0, 0, larguraTela, 84, Fade((Color){ 10, 32, 110, 255 }, 0.82f));
        DrawText("TESTANDO FASE DO EDITOR", 20, 15, 20, (Color){ 52, 182, 255, 255 });
        DrawText(
            TextFormat("Colisoes: %d | Progresso: %03i%% | [ESC] Volta ao Editor", contadorColisoes, (int)roundf(progressoFase * 100.0f)),
            20,
            40,
            16,
            (Color){ 190, 240, 255, 255 }
        );
        DrawText(
            faseConcluida ? "Fase concluida" : (portaLiberada ? "Saida liberada" : "Passe pelos obstaculos"),
            20,
            60,
            16,
            faseConcluida ? (Color){ 120, 235, 255, 255 } :
                (portaLiberada ? (Color){ 255, 220, 110, 255 } : (Color){ 190, 240, 255, 255 })
        );

        if (temporizadorFlashDano > 0.0f)
        {
            Color corFlash = Fade((Color){ 255, 90, 110, 255 }, temporizadorFlashDano * 2.0f);
            DrawText("COLIDIU!", larguraTela / 2 - 50, alturaTela / 2, 32, corFlash);
        }

        if (faseConcluida)
        {
            const char *titulo = "FASE CONCLUIDA";
            const char *subtitulo = "RETORNANDO AO EDITOR";
            int larguraTitulo = MeasureText(titulo, 38);
            int larguraSubtitulo = MeasureText(subtitulo, 18);

            DrawRectangleRounded(
                (Rectangle){ larguraTela * 0.5f - 220.0f, alturaTela * 0.5f - 72.0f, 440.0f, 118.0f },
                0.12f,
                10,
                Fade((Color){ 4, 8, 22, 255 }, 0.82f)
            );
            DrawText(titulo, larguraTela / 2 - larguraTitulo / 2 + 4, alturaTela / 2 - 22, 38, Fade(BLACK, 0.50f));
            DrawText(titulo, larguraTela / 2 - larguraTitulo / 2, alturaTela / 2 - 26, 38, Fade((Color){ 120, 235, 255, 255 }, 0.98f));
            DrawText(subtitulo, larguraTela / 2 - larguraSubtitulo / 2 + 3, alturaTela / 2 + 26, 18, Fade(BLACK, 0.45f));
            DrawText(subtitulo, larguraTela / 2 - larguraSubtitulo / 2, alturaTela / 2 + 22, 18, Fade(estilo.azulNeon, 0.92f));
        }

        EndDrawing();
    }

    DestruirJogador(&jogador);
    return continuarEditando;
}
