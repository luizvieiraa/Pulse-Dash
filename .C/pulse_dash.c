#include "../.H/effects.h"
#include "../.H/game_types.h"
#include "../.H/player.h"
#include "../.H/scene.h"
#include "../.H/editor.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define CAMINHO_PROGRESSO_FASES "fases/progresso.dat"

// Funcao que desenha o menu principal com opcoes de jogar ou editar.
void DesenharMenu(int larguraTela, int alturaTela, EstiloCena estilo, int opcaoSelecionada)
{
    ClearBackground(estilo.corFundo);

    // Desenha o titulo do jogo.
    const char *titulo = "PULSE DASH";
    int larguraTitulo = MeasureText(titulo, 64);
    DrawText(titulo, larguraTela / 2 - larguraTitulo / 2, 100, 64, estilo.azulNeon);

    // Desenha as opcoes do menu.
    const char *opcoes[] = { "JOGAR", "EDITOR DE FASES", "SAIR" };
    int quantidadeOpcoes = 3;
    int espacoVertical = 120;
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
    DrawText("[SETA CIMA/BAIXO] Selecionar | [ENTER] Confirmar", 
             50, alturaTela - 50, 16, Fade(estilo.azulNeon, 0.7f));
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
            return -1; // Sair
        }
    }

    return MODO_MENU;
}

static void DesenharMenuSelecaoFase(int larguraTela, int alturaTela, int opcaoSelecionada, const float progressoFases[TOTAL_FASES])
{
    EstiloCena estiloBase = ObterEstiloCena();

    ClearBackground(estiloBase.corFundo);

    const char *titulo = "SELECIONAR FASE";
    int larguraTitulo = MeasureText(titulo, 52);
    DrawText(titulo, larguraTela / 2 - larguraTitulo / 2, 80, 52, estiloBase.azulNeon);

    int quantidadeOpcoes = TOTAL_FASES + 1;
    int larguraCartao = 640;
    int alturaCartao = 84;
    int posY = 190;

    for (int i = 0; i < quantidadeOpcoes; i++)
    {
        bool selecionada = (i == opcaoSelecionada);
        Rectangle areaOpcao = {
            larguraTela * 0.5f - larguraCartao * 0.5f,
            (float)posY,
            (float)larguraCartao,
            (float)alturaCartao
        };

        if (i < TOTAL_FASES)
        {
            int numeroFase = i + 1;
            EstiloCena estiloFase = ObterEstiloCenaFase(numeroFase);
            float progresso = fmaxf(0.0f, fminf(progressoFases[i], 1.0f));
            int percentual = (int)roundf(progresso * 100.0f);

            DrawRectangleRounded(areaOpcao, 0.08f, 8, Fade(estiloFase.azulProfundo, selecionada ? 0.86f : 0.54f));
            DrawRectangleRoundedLines(areaOpcao, 0.08f, 8, selecionada ? Fade(WHITE, 0.90f) : Fade(estiloFase.azulNeon, 0.62f));
            DrawText(TextFormat("FASE %d", numeroFase), (int)areaOpcao.x + 24, posY + 18, 28, selecionada ? WHITE : estiloFase.azulNeon);
            DrawText(TextFormat("PROGRESSO: %03d%%", percentual), (int)areaOpcao.x + 420, posY + 20, 22, Fade((Color){ 190, 240, 255, 255 }, 0.95f));

            DrawRectangleRounded(
                (Rectangle){ areaOpcao.x + 24.0f, areaOpcao.y + 58.0f, 592.0f, 10.0f },
                0.45f,
                8,
                Fade(BLACK, 0.38f)
            );
            DrawRectangleRounded(
                (Rectangle){ areaOpcao.x + 24.0f, areaOpcao.y + 58.0f, 592.0f * progresso, 10.0f },
                0.45f,
                8,
                Fade(estiloFase.azulNeon, 0.92f)
            );
        }
        else
        {
            DrawRectangleRounded(areaOpcao, 0.08f, 8, Fade(estiloBase.azulProfundo, selecionada ? 0.82f : 0.44f));
            DrawRectangleRoundedLines(areaOpcao, 0.08f, 8, selecionada ? Fade(WHITE, 0.90f) : Fade(estiloBase.azulNeon, 0.55f));

            const char *textoVoltar = "VOLTAR";
            int larguraTexto = MeasureText(textoVoltar, 28);
            DrawText(textoVoltar, larguraTela / 2 - larguraTexto / 2, posY + 28, 28, selecionada ? WHITE : estiloBase.azulNeon);
        }

        posY += alturaCartao + 18;
    }

    const char *instrucoes = "[SETA CIMA/BAIXO] Selecionar | [ENTER] Confirmar | [ESC] Voltar";
    int larguraInstrucoes = MeasureText(instrucoes, 16);
    DrawText(instrucoes, larguraTela / 2 - larguraInstrucoes / 2, alturaTela - 48, 16, Fade(estiloBase.azulNeon, 0.75f));
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
        return -1;
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (*opcaoSelecionada < TOTAL_FASES)
        {
            return *opcaoSelecionada + 1;
        }

        return -1;
    }

    return 0;
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

    if (numeroFase == 1 && CarregarFaseEditor(editor, CAMINHO_FASE_CUSTOMIZADA_LEGADO))
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
    ZerarProgressosFases(progressoFases);

    FILE *arquivo = fopen(CAMINHO_PROGRESSO_FASES, "rb");

    if (arquivo == NULL)
    {
        return;
    }

    fread(progressoFases, sizeof(float), TOTAL_FASES, arquivo);
    fclose(arquivo);

    for (int i = 0; i < TOTAL_FASES; i++)
    {
        progressoFases[i] = fmaxf(0.0f, fminf(progressoFases[i], 1.0f));
    }
}

static bool SalvarProgressosFases(const float progressoFases[TOTAL_FASES])
{
    FILE *arquivo = fopen(CAMINHO_PROGRESSO_FASES, "wb");

    if (arquivo == NULL)
    {
        return false;
    }

    bool salvou = fwrite(progressoFases, sizeof(float), TOTAL_FASES, arquivo) == TOTAL_FASES;
    fclose(arquivo);

    return salvou;
}

static void DesenharEspinhosCustomizados(const DadosEspinho *espinhos, int quantidadeEspinhos, float chaoY, EstiloCena estilo)
{
    if (espinhos == NULL || quantidadeEspinhos <= 0)
    {
        return;
    }

    for (int i = 0; i < quantidadeEspinhos; i++)
    {
        float largura = 34.0f;
        float altura = 42.0f + espinhos[i].variacaoAltura;
        float baseX = espinhos[i].posicaoX;

        Vector2 pontoEsquerdo = { baseX, chaoY };
        Vector2 pontoTopo = { baseX + largura * 0.5f, chaoY - altura };
        Vector2 pontoDireito = { baseX + largura, chaoY };

        DrawTriangle(pontoEsquerdo, pontoTopo, pontoDireito, Fade(estilo.azulNeon, 0.18f));
        DrawTriangleLines(pontoEsquerdo, pontoTopo, pontoDireito, Fade(estilo.azulNeon, 0.92f));
        DrawLineEx(pontoEsquerdo, pontoTopo, 2.0f, Fade((Color){ 190, 240, 255, 255 }, 0.90f));
        DrawLineEx(pontoTopo, pontoDireito, 2.0f, Fade((Color){ 190, 240, 255, 255 }, 0.90f));
    }
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
                     float chaoY, float larguraJogador, float alturaJogador, int larguraFrameSprite, 
                     int alturaFrameSprite, int quantidadeFramesSprite, float velocidadeJogador, 
                     float gravidade, float forcaPulo, float larguraBlocoChao, float inicioJogadorX,
                     DadosEspinho *espinhosCustomizados, int quantidadeEspinhosCustomizados,
                     float *progressoMaximoFase)
{
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

    // Cria o personagem principal com sprite, fisica e animacao.
    Jogador jogador = CriarJogador(
        inicioJogadorX,
        chaoY,
        larguraJogador,
        alturaJogador,
        larguraFrameSprite,
        alturaFrameSprite,
        quantidadeFramesSprite
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
        DesenharFundoEstiloLogo(camera.target.x - camera.offset.x, alturaTela, chaoY, estilo);

        // Desenha o piso completo da fase com blocos, linhas e espinhos decorativos.
        DesenharChaoMundo(camera.target.x, chaoY, alturaChao, larguraBlocoChao, estilo);

        // Desenha os espinhos customizados se existirem.
        DesenharEspinhosCustomizados(espinhosCustomizados, quantidadeEspinhosCustomizados, chaoY, estilo);

        // Desenha as particulas antes do personagem para a energia ficar parcialmente atras dele.
        DesenharParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA);

        // Desenha o personagem com o frame correto da animacao.
        DesenharJogador(&jogador, larguraFrameSprite, alturaFrameSprite);

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

    // Libera os recursos do personagem antes de encerrar o jogo.
    DestruirJogador(&jogador);

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
        AtualizarEditor(editor, alturaTela);

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
        DesenharEditor(editor, larguraTela, alturaTela);
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

    // Define o tamanho de cada frame do sprite e quantos frames existem.
    const int larguraFrameSprite = 96;
    const int alturaFrameSprite = 128;
    const int quantidadeFramesSprite = 4;

    // Define a velocidade horizontal do boneco em pixels por segundo.
    const float velocidadeJogador = 600.0f;

    // Define a forca da gravidade e a velocidade inicial do pulo.
    const float gravidade = 1800.0f;
    const float forcaPulo = -700.0f;

    // Define a largura de cada bloco visual do chao.
    const float larguraBlocoChao = 160.0f;

    // Define a posicao inicial usada quando o jogador precisa voltar apos uma colisao.
    const float inicioJogadorX = 120.0f;

    // Carrega a paleta principal do cenario para unificar o visual do jogo.
    EstiloCena estilo = ObterEstiloCena();

    // Inicializa a janela principal do projeto.
    InitWindow(larguraTela, alturaTela, "Pulse Dash");

    // Mantem a atualizacao e o desenho em 60 quadros por segundo.
    SetTargetFPS(60);

    // Loop principal: menu, jogo e editor.
    ModoAplicacao modoAtual = MODO_MENU;
    int opcaoMenuSelecionada = 0;
    int opcaoFaseSelecionada = 0;
    int faseSelecionadaParaJogar = 1;
    float progressoFases[TOTAL_FASES] = { 0 };

    CarregarProgressosFases(progressoFases);

    while (!WindowShouldClose() && modoAtual != (ModoAplicacao)(-1))
    {
        if (modoAtual == MODO_MENU)
        {
            BeginDrawing();
            DesenharMenu(larguraTela, alturaTela, estilo, opcaoMenuSelecionada);
            EndDrawing();

            ModoAplicacao novoModo = AtualizarMenu(&opcaoMenuSelecionada);
            
            if (novoModo != MODO_MENU)
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
            else if (resultadoSelecao < 0)
            {
                modoAtual = MODO_MENU;
                opcaoMenuSelecionada = 0;
            }
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
                                                    larguraJogador, alturaJogador, larguraFrameSprite, alturaFrameSprite,
                                                    quantidadeFramesSprite, velocidadeJogador, gravidade, forcaPulo,
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
