# 🎮 Pulse Dash

Um jogo de plataforma e reflexos rápido, emocionante e desafiador desenvolvido em **C** com **raylib**. Navegue por fases repletas de obstáculos, teste seus reflexos e supere os desafios para chegar ao final!

## 📋 Descrição do Jogo

**Pulse Dash** é um jogo de corrida infinita onde você controla um personagem que se move constantemente para a frente. Seu objetivo é desviar dos espinhos, pular obstáculos e alcançar a porta de saída em cada fase.

### 🎯 Características Principais
- 🏃 Movimento contínuo automático do personagem
- ⌨️ Controle simples com apenas um botão de pulo
- 🎨 Visual estilizado com efeitos visuais
- 📍 Múltiplas fases progressivas
- 🛠️ Editor de fases integrado para criar seus próprios níveis
- 💾 Sistema de progresso para acompanhar seu avanço

---

## 🚀 Como Executar o Jogo

### Pré-requisitos
- **Windows (64-bit)**
- **GCC** ou compilador C compatível
- **raylib** instalada e configurada
- **Make** (opcional, para facilitar a compilação)

### Opção 1: Executar o Arquivo .exe (Se já compilado)
```bash
.C/pulse_dash.exe
```

### Opção 2: Compilar e Executar via Makefile (Linux/macOS)
```bash
make run
```

### Opção 3: Compilar Manualmente no Windows (via VS Code Task)
1. Abra o projeto no VS Code
2. Pressione `Ctrl + Shift + B` para rodar a tarefa de build padrão ("Build Pulse Dash")
3. Após compilar, execute:
```bash
.C\pulse_dash.exe
```

### Opção 4: Compilar Manualmente (Command Line)
```bash
gcc -std=c99 -Wall -Wextra -Wpedantic ^
  -I.H ^
  -I"C:\raylib\w64devkit\x86_64-w64-mingw32\include" ^
  .C\pulse_dash.c -o .C\pulse_dash.exe ^
  -L"C:\raylib\w64devkit\x86_64-w64-mingw32\lib" ^
  -lraylib -lopengl32 -lgdi32 -lwinmm -lm
```

---

## 🎮 Como Jogar

### Menu Principal
Ao iniciar o jogo, você verá o menu principal com as seguintes opções:
- **▶ Jogar** - Começar uma nova partida
- **📋 Fases** - Selecionar uma fase específica
- **⚙️ Configurações** - Editor de fases
- **❌ Sair** - Fechar o jogo

### Controles Básicos
| Ação | Controle |
|------|----------|
| **Pular** | `ESPAÇO` ou `CLICAR DO MOUSE` |
| **Voltar ao Menu** | `ESC` |
| **Sair** | `ALT + F4` |

### Mecânicas do Jogo

#### ⚡ Movimento
- Seu personagem se move **automaticamente** para a frente
- Não é necessário controlar a velocidade horizontal
- Você só precisa gerenciar o **salto** para desviar dos obstáculos

#### 🏃 Pulo
- Pressione **ESPAÇO** para pular
- Você só pode pular quando estiver no chão
- Mantenha **ESPAÇO** pressionado para pulos mais altos
- Trabalhe sua **sincronização** para passar pelos obstáculos

#### 🌹 Obstáculos
- **Espinhos** aparecem em diferentes alturas
- Um toque em qualquer espinho reinicia a fase
- A dificuldade aumenta com cada fase

#### 🚪 Objetivo
- Chegue até a **porta de saída** (porta aberta no final da fase)
- Entre na porta para completar o nível
- Desbloqueie as fases seguintes

### Exemplo de Jogabilidade
1. Comece em uma fase
2. Seu personagem avança automaticamente
3. Você vê um obstáculo (espinhos) se aproximando
4. Pressione ESPAÇO no momento certo para pular
5. Desvie de todos os obstáculos
6. Alcance a porta de saída para vencer

---

## 📊 Fases

O jogo inclui **múltiplas fases** com progressão de dificuldade:
- **Fase 1** - Introdução aos controles
- **Fase 2** - Desafios intermediários
- **Fase 3** - Desafios avançados
- **Fase Customizada** - Crie seus próprios níveis!

### Progresso
Seu progresso é salvo automaticamente em `fases/progresso.dat`. Você pode:
- Voltar a uma fase anterior
- Tentar melhorar seu tempo
- Destrancar novas fases

---

## 🛠️ Modo Editor (Criador de Fases)

Crie seus próprios níveis personalizados!

### Como Acessar
1. No menu principal, selecione **⚙️ Configurações**
2. Escolha **Editor de Fases**

### Controles do Editor
| Ação | Controle |
|------|----------|
| **Adicionar Espinho** | `CLIQUE ESQUERDO` do mouse |
| **Remover Espinho** | `CLIQUE DIREITO` do mouse |
| **Visualizar Fase** | Botão de Preview |
| **Salvar Fase** | Menu do Editor |
| **Mover Câmera** | `SETAS` ou `A`/`D` |

### Dicas para Criar Fases
- Comece com obstáculos simples e aumente a dificuldade gradualmente
- Use variações de altura para desafios mais interessantes
- Deixe espaço suficiente entre os obstáculos para o jogador pular
- Teste sua fase antes de salvar

---

## 📁 Estrutura do Projeto

```
PULSE DASH/
├── .C/                 # Arquivos C fonte
│   ├── pulse_dash.c   # Arquivo principal do jogo
│   ├── player.c       # Lógica do personagem
│   ├── scene.c        # Gerenciamento de cenas
│   ├── effects.c      # Efeitos visuais
│   ├── editor.c       # Editor de fases
│   └── pulse_dash.exe # Executável compilado
├── .H/                 # Arquivos header
│   ├── game_types.h   # Tipos e estruturas
│   ├── player.h       # Funções do jogador
│   ├── scene.h        # Funções de cena
│   ├── effects.h      # Funções de efeitos
│   └── editor.h       # Funções do editor
├── fases/              # Dados das fases
│   ├── fase_1.dat
│   ├── fase_2.dat
│   ├── fase_3.dat
│   ├── fase_customizada.dat
│   └── progresso.dat  # Arquivo de progresso do jogador
├── build/              # Pasta de build (gerada)
├── Makefile            # Script de compilação
└── README.md           # Este arquivo
```

---

## 🔧 Requisitos de Sistema

- **Sistema Operacional**: Windows 10+ (64-bit)
- **Memória RAM**: 512 MB ou mais
- **Resolução de Tela**: 1280 x 720 ou superior (recomendado)
- **Taxa de Quadros**: 60 FPS

---

## 🎨 Tecnologias Utilizadas

- **Linguagem**: C (C99)
- **Biblioteca Gráfica**: [raylib](https://www.raylib.com/) - Uma biblioteca C simples e poderosa para desenvolvimento de jogos
- **Compilador**: GCC (MinGW no Windows)

---

## 💡 Dicas para Ganhar

1. 🎯 **Pratique o Timing** - O segredo é acertar o momento exato do pulo
2. 📺 **Antecipe os Obstáculos** - Observe o padrão de espinhos à frente
3. ⚡ **Reações Rápidas** - Quanto mais você jogar, mais rápidas ficarão suas reações
4. 🔄 **Tente Novamente** - Persistência é a chave para completar os desafios
5. 🛠️ **Crie Desafios** - Use o editor para treinar padrões específicos

---

## 📝 Controles Rápidos

```
ESPAÇO      → Pular
ESC         → Voltar / Menu
MOUSE       → Clique para pular ou interagir
ALT + F4    → Fechar jogo
```

---

## 🎓 Como Começar

1. **Baixe ou clone** o projeto
2. **Compile** usando a tarefa de build ou Makefile
3. **Execute** o arquivo `.exe`
4. **Comece a jogar** no Menu Principal
5. **Divirta-se** e bata seus próprios recordes!

---

## 📞 Suporte

Se encontrar problemas ao compilar ou executar:
- Verifique se raylib está corretamente instalada
- Confirme que as variáveis de ambiente estão configuradas
- Tente recompilando com `make clean` seguido de `make build`

---

**Bom jogo e boa sorte! 🎮✨**
