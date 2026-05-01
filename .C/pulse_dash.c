#include "../.H/effects.h"
#include "../.H/game_types.h"
#include "../.H/player.h"
#include "../.H/scene.h"
#include "../.H/editor.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

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
            return MODO_JOGO;
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

// Versao do LoopJogo que suporta fases customizadas.
void LoopJogoComFase(int larguraTela, int alturaTela, EstiloCena estilo, float alturaChao, 
                     float chaoY, float larguraJogador, float alturaJogador, int larguraFrameSprite, 
                     int alturaFrameSprite, int quantidadeFramesSprite, float velocidadeJogador, 
                     float gravidade, float forcaPulo, float larguraBlocoChao, float inicioJogadorX,
                     DadosEspinho *espinhosCustomizados, int quantidadeEspinhosCustomizados)
{
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

    // Cria o personagem principal com sprite, fisica e animacao.
    Jogador jogador = CriarJogador(
        120.0f,
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
        // Trata a pausa com a tecla P (apenas quando nao estiver em pausa).
        if (IsKeyPressed(KEY_P))
        {
            emPausa = true;
            opcaoPausa = 0; // Reseta a selecao do menu de pausa.
        }

        // Calcula o tempo do frame atual para o movimento ficar suave.
        float tempoFrame = GetFrameTime();

        // Atualiza o tempo restante do flash de dano e da protecao de respawn.
        temporizadorFlashDano = fmaxf(0.0f, temporizadorFlashDano - tempoFrame);
        temporizadorProtecaoRespawn = fmaxf(0.0f, temporizadorProtecaoRespawn - tempoFrame);

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

        // Atualiza a simulacao das particulas depois da logica principal do frame.
        AtualizarParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA, tempoFrame);

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
        DesenharFundoEstiloLogo(camera.target.x - camera.offset.x, alturaTela, chaoY);

        // Desenha o piso completo da fase com blocos, linhas e espinhos decorativos.
        DesenharChaoMundo(camera.target.x, chaoY, alturaChao, larguraBlocoChao, estilo);

        // Desenha os espinhos customizados se existirem.
        if (espinhosCustomizados != NULL && quantidadeEspinhosCustomizados > 0)
        {
            for (int i = 0; i < quantidadeEspinhosCustomizados; i++)
            {
                float largura = 34.0f;
                float altura = 42.0f + espinhosCustomizados[i].variacaoAltura;
                float baseX = espinhosCustomizados[i].posicaoX;

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

        // Desenha as particulas antes do personagem para a energia ficar parcialmente atras dele.
        DesenharParticulasPoeira(particulasPoeira, MAX_PARTICULAS_POEIRA);

        // Desenha o personagem com o frame correto da animacao.
        DesenharJogador(&jogador, larguraFrameSprite, alturaFrameSprite);

        // Sai do modo de camera para desenhar elementos fixos da interface.
        EndMode2D();

        // Desenha a interface fixa com moldura e textos.
        DesenharHudCena(larguraTela, alturaTela, estilo, contadorColisoes, temporizadorFlashDano);

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
}

// Funcao principal do editor de fases.
void LoopEditor(int larguraTela, int alturaTela)
{
    EditorFase *editor = CriarEditorFase();
    
    if (editor == NULL)
    {
        return;
    }

    // Loop do editor: executa enquanto o jogador nao fecha a janela ou pressiona ESC.
    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE))
    {
        // Atualiza a logica do editor (entrada do mouse).
        AtualizarEditor(editor, alturaTela);

        // Atalho para salvar.
        if (IsKeyPressed(KEY_S))
        {
            if (SalvarFaseEditor(editor, "fases/fase_customizada.dat"))
            {
                printf("Fase salva em: fases/fase_customizada.dat\n");
            }
        }

        // Atalho para carregar.
        if (IsKeyPressed(KEY_L))
        {
            if (CarregarFaseEditor(editor, "fases/fase_customizada.dat"))
            {
                printf("Fase carregada de: fases/fase_customizada.dat\n");
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
        else if (modoAtual == MODO_JOGO)
        {
            // Carrega a fase customizada salva.
            EditorFase faseCarregada = { 0 };
            
            if (CarregarFaseEditor(&faseCarregada, "fases/fase_customizada.dat") && faseCarregada.quantidadeEspinhos > 0)
            {
                // Toca a fase customizada.
                LoopJogoComFase(larguraTela, alturaTela, estilo, alturaChao, chaoY,
                                larguraJogador, alturaJogador, larguraFrameSprite, alturaFrameSprite,
                                quantidadeFramesSprite, velocidadeJogador, gravidade, forcaPulo,
                                larguraBlocoChao, inicioJogadorX,
                                faseCarregada.espinhos, faseCarregada.quantidadeEspinhos);
            }
            else
            {
                printf("Nenhuma fase customizada encontrada! Crie uma no editor.\n");
            }
            
            modoAtual = MODO_MENU;
            opcaoMenuSelecionada = 0;
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
