#ifndef EFEITOS_H
#define EFEITOS_H

#include "game_types.h"

// Cria pequenas particulas continuas de corrida atras do personagem.
void CriarPoeiraCorrida(ParticulaPoeira particulas[], int quantidadeParticulas, Rectangle limitesJogador, float tempoFrame, float *temporizadorCriacao);

// Cria um efeito mais forte quando o personagem toca o chao.
void CriarPoeiraPouso(ParticulaPoeira particulas[], int quantidadeParticulas, Rectangle limitesJogador);

// Atualiza o movimento e o tempo de vida das particulas.
void AtualizarParticulasPoeira(ParticulaPoeira particulas[], int quantidadeParticulas, float tempoFrame);

// Desenha todas as particulas ativas com transparencia baseada no tempo de vida.
void DesenharParticulasPoeira(const ParticulaPoeira particulas[], int quantidadeParticulas);

// Limpa todas as particulas para reiniciar a cena visualmente.
void LimparParticulasPoeira(ParticulaPoeira particulas[], int quantidadeParticulas);

#endif
