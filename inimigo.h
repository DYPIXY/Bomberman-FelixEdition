#pragma once

#include "BomberMan.h"

#define TICKS_UNTIL_NEXT_POS_ENEMY 30

// Sobrecarga simples: cria inimigo numa posição fixa
void criarInimigo(int x, int y) {
    state->enemies.push_back(Enemy(x, y));
    state->screenBuffer[y][x] = WHITE;
}

// Sobrecarga: Spawna 'qtd' inimigos em posições aleatórias livres
void criarInimigo(int mapa[hMax][wMax], int qtd) {
    int inimigosCriados = 0;
    int tentativas = 0;
    const int MAX_TENT = 1000;

    while (inimigosCriados < qtd && tentativas < MAX_TENT) {
        tentativas++;
        int x = rand() % wMax;
        int y = rand() % hMax;

        if (mapa[y][x] != WHITE) continue;

        // Não spawnar perto do P1
        if (abs(x - 1) <= 2 && abs(y - 1) <= 2) continue;

        // Não spawnar perto do P2
        if (state->p2Ativo && abs(x - 1) <= 2 && abs(y - (hMax - 2)) <= 2) continue;

        bool jaTemInimigo = false;
        for (const Enemy& e : state->enemies) {
            if (e.inimigoVivo && e.pos.x == x && e.pos.y == y) {
                jaTemInimigo = true;
                break;
            }
        }

        if (jaTemInimigo) continue;

        criarInimigo(x, y);
        inimigosCriados++;
    }
}

// Sobrecarga: Spawna inimigos marcados com 99 no mapa
void criarInimigo(int mapa[hMax][wMax]) {
    for (int i = 0; i < hMax; i++) {
        for (int j = 0; j < wMax; j++) {
            if (mapa[i][j] == 99) {
                criarInimigo(j, i);
                mapa[i][j] = WHITE;
                state->screenBuffer[i][j] = WHITE;
            }
        }
    }
}

// Spawna boss marcado com 98 no mapa
void spawnarBoss(int mapa[hMax][wMax]) {
    for (int i = 0; i < hMax; i++) {
        for (int j = 0; j < wMax; j++) {
            if (mapa[i][j] == BOSS_ID) {
                Enemy boss(j, i);
                boss.boss = true;
                boss.vida = 3;
                state->enemies.push_back(boss);
                mapa[i][j] = WHITE;
                state->screenBuffer[i][j] = WHITE;
            }
        }
    }
}

// Verifica se uma posição pode ser ocupada pelo inimigo
bool posicaoLivreParaInimigo(int x, int y, const Enemy& inimigoAtual) {
    if (x < 0 || x >= wMax || y < 0 || y >= hMax) return false;

    bool bloqueado =
        state->screenBuffer[y][x] == BLOCO_SOLIDO ||
        state->screenBuffer[y][x] == PAREDE_DESTRUTIVEL ||
        state->screenBuffer[y][x] == BOMBA;

    if (bloqueado) return false;

    for (const Enemy& outro : state->enemies) {
        if (&outro == &inimigoAtual) continue;

        if (outro.inimigoVivo && outro.pos.x == x && outro.pos.y == y) {
            return false;
        }
    }

    return true;
}

// Move inimigo em direção ao jogador mais próximo
void moverEmDirecaoJogador(Enemy& inimigo) {
    int alvoX = state->p1.pos.x;
    int alvoY = state->p1.pos.y;

    // Se tiver P2 ativo, escolhe o jogador mais próximo
    if (state->p2Ativo && state->p2Vivo && state->p2.alive) {
        int distP1 = abs(inimigo.pos.x - state->p1.pos.x) + abs(inimigo.pos.y - state->p1.pos.y);
        int distP2 = abs(inimigo.pos.x - state->p2.pos.x) + abs(inimigo.pos.y - state->p2.pos.y);

        if (!state->p1.alive || distP2 < distP1) {
            alvoX = state->p2.pos.x;
            alvoY = state->p2.pos.y;
        }
    }

    int dx = 0;
    int dy = 0;

    int distanciaX = abs(alvoX - inimigo.pos.x);
    int distanciaY = abs(alvoY - inimigo.pos.y);

    // Tenta andar primeiro no eixo que aproxima mais do jogador
    if (distanciaX >= distanciaY) {
        dx = (alvoX > inimigo.pos.x) ? 1 : -1;
        dy = 0;

        if (alvoX == inimigo.pos.x) {
            dx = 0;
            dy = (alvoY > inimigo.pos.y) ? 1 : -1;
        }
    } else {
        dx = 0;
        dy = (alvoY > inimigo.pos.y) ? 1 : -1;

        if (alvoY == inimigo.pos.y) {
            dx = (alvoX > inimigo.pos.x) ? 1 : -1;
            dy = 0;
        }
    }

    int proxX = inimigo.pos.x + dx;
    int proxY = inimigo.pos.y + dy;

    if (posicaoLivreParaInimigo(proxX, proxY, inimigo)) {
        inimigo.dx = dx;
        inimigo.dy = dy;
        inimigo.passosRestantes = 1;
        return;
    }

    // Se o eixo principal estiver bloqueado, tenta o outro eixo
    int dxAlternativo = 0;
    int dyAlternativo = 0;

    if (dx != 0) {
        dxAlternativo = 0;
        dyAlternativo = (alvoY > inimigo.pos.y) ? 1 : -1;
    } else {
        dxAlternativo = (alvoX > inimigo.pos.x) ? 1 : -1;
        dyAlternativo = 0;
    }

    proxX = inimigo.pos.x + dxAlternativo;
    proxY = inimigo.pos.y + dyAlternativo;

    if (posicaoLivreParaInimigo(proxX, proxY, inimigo)) {
        inimigo.dx = dxAlternativo;
        inimigo.dy = dyAlternativo;
        inimigo.passosRestantes = 1;
        return;
    }

    // Se nenhum caminho aproximando estiver livre, ele não persegue nesse turno
    inimigo.passosRestantes = 0;
}

// Atualiza inimigo (movimento + morte)
void updateInimigo(Enemy& inimigo) {
    if (!inimigo.inimigoVivo) return;

    // Dano por explosão
    if (state->screenBuffer[inimigo.pos.y][inimigo.pos.x] == BOMBA_EXPLOSAO) {
        if (!inimigo.tomouDano) {
            inimigo.tomouDano = true;
            inimigo.vida--;

            if (inimigo.vida <= 0) {
                inimigo.inimigoVivo = false;
                state->hud.pontuacao += inimigo.boss ? 200 : 50;
                return;
            }
        }
    } else {
        inimigo.tomouDano = false;
    }

    inimigo.tempoInimigo++;

    if (inimigo.tempoInimigo < TICKS_UNTIL_NEXT_POS_ENEMY) return;

    inimigo.tempoInimigo = 0;

    bool moveu = false;

    for (int tentativa = 0; tentativa < 8; tentativa++) {
        if (inimigo.passosRestantes <= 0) {
            int chancePerseguir = 0;

            if (state->difficulty == 1) chancePerseguir = 0;   // Fácil: aleatório
            if (state->difficulty == 2) chancePerseguir = 50;  // Médio: 50%
            if (state->difficulty == 3) chancePerseguir = 75;  // Difícil: 75%

            if (chancePerseguir > 0 && (rand() % 100) < chancePerseguir) {
                moverEmDirecaoJogador(inimigo);
            }

            // Se não perseguiu ou não conseguiu perseguir, anda aleatório
            if (inimigo.passosRestantes <= 0) {
                int dir = rand() % 4;

                inimigo.dx = (dir == 2 ? -1 : (dir == 3 ? 1 : 0));
                inimigo.dy = (dir == 0 ? -1 : (dir == 1 ? 1 : 0));
                inimigo.passosRestantes = (rand() % 3) + 1;
            }
        }

        int proxX = inimigo.pos.x + inimigo.dx;
        int proxY = inimigo.pos.y + inimigo.dy;

        if (!posicaoLivreParaInimigo(proxX, proxY, inimigo)) {
            inimigo.passosRestantes = 0;
            continue;
        }

        inimigo.pos.x = proxX;
        inimigo.pos.y = proxY;
        inimigo.passosRestantes--;
        moveu = true;
        break;
    }

    if (!moveu) {
        inimigo.dx = 0;
        inimigo.dy = 0;
        inimigo.passosRestantes = 0;
    }
}

bool todosInimigosMortos() {
    for (const Enemy& e : state->enemies) {
        if (e.inimigoVivo) return false;
    }

    return true;
}