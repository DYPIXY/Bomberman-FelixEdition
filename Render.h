#pragma once

// =============================================
// Trabalho M2 - Bomberman
// Algoritmos e Programação II
// =============================================

#include <iostream>
#include <vector>
#include <windows.h>
#include <ctime>
#include <iomanip>

#include "BomberMan.h"

// Cores do console
#define COLOR_WALL      FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define COLOR_PLAYER    FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define COLOR_PLAYER2   FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define COLOR_ENEMY     FOREGROUND_RED | FOREGROUND_INTENSITY
#define COLOR_BOSS      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define COLOR_BOMB      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define COLOR_EXPLOSION FOREGROUND_RED | FOREGROUND_INTENSITY
#define COLOR_DEFAULT   FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define COLOR_PORTAL    FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY

static const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

enum sprites {
    WHITE,
    BLOCO_SOLIDO,
    BOMBA,
    BOMBA_TICK,
    BOMBA_EXPLOSAO,
    PAREDE_DESTRUTIVEL,
    PORTAL_TILE
};

struct SpriteInfo {
    char character;
    WORD color;
};

inline SpriteInfo getSprite(int value) {
    switch (value) {
        case BLOCO_SOLIDO:       return { (char)219, COLOR_WALL };
        case PAREDE_DESTRUTIVEL: return { '#', (WORD)(COLOR_WALL & ~FOREGROUND_INTENSITY) };
        case BOMBA:              return { 'O', COLOR_BOMB };
        case BOMBA_EXPLOSAO:     return { (char)178, COLOR_EXPLOSION };
        case PORTAL_TILE:        return { 'X', COLOR_PORTAL };
        default:                 return { ' ', COLOR_DEFAULT };
    }
}

inline void renderCursorHandler() {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    SetConsoleCursorPosition(out, coord);
}

inline void limparTela() { system("cls"); }

void renderDraw() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    renderCursorHandler();

    SpriteInfo frameBuffer[hMax][wMax];

    // 1. Mapa base
    for (int i = 0; i < hMax; i++)
        for (int j = 0; j < wMax; j++)
            frameBuffer[i][j] = getSprite(state->screenBuffer[i][j]);

    // 2. Itens
    for (Item& item : state->itens)
        if (item.ativo)
            frameBuffer[item.pos.y][item.pos.x] = { item.simbolo, COLOR_BOMB };

    // 3. Inimigos
    for (const auto& e : state->enemies) {
        if (e.inimigoVivo) {
            char sym = e.boss ? 'B' : 'E';
            WORD cor = e.boss ? COLOR_BOSS : COLOR_ENEMY;
            frameBuffer[e.pos.y][e.pos.x] = { sym, cor };
        }
    }

    // 4. Jogador 1
    if (state->p1.alive) {
        char sym = (state->p1.tempoImune > 0) ? 'p' : 'P';
        frameBuffer[state->p1.pos.y][state->p1.pos.x] = { sym, COLOR_PLAYER };
    }

    // 5. Jogador 2
    if (state->p2Ativo && state->p2Vivo) {
        char sym = (state->p2.tempoImune > 0) ? 'q' : 'Q';
        frameBuffer[state->p2.pos.y][state->p2.pos.x] = { sym, COLOR_PLAYER2 };
    }

    // Imprime o frame
    for (int i = 0; i < hMax; i++) {
        for (int j = 0; j < wMax; j++) {
            SetConsoleTextAttribute(h, frameBuffer[i][j].color);
            std::cout << frameBuffer[i][j].character;
        }
        std::cout << "\n";
    }

    // HUD
    int tempoDecorrido = (int)(time(nullptr) - state->hud.inicioJogo);
    int minutos = tempoDecorrido / 60;
    int segundos = tempoDecorrido % 60;

    SetConsoleTextAttribute(h, COLOR_BOMB);
    #define SIZE_HORIZONTAL 100

    std::string formated = "+" + std::string(SIZE_HORIZONTAL, '-') + "+";
    std::string title = " FASE " + std::to_string(state->faseAtual) + "/" + std::to_string(TOTAL_FASES) +" Tempo: " + (minutos < 10 ? "0" : "") + std::to_string(minutos) + ":" +
                    (segundos < 10 ? "0" : "") + std::to_string(segundos);
    std::string hudInfo "Pontos: " << std::setw(6) << state->hud.pontuacao
              << " Movimentos: " << std::setw(4) << state->hud.movimentos
              << " Bombas: " << std::setw(3) << state->hud.bombasUsadas;
    printf("|%"+SIZE_HORIZONTAL+"s|\n", hudInfo.c_str());

    std::cout << formated << "\n";
    printf("|%"+SIZE_HORIZONTAL+"s|\n", title.c_str());
    std::string vidas = " P1 Vidas:" + std::to_string(state->p1.vidas);
    if (state->p1.escudo) 
        vidas += " [ESCUDO]";
    else 
        vidas += "         ";

    if (state->p2Ativo) 
    {
        vidas += " P2 Vidas:" + std::to_string(state->p2.vidas);
        if (state->p2.escudo) 
            vidas += " [ESCUDO]";
    }
    printf("|%"+SIZE_HORIZONTAL+"s|\n", vidas.c_str());

//    SetConsoleTextAttribute(h, COLOR_BOMB);
    std::string itens = " Items: Fogo+" + std::to_string(state->hud.itemFogo) +
                        " Bomb+" + std::to_string(state->hud.itemBombas) +
                        " Vida+" + std::to_string(state->hud.itemVidaExtra) +
                        " Rel:" + std::to_string(state->hud.itemBombaRelogio) +
                        " Esc:" + std::to_string(state->hud.itemEscudo) ;
    printf("|%"+SIZE_HORIZONTAL+"s|\n", itens.c_str());
    std::cout << formated << "\n";

    if (state->p2Ativo) {
        SetConsoleTextAttribute(h, COLOR_PLAYER);
        std::cout << "P1: WASD+E  ";
        SetConsoleTextAttribute(h, COLOR_PLAYER2);
        std::cout << "P2: Setas+Enter\n";
    } else {
        SetConsoleTextAttribute(h, COLOR_DEFAULT);
        std::cout << "WASD=Mover  E=Bomba  P=Salvar\n";
    }

    SetConsoleTextAttribute(h, COLOR_DEFAULT);
}

void renderResult(bool venceu, bool todasFases, int pontuacao, int movimentos, int bombasUsadas, int itensPegos) {
    limparTela();
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    if (todasFases) {
        SetConsoleTextAttribute(h, COLOR_PORTAL);
        std::cout << "\n\n  PARABENS! VOCE COMPLETOU TODAS AS FASES!\n";
    } else if (venceu) {
        SetConsoleTextAttribute(h, COLOR_PLAYER);
        std::cout << "\n\n  FASE CONCLUIDA!\n";
    } else {
        SetConsoleTextAttribute(h, COLOR_ENEMY);
        std::cout << "\n\n  GAME OVER!\n";
    }

    SetConsoleTextAttribute(h, COLOR_DEFAULT);
    std::cout << "\nPontuacao: " << pontuacao << "\n";
    std::cout << "Movimentos: " << movimentos << "\n";
    std::cout << "Bombas usadas: " << bombasUsadas << "\n";
    std::cout << "Itens pegos: " << itensPegos << "\n";
    std::cout << "\nPressione qualquer tecla...\n";
    system("pause > nul");
}

// Tela de transição entre fases
void renderTransicaoFase(int proxFase) {
    limparTela();
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, COLOR_PORTAL);
    std::cout << "\n\n  *** FASE " << (proxFase-1) << " CONCLUIDA! ***\n\n";
    SetConsoleTextAttribute(h, COLOR_DEFAULT);
    std::cout << "  Preparando FASE " << proxFase << "...\n";
    std::cout << "  (Itens resetados para a proxima fase)\n\n";
    if (proxFase == 3) {
        SetConsoleTextAttribute(h, COLOR_ENEMY);
        std::cout << "  AVISO: BOSS nesta fase!\n";
        SetConsoleTextAttribute(h, COLOR_DEFAULT);
    }
    std::cout << "\n  Pressione qualquer tecla para continuar...\n";
    system("pause > nul");
    limparTela();
}
