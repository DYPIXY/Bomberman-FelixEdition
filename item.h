#pragma once

#include "BomberMan.h"

// sorteia qual item vai aparecer no mapa
char sortearTipoItem()
{
    char tipos[] = {'F', 'B', 'V', 'R', 'E', 'P'};
    return tipos[rand() % 6];
}

// spawna itens aleatorios no mapa
void spawnarItens()
{
    int quantidade = (rand() % 6) + 3; // 3 até 8 itens

    for (int i = 0; i < quantidade; i++)
    {
        int x, y;

        do {
            x = rand() % wMax;
            y = rand() % hMax;

        } while (
            state->screenBuffer[y][x] != WHITE ||
            (x == state->p1.pos.x && y == state->p1.pos.y)
        );

        char simbolo = sortearTipoItem();

        state->itens.push_back(Item(x, y, simbolo));
    }
}

// verifica se o jogador coletou algum item
void verificarColetaItem()
{
    for (Item& item : state->itens)
    {
        if (!item.ativo)
            continue;

        if (item.pos.x == state->p1.pos.x && item.pos.y == state->p1.pos.y)
        {
            item.ativo = false;
            state->hud.itensPegos++;
            state->hud.pontuacao += 10;

            if (item.simbolo == 'F') {
                state->hud.itemFogo++;
                state->alcanceBomba++;
            }

            else if (item.simbolo == 'B') {
                state->hud.itemBombas++;
                state->limiteBombas++;
            }

            else if (item.simbolo == 'V') {
                state->hud.itemVidaExtra++;
                state->p1.vidas++;
            }

            else if (item.simbolo == 'R') {
                state->hud.itemBombaRelogio++;

                if (state->velocidadeBomba > 10)
                    state->velocidadeBomba -= 5;
            }

            else if (item.simbolo == 'E') {
                state->hud.itemEscudo++;
                state->p1.escudo = true;
            }

            else if (item.simbolo == 'P') {
                state->hud.itemPassaBlocos++;
                state->p1.passaBlocos = true;
            }
        }
    }
}