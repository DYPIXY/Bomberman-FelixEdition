#pragma once

// =============================================
// Trabalho M1 - Bomberman
// Algoritmos e Programação II
// Desenvolvido por:  Joao Felix, Derick Kunz, Joao Guilherme, Eduardo Loyola
// =============================================

#include <iostream>
#include <windows.h>
#include <vector>
#include "Render.h"
#include <ctime>


// CONSTANTES

//Modificar esses valores para + desacelera e para - acelera a bomba/inimigo.
#define TICKS_UNTIL_NEXT_POS_ENEMY 30
#define TICKS_UNTIL_NEXT_STATE_BOMB 45


// STRUCTS (Dados do Jogo)

//facilita o acesso a cordanada de entidades 
struct Coordenadas {
    int x, y;
};

struct Player {
    Coordenadas pos;
    bool alive;
    //jogador começa em (1,1) para nao nascer dentro da parede
    Player() { pos = {1, 1}; alive = true; }
};

struct Enemy {
    Coordenadas pos;
    bool inimigoVivo;
    int tempoInimigo;
    int dx = 0;
    int dy = 0;
    int passosRestantes = 0;

    Enemy(int x, int y) : pos({x, y}), inimigoVivo(true), tempoInimigo(0) {}
};

struct Bomba {
    Coordenadas pos;
    bool temBomba   = false;
    bool explodindo = false;
    int tempoBomba  = 0;
    int cooldownBomba = 0;
};

struct GameState {
    bool session = false;
    Player p1;
    Bomba bomba;
    std::vector<Enemy> enemies;
    HudInfo hud;
};


//Variaveis (Acessiveis por outros arquivos)

extern GameState state;
extern int screenBuffer[hMax + 2][wMax + 2];


//Prototipos das funções


void rodarJogo(int mapa[][wMax + 2]);
void inputHandler();
void updateBomba();
void updateInimigo(Enemy& inimigo);
void colocarBomba();
void gerenciarExplosao(int tipoTile);
bool checkColisao(int target, int posX, int posY, int offX, int offY);
void criarInimigo(int x, int y);
