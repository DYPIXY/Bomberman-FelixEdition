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


// cores usadas no console 
#define COLOR_WALL      FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define COLOR_PLAYER    FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define COLOR_ENEMY     FOREGROUND_RED | FOREGROUND_INTENSITY
#define COLOR_BOMB      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY 
#define COLOR_EXPLOSION FOREGROUND_RED | FOREGROUND_INTENSITY
#define COLOR_DEFAULT   FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE 

#define wMax 20
#define hMax 10

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
SpriteInfo getSprite(int value) {
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
void renderCursorHandler() {

    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(out, coord);
    
}


// limpa a tela inteira (obvio)
void limparTela() {
    system("cls");
}

struct HudInfo {
    int pontuacao = 0;
    int movimentos = 0;
    int bombasUsadas = 0;
    int itensPegos = 0;

    int itemFogo = 0;
    int itemBombas = 0;
    int itemVidaExtra = 0;
    int itemBombaRelogio = 0;
    int itemSobreviveBomba = 0;
    int itemPassaBlocos = 0;

    time_t inicioJogo = 0;
};


void renderHUD(HANDLE hConsole, const HudInfo& hud) {
    int tempoDecorrido = time(nullptr) - hud.inicioJogo;
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

    std::cout << "| Pontos: " << hud.pontuacao << " \n";
    std::cout << "| Movimentos: " << hud.movimentos << " \n";
    std::cout << "| Bombas: " << hud.bombasUsadas << " \n";

    std::cout << "+----------------------+\n";

    std::cout << "| Fogo +" << hud.itemFogo << " \n";
    std::cout << "| Bombas +" << hud.itemBombas << " \n";
    std::cout << "| Vida +" << hud.itemVidaExtra << " \n";
    std::cout << "| Relogio " << hud.itemBombaRelogio << " \n";
    std::cout << "| Escudo " << hud.itemSobreviveBomba << " \n";
    std::cout << "| Passa Blocos " << hud.itemPassaBlocos << " \n";

    std::cout << "+----------------------+\n";

    SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
}

// desenha tudo na tela
// jogador + inimigos + mapa
void renderDraw(int playerX, int playerY, bool playerAlive,
                const std::vector<std::pair<int,int>>& inimigosVivos,
                const HudInfo& hud)
{
    // buffer do main
    extern int screenBuffer[hMax + 2][wMax + 2];

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // volta cursor pro inicio pra sobrescrever frame antigo
    renderCursorHandler();

    for (int i = 0; i < hMax + 2; i++) {
        for (int j = 0; j < wMax + 2; j++) {

            // verifica se tem inimigo aqui
            bool temInimigo = false;

            for (const auto& pos : inimigosVivos) {
                if (i == pos.second && j == pos.first) {
                    temInimigo = true;
                    break;
                }
            }

            // ordem de prioridade:
            // jogador > inimigo > mapa
            if (playerAlive && i == playerY && j == playerX) {

                SetConsoleTextAttribute(hConsole, COLOR_PLAYER);
                std::cout << 'P';

            }
            else if (temInimigo) {

                SetConsoleTextAttribute(hConsole, COLOR_ENEMY);
                std::cout << 'E';

            }
            else {

                SpriteInfo info = getSprite(screenBuffer[i][j]);

                SetConsoleTextAttribute(hConsole, info.color);
                std::cout << info.character;
            }
        }

        std::cout << "\n";
    }
    
    renderHUD(hConsole, hud);
    
    // volta cor padrao no final
    SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
}