#pragma once

// =============================================
// Trabalho M1 - Bomberman
// Algoritmos e Programação II
// Desenvolvido por:  Joao Felix, Derick Kunz, Joao Guilherme, Eduardo Loyola
// =============================================

#include <iostream>
#include <vector>
#include <windows.h>
#include <ctime>
#include <iomanip>

#include "BomberMan.h"

// cores usadas no console 
#define COLOR_WALL      FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define COLOR_PLAYER    FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define COLOR_ENEMY     FOREGROUND_RED | FOREGROUND_INTENSITY
#define COLOR_BOMB      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY 
#define COLOR_EXPLOSION FOREGROUND_RED | FOREGROUND_INTENSITY
#define COLOR_DEFAULT   FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE 

//init var
static const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

// tipos de tiles que podem aparecer no mapa
enum sprites { 
    WHITE, 
    BLOCO_SOLIDO, 
    BOMBA, 
    BOMBA_TICK, 
    BOMBA_EXPLOSAO, 
    PAREDE_DESTRUTIVEL 
};

// guarda o que desenhar (char + cor)
struct SpriteInfo {
    char character;
    WORD color;
};


// transforma o numero do mapa em algo visivel
inline SpriteInfo getSprite(int value) {
    switch (value) {
        case BLOCO_SOLIDO:
            return { (char)219, COLOR_WALL };

        case PAREDE_DESTRUTIVEL:
            // mesma cor da parede, so que mais fraca
            return { '#', (WORD)(COLOR_WALL & ~FOREGROUND_INTENSITY) };

        case BOMBA:
            return { 'O', COLOR_BOMB };

        case BOMBA_EXPLOSAO:
            return { (char)178, COLOR_EXPLOSION };
        default:
            return { ' ', COLOR_DEFAULT };
    }
}


// script do professor pra nao flickar
inline void renderCursorHandler() {

    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(out, coord);
    
}


// limpa a tela inteira (obvio)
inline void limparTela() {
    system("cls");
}

void renderDraw()
{
    // buffer do main
    extern int screenBuffer[hMax][wMax];

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // volta cursor pro inicio pra sobrescrever frame antigo
    renderCursorHandler();

    for (int i = 0; i < hMax; i++) {
        for (int j = 0; j < wMax; j++) {

            // verifica se tem inimigo aqui
            bool temInimigo = false;

            for (const auto& pos : state->enemies) {
                if (i == pos.pos.y && j == pos.pos.x) {
                    temInimigo = true;
                    break;
                }
            }

            // ordem de prioridade:
            // jogador > inimigo > mapa
            if (state->p1.alive && i == state->p1.pos.y && j == state->p1.pos.x) 
            {
                SetConsoleTextAttribute(hConsole, COLOR_PLAYER);
                std::cout << 'P';
            }
            else if (temInimigo) 
            {
                SetConsoleTextAttribute(hConsole, COLOR_ENEMY);
                std::cout << 'E';
            }
            else 
            {
                SpriteInfo info = getSprite(state->screenBuffer[i][j]);

                SetConsoleTextAttribute(hConsole, info.color);
                std::cout << info.character;
            }
        }

        std::cout << "\n";
    }
    
    //FOOTER <- DEIXAR NO FINAL!!
    int tempoDecorrido = time(nullptr) - state->hud.inicioJogo;
    int minutos = tempoDecorrido / 60;
    int segundos = tempoDecorrido % 60;

    SetConsoleTextAttribute(hConsole, COLOR_BOMB);

    std::cout << "\n+----------------------+\n";

    std::cout << "| Tempo: "
              << std::setw(2) << std::setfill('0') << minutos
              << ":"
              << std::setw(2) << segundos
              << std::setfill(' ')
              << " \n";

    std::cout << "| Pontos: " << state->hud.pontuacao << " \n";
    std::cout << "| Movimentos: " << state->hud.movimentos << " \n";
    std::cout << "| Bombas: " << state->hud.bombasUsadas << " \n";

    std::cout << "+----------------------+\n";

    std::cout << "| Fogo +" << state->hud.itemFogo << " \n";
    std::cout << "| Bombas +" << state->hud.itemBombas << " \n";
    std::cout << "| Vida +" << state->hud.itemVidaExtra << " \n";
    std::cout << "| Relogio " << state->hud.itemBombaRelogio << " \n";
    std::cout << "| Escudo " << state->hud.itemSobreviveBomba << " \n";
    std::cout << "| Passa Blocos " << state->hud.itemPassaBlocos << " \n";

    std::cout << "+----------------------+\n";
    //END FOOTER

    SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
    
    // volta cor padrao no final
    SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
}

// tela de fim
void renderResult(bool venceu) {

    limparTela();

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    if (venceu) {
        SetConsoleTextAttribute(h, COLOR_PLAYER);
        std::cout << "\n\n  VOCE VENCEU!\n";
    } else {
        SetConsoleTextAttribute(h, COLOR_ENEMY);
        std::cout << "\n\n  GAME OVER!\n";
    }

    SetConsoleTextAttribute(h, COLOR_DEFAULT);

    std::cout << "\nvoltando...\n";
}
