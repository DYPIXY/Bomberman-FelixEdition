#pragma Once

#include "BomberMan.h"

#define TICKS_UNTIL_NEXT_STATE_BOMB 45


// conta quantas bombas ainda estao ativas
int contarBombasAtivas() {
    int total = 0;

    for (Bomba& bomba : state->bombas) {
        if (bomba.temBomba || bomba.explodindo)
            total++;
    }

    return total;
}


// coloca bomba onde o jogador ta
void colocarBomba() {

    // respeita o limite de bombas do jogador
    if (contarBombasAtivas() >= state->limiteBombas)
        return;

    // nao deixa colocar duas bombas exatamente no mesmo lugar
    for (Bomba& bomba : state->bombas) {
        if ((bomba.temBomba || bomba.explodindo) &&
            bomba.pos.x == state->p1.pos.x &&
            bomba.pos.y == state->p1.pos.y) {
            return;
        }
    }

    Bomba novaBomba;

    novaBomba.temBomba = true;
    novaBomba.explodindo = false;
    novaBomba.pos = state->p1.pos;
    novaBomba.tempoBomba = 0;
    novaBomba.cooldownBomba = 0;

    state->bombas.push_back(novaBomba);

    state->screenBuffer[novaBomba.pos.y][novaBomba.pos.x] = BOMBA;
    state->hud.bombasUsadas++;
}


// cuida da explosao (criar e apagar)
void gerenciarExplosao(Bomba& bomba, int tipoTile) {

    int bx = bomba.pos.x;
    int by = bomba.pos.y;

    // centro
    if (state->screenBuffer[by][bx] != BLOCO_SOLIDO)
        state->screenBuffer[by][bx] = tipoTile;

    // direcoes da explosao
    int direcoes[4][2] = {
        {0, -1}, // cima
        {0, 1},  // baixo
        {-1, 0}, // esquerda
        {1, 0}   // direita
    };

    // percorre todas as direcoes
    for (int d = 0; d < 4; d++) {

        // usa o alcance da bomba do jogador
        for (int alcance = 1; alcance <= state->alcanceBomba; alcance++) {

            int nx = bx + direcoes[d][0] * alcance;
            int ny = by + direcoes[d][1] * alcance;

            // evita acessar fora do mapa
            if (nx < 0 || nx >= wMax || ny < 0 || ny >= hMax)
                break;

            // parede solida bloqueia explosao
            if (state->screenBuffer[ny][nx] == BLOCO_SOLIDO)
                break;

            int tileAntes = state->screenBuffer[ny][nx];

            // desenha explosao ou limpa fogo
            state->screenBuffer[ny][nx] = tipoTile;

            // parede destrutivel para explosao
            if (tileAntes == PAREDE_DESTRUTIVEL)
                break;
        }
    }
}


// controla o tempo das bombas
void updateBomba() {

    for (Bomba& bomba : state->bombas) {

        // esperando explodir
        if (bomba.temBomba && !bomba.explodindo) {

            bomba.tempoBomba++;

            // velocidade da bomba pode ser alterada por item
            if (bomba.tempoBomba >= state->velocidadeBomba) {

                gerenciarExplosao(bomba, BOMBA_EXPLOSAO);

                bomba.temBomba = false;
                bomba.explodindo = true;
                bomba.cooldownBomba = 0;
            }
        }

        // explosao ativa
        if (bomba.explodindo) {

            bomba.cooldownBomba++;

            if (bomba.cooldownBomba > 20) {

                gerenciarExplosao(bomba, WHITE); // limpa fogo
                bomba.explodindo = false;
            }
        }
    }

    // remove bombas que ja terminaram completamente
    for (int i = state->bombas.size() - 1; i >= 0; i--) {
        if (!state->bombas[i].temBomba && !state->bombas[i].explodindo) {
            state->bombas.erase(state->bombas.begin() + i);
        }
    }
}