#pragma Once

#include "BomberMan.h"

#define TICKS_UNTIL_NEXT_STATE_BOMB 45


// coloca bomba onde o jogador ta
void colocarBomba() {

    // apenas uma bomba
    if (state->bomba.temBomba || state->bomba.explodindo)
        return;

    state->bomba.temBomba = true;
    state->bomba.pos = state->p1.pos;
    state->bomba.tempoBomba = 0;

    state->screenBuffer[state->bomba.pos.y][state->bomba.pos.x] = BOMBA;
    state->hud.bombasUsadas++;
}


// cuida da explosao (criar e apagar)
void gerenciarExplosao(int tipoTile) {
    int bx = state->bomba.pos.x;
    int by = state->bomba.pos.y;

    // centro
    if (state->screenBuffer[by][bx] != BLOCO_SOLIDO)
        state->screenBuffer[by][bx] = tipoTile;

    // cima
    if (by > 0 && state->screenBuffer[by - 1][bx] != BLOCO_SOLIDO)
        state->screenBuffer[by - 1][bx] = tipoTile;

    // baixo
    if (by < hMax - 1 && state->screenBuffer[by + 1][bx] != BLOCO_SOLIDO)
        state->screenBuffer[by + 1][bx] = tipoTile;

    // esquerda
    if (bx > 0 && state->screenBuffer[by][bx - 1] != BLOCO_SOLIDO)
        state->screenBuffer[by][bx - 1] = tipoTile;

    // direita
    if (bx < wMax - 1 && state->screenBuffer[by][bx + 1] != BLOCO_SOLIDO)
        state->screenBuffer[by][bx + 1] = tipoTile;
}


// controla o tempo da bomba
void updateBomba() {

    // esperando explodir
    if (state->bomba.temBomba && !state->bomba.explodindo) {
        state->bomba.tempoBomba++;

        if (state->bomba.tempoBomba >= TICKS_UNTIL_NEXT_STATE_BOMB) {
            gerenciarExplosao(BOMBA_EXPLOSAO);

            state->bomba.temBomba = false;
            state->bomba.explodindo = true;
            state->bomba.cooldownBomba = 0;
        }
    }

    // explosao ativa
    if (state->bomba.explodindo) {
        state->bomba.cooldownBomba++;

        if (state->bomba.cooldownBomba > 20) {
            gerenciarExplosao(WHITE); // limpa fogo
            state->bomba.explodindo = false;
        }
    }
}



