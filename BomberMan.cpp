// =============================================
// Trabalho M1 - Bomberman
// Algoritmos e Programação II
// Desenvolvido por:  Joao Felix, Derick Kunz, Joao Guilherme, Eduardo Loyola
// =============================================

#include "BomberMan.h"
#include "Mapas.h"
#include "inimigo.h"
#include "Bomba.h"
#include "Menu.h"

#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <ctime>

// variaveis principais do jogo
GameState state;
int screenBuffer[hMax][wMax];


// LOGICA DO JOGO


// verifica se tem o target na posicao dejesada(poderia ser apenas a posicao dejesada mais pode ser util ter a posição atual de quem pediu o check)
bool checkColisao(int target, int posX, int posY, int offX, int offY) {
    int destX = posX + offX;
    int destY = posY + offY;

    // evita acessar fora do mapa
    if (destX < 0 || destX >= wMax || destY < 0 || destY >= hMax)
        return true;

    return screenBuffer[destY][destX] == target;
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

char sortearTipoItem()
{
    char tipos[] = {'F', 'B', 'V', 'R', 'E', 'P'};
    return tipos[rand() % 6];
}

void spawnarItens()
{
    int quantidade = (rand() % 6) + 2; // 2 até 7 itens

    for (int i = 0; i < quantidade; i++)
    {
        int x, y;

        do {
            x = rand() % wMax;
            y = rand() % hMax;

        } while (
            screenBuffer[y][x] != WHITE ||
            (x == state.p1.pos.x && y == state.p1.pos.y)
        );

        char simbolo = sortearTipoItem();

        state.itens.push_back(Item(x, y, simbolo));
    }
}

void renderizarItens()
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    for (Item& item : state.itens)
    {
        if (!item.ativo)
            continue;

        COORD coord;
        coord.X = item.pos.x;
        coord.Y = item.pos.y;
        SetConsoleCursorPosition(h, coord);

        SetConsoleTextAttribute(h, COLOR_BOMB);
        std::cout << item.simbolo;
    }

    SetConsoleTextAttribute(h, COLOR_DEFAULT);
}

void verificarColetaItem()
{
    for (Item& item : state.itens)
    {
        if (!item.ativo)
            continue;

        if (item.pos.x == state.p1.pos.x && item.pos.y == state.p1.pos.y)
        {
            item.ativo = false;

            if (item.simbolo == 'F')
                state.hud.itemFogo++;

            else if (item.simbolo == 'B')
                state.hud.itemBombas++;

            else if (item.simbolo == 'V')
                state.hud.itemVidaExtra++;

            else if (item.simbolo == 'R')
                state.hud.itemBombaRelogio++;

            else if (item.simbolo == 'E')
                state.hud.itemEscudo++;

            else if (item.simbolo == 'P')
                state.hud.itemPassaBlocos++;
        }
    }
}

// loop principal
void rodarJogo(int mapa[hMax][wMax]) {

    state = GameState();
    state.session = true;
    state.hud.inicioJogo = time(nullptr);

    // copia mapa
    for (int i = 0; i < hMax; i++)
        for (int j = 0; j < wMax; j++)
            screenBuffer[i][j] = mapa[i][j];

    criarInimigo(mapa, 3);
    criarInimigo(mapa);


    spawnarItens();

    bool venceu = false;

    while (state.session) {

        inputHandler();
        verificarColetaItem();
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
        renderizarItens();

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
    renderResult(venceu);
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

    bool executando = true;

    while (executando) {
        int opcao = renderMenu(); // Supondo que seu menu retorne a escolha (1 = Jogar, 2 = Sair)

        if (opcao == 1) {
            limparTela();
            rodarJogo(map_0); // Quando rodarJogo acabar, o 'while' vai repetir e mostrar o menu de novo!
        } 
        else if (opcao == 4) {
            executando = false; // Fecha o jogo de forma limpa
        }
    }
    return 0;
}
