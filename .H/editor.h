#ifndef EDITOR_H
#define EDITOR_H

#include "game_types.h"
#include "raylib.h"

// Inicializa um novo editor de fases vazio.
EditorFase *CriarEditorFase(void);

// Libera memoria do editor.
void DestruirEditorFase(EditorFase *editor);

// Limpa todos os espinhos do editor.
void LimparEspinhosEditor(EditorFase *editor);

// Adiciona um espinho na posicao do mouse (clique esquerdo).
void AdicionarEspinhoEditor(EditorFase *editor, Vector2 posicao);

// Remove o espinho mais proximo da posicao (clique direito).
bool RemoverEspinhoEditor(EditorFase *editor, Vector2 posicao);

// Encontra o espinho mais proximo de uma posicao.
int EncontrarEspinhoProximo(EditorFase *editor, Vector2 posicao, float distancia);

// Desenha a interface do editor na tela.
void DesenharEditor(EditorFase *editor, int larguraTela, int alturaTela);

// Atualiza a logica do editor (entrada do usuario).
void AtualizarEditor(EditorFase *editor, int alturaTela);

// Salva a fase em arquivo binario.
bool SalvarFaseEditor(EditorFase *editor, const char *caminhoArquivo);

// Carrega uma fase de um arquivo binario.
bool CarregarFaseEditor(EditorFase *editor, const char *caminhoArquivo);

// Testa a fase editada no modo jogo.
// Retorna true se quer continuar editando, false se quer voltar ao menu.
bool TestarFaseEditor(EditorFase *editor, int larguraTela, int alturaTela);

// Verifica colisao entre o jogador e os espinhos customizados.
bool VerificarColisaoComEspinhos(Rectangle limitesJogador, DadosEspinho *espinhos, int quantidadeEspinhos, float chaoY);

#endif
