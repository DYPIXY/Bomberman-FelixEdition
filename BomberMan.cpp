
// =============================================
// Trabalho M1 - Bomberman
// Algoritmos e Programação II
// Desenvolvido por:  Joao Felix, Derick Kunz, Joao Guilherme, Eduardo Loyola
// =============================================

#include "BomberMan.h"
#include "Mapas.h"

#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <ctime>

// variaveis principais do jogo
GameState state;
int screenBuffer[hMax + 2][wMax + 2];


// LOGICA DO JOGO


// verifica se tem o target na posicao dejesada(poderia ser apenas a posicao dejesada mais pode ser util ter a posição atual de quem pediu o check)
bool checkColisao(int target, int posX, int posY, int offX, int offY) {
    int destX = posX + offX;
    int destY = posY + offY;

    // evita acessar fora do mapa
    if (destX < 0 || destX >= wMax + 2 || destY < 0 || destY >= hMax + 2)
        return true;

    return screenBuffer[destY][destX] == target;
}


// coloca bomba onde o jogador ta
void colocarBomba() {

    // apenas uma bomba
    if (state.bomba.temBomba || state.bomba.explodindo)
        return;

    state.bomba.temBomba = true;
    state.bomba.pos = state.p1.pos;
    state.bomba.tempoBomba = 0;

    screenBuffer[state.bomba.pos.y][state.bomba.pos.x] = BOMBA;
    state.hud.bombasUsadas++;
}


// cuida da explosao (criar e apagar)
void gerenciarExplosao(int tipoTile) {
    int bx = state.bomba.pos.x;
    int by = state.bomba.pos.y;

    // centro
    if (screenBuffer[by][bx] != BLOCO_SOLIDO)
        screenBuffer[by][bx] = tipoTile;

    // cima
    if (by > 0 && screenBuffer[by - 1][bx] != BLOCO_SOLIDO)
        screenBuffer[by - 1][bx] = tipoTile;

    // baixo
    if (by < hMax && screenBuffer[by + 1][bx] != BLOCO_SOLIDO)
        screenBuffer[by + 1][bx] = tipoTile;

    // esquerda
    if (bx > 0 && screenBuffer[by][bx - 1] != BLOCO_SOLIDO)
        screenBuffer[by][bx - 1] = tipoTile;

    // direita
    if (bx < wMax && screenBuffer[by][bx + 1] != BLOCO_SOLIDO)
        screenBuffer[by][bx + 1] = tipoTile;
}


// controla o tempo da bomba
void updateBomba() {

    // esperando explodir
    if (state.bomba.temBomba && !state.bomba.explodindo) {
        state.bomba.tempoBomba++;

        if (state.bomba.tempoBomba >= TICKS_UNTIL_NEXT_STATE_BOMB) {
            gerenciarExplosao(BOMBA_EXPLOSAO);

            state.bomba.temBomba = false;
            state.bomba.explodindo = true;
            state.bomba.cooldownBomba = 0;
        }
    }

    // explosao ativa
    if (state.bomba.explodindo) {
        state.bomba.cooldownBomba++;

        if (state.bomba.cooldownBomba > 20) {
            gerenciarExplosao(WHITE); // limpa fogo
            state.bomba.explodindo = false;
        }
    }
}


// cria inimigo
void criarInimigo(int x, int y) {
    state.enemies.push_back(Enemy(x, y));
}


// atualiza inimigo (movimento + morte)
void updateInimigo(Enemy& inimigo) {

    if (!inimigo.inimigoVivo)
        return;

    // morreu na explosao
    if (screenBuffer[inimigo.pos.y][inimigo.pos.x] == BOMBA_EXPLOSAO) {
        inimigo.inimigoVivo = false;
        return;
    }

    inimigo.tempoInimigo++;

    if (inimigo.tempoInimigo < TICKS_UNTIL_NEXT_POS_ENEMY)
        return;

    inimigo.tempoInimigo = 0;

    // se nao tem passos definidos, escolhe nova direcao
    if (inimigo.passosRestantes <= 0) {
        int dir = rand() % 4;

        inimigo.dx = (dir == 2 ? -1 : (dir == 3 ? 1 : 0));
        inimigo.dy = (dir == 0 ? -1 : (dir == 1 ? 1 : 0));

        inimigo.passosRestantes = (rand() % 3) + 1; // 1 a 3 passos
    }

    // tenta andar 1 passo
    bool bloqueado =
        checkColisao(BLOCO_SOLIDO, inimigo.pos.x, inimigo.pos.y, inimigo.dx, inimigo.dy) ||
        checkColisao(PAREDE_DESTRUTIVEL, inimigo.pos.x, inimigo.pos.y, inimigo.dx, inimigo.dy) ||
        checkColisao(BOMBA, inimigo.pos.x, inimigo.pos.y, inimigo.dx, inimigo.dy);

    if (!bloqueado) {
        inimigo.pos.x += inimigo.dx;
        inimigo.pos.y += inimigo.dy;
        inimigo.passosRestantes--; // anda 1 passo por frame
    }
    else {
        // se bateu em algo, cancela movimento atual
        inimigo.passosRestantes = 0;
    }
}


// ve se jogador bateu em inimigo
bool checkColisaoJogadorInimigo() {
    for (const Enemy& e : state.enemies) {
        if (e.inimigoVivo &&
            e.pos.x == state.p1.pos.x &&
            e.pos.y == state.p1.pos.y) {
            return true;
        }
    }
    return false;
}


// ganhou o jogo?
bool todosInimigosMortos() {
    for (const Enemy& e : state.enemies) {
        if (e.inimigoVivo)
            return false;
    }
    return true;
}


// entrada do teclado
void inputHandler() {

    if (!_kbhit())
        return;

    char tecla = _getch();

    int dx = 0;
    int dy = 0;

    switch (tecla) {
        case 'w': case 'W': dy = -1; break;
        case 's': case 'S': dy =  1; break;
        case 'a': case 'A': dx = -1; break;
        case 'd': case 'D': dx =  1; break;
        case 'e': case 'E': colocarBomba(); break;
        case 't': case 'T': state.session = false; break;
    }

    if (dx != 0 || dy != 0) {

        bool parede1 = checkColisao(BLOCO_SOLIDO, state.p1.pos.x, state.p1.pos.y, dx, dy);
        bool parede2 = checkColisao(PAREDE_DESTRUTIVEL, state.p1.pos.x, state.p1.pos.y, dx, dy);

        if (!parede1 && !parede2) {
            state.p1.pos.x += dx;
            state.p1.pos.y += dy;
            state.hud.movimentos++;
        }
    }
}


// tela de fim
void exibirResultado(bool venceu) {

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
    Sleep(2000);
}


// loop principal
void rodarJogo(int mapa[][wMax + 2]) {

    state = GameState();
    state.session = true;
    state.hud.inicioJogo = time(nullptr);

    // copia mapa
    for (int i = 0; i < hMax + 2; i++)
        for (int j = 0; j < wMax + 2; j++)
            screenBuffer[i][j] = mapa[i][j];

    criarInimigo(1, 7);
    criarInimigo(15, 5);
    criarInimigo(3, 3);
    criarInimigo(20, 10);
    criarInimigo(9, 6);

    bool venceu = false;

    while (state.session) {

        inputHandler();
        updateBomba();

        for (Enemy& e : state.enemies)
            updateInimigo(e);

        // morreu na explosao
        if (screenBuffer[state.p1.pos.y][state.p1.pos.x] == BOMBA_EXPLOSAO)
            state.p1.alive = false;

        // morreu no inimigo
        if (checkColisaoJogadorInimigo())
            state.p1.alive = false;

        std::vector<std::pair<int,int>> vivos;

        for (const Enemy& e : state.enemies) {
            if (e.inimigoVivo)
                vivos.push_back({e.pos.x, e.pos.y});
        }

        renderDraw(state.p1.pos.x, state.p1.pos.y, state.p1.alive, vivos, state.hud);

        if (!state.p1.alive) {
            venceu = false;
            state.session = false;
        }
        else if (todosInimigosMortos()) {
            venceu = true;
            state.session = false;
        }

        Sleep(16);
    }

    exibirResultado(venceu);
}


// menu
int exibirMenu() {

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    limparTela();
    SetConsoleTextAttribute(h, COLOR_BOMB);
    std::cout << "======================\n";
    std::cout << "\nBOMBERMAN\n";
    std::cout << "\n======================\n\n";
    SetConsoleTextAttribute(h, COLOR_DEFAULT);
    std::cout << "WASD = mover\n";
    std::cout << "E = bomba\n";
    std::cout << "T = sair\n\n";

    std::cout << "1 jogar\n";
    std::cout << "0 sair\n\n";

    int op;
    std::cin >> op;

    return op;
}


// main
int main() {

    // esconde cursor 
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO c;
    GetConsoleCursorInfo(h, &c);
    c.bVisible = false;
    SetConsoleCursorInfo(h, &c);

    srand(time(0));

    int op = -1;

    while (op != 0) {
        op = exibirMenu();

        if (op == 1)
        {
            limparTela();
            rodarJogo(map_0);
        }
    }

    limparTela();
    std::cout << "\n Fechando...\n";

    return 0;
}   