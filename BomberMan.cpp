// =============================================
// Trabalho M1 - Bomberman
// Algoritmos e Programação II
// Desenvolvido por:  Joao Felix, Derick Kunz, Joao Guilherme, Eduardo Loyola
// =============================================

#include "BomberMan.h"
#include "Mapas.h"
#include "Inimigo.h"
#include "Bomba.h"
#include "Item.h"
#include "Menu.h"

#include <signal.h>
#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <ctime>


// verifica se tem o target na posicao dejesada(poderia ser apenas a posicao dejesada mais pode ser util ter a posição atual de quem pediu o check)
bool checkColisao(int target, int posX, int posY, int offX, int offY) {
    int destX = posX + offX;
    int destY = posY + offY;

    // evita acessar fora do mapa
    if (destX < 0 || destX >= wMax || destY < 0 || destY >= hMax)
        return true;

    return state->screenBuffer[destY][destX] == target;
}


// ve se jogador bateu em inimigo
bool checkColisaoJogadorInimigo() {
    for (const Enemy& e : state->enemies) {
        if (e.inimigoVivo &&
            e.pos.x == state->p1.pos.x &&
            e.pos.y == state->p1.pos.y) {
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
        case 't': case 'T': state->session = false; break;
    }

    if (dx != 0 || dy != 0) {

        bool parede1 = checkColisao(BLOCO_SOLIDO, state->p1.pos.x, state->p1.pos.y, dx, dy);
        bool parede2 = checkColisao(PAREDE_DESTRUTIVEL, state->p1.pos.x, state->p1.pos.y, dx, dy);

        // se o jogador tiver o item passa blocos, ele pode passar pela parede destrutivel
        if (!parede1 && (!parede2 || state->p1.passaBlocos)) {
            state->p1.pos.x += dx;
            state->p1.pos.y += dy;
            state->hud.movimentos++;

            if (state->hud.pontuacao > 0)
                state->hud.pontuacao--;
        }
    }
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

static const char* savePath = "saves/savegame.dk";
void saveGame() 
{
    FILE* file = fopen(savePath, "wb");
    if (file) {
        fwrite(&state, sizeof(GameState), 1, file);
    }
    fclose(file);   
}

GameState loadGame() 
{
    GameState loadedState;
    FILE* file = fopen(savePath, "rb");
    if (file)
        fread(&loadedState, sizeof(GameState), 1, file);
    else
        loadedState = GameState();
    
    fclose(file);
    return loadedState;
}

// loop principal
void rodarJogo(int mapa[][wMax]) 
{
    state = new GameState();
    state->session = true;
    state->hud.inicioJogo = time(nullptr);

    // copia mapa
    for (int i = 0; i < hMax; i++)
        for (int j = 0; j < wMax; j++)
            state->screenBuffer[i][j] = mapa[i][j];

    criarInimigo(mapa, 3);
    criarInimigo(mapa);

    spawnarItens();

    bool venceu = false;
    bool bonusVitoriaAplicado = false;

    struct timespec req = {};
    while (state->session) 
    {
        clock_t start = clock(); 

        inputHandler();
        verificarColetaItem();
        updateBomba();

        // controla tempo de imunidade apos perder vida ou escudo
        if (state->p1.tempoImune > 0)
            state->p1.tempoImune--;

        for (Enemy& e : state->enemies) {
            bool estavaVivo = e.inimigoVivo;

            updateInimigo(e);

            if (estavaVivo && !e.inimigoVivo) {
                state->hud.pontuacao += 50;
            }
        }

        // morreu na explosao
        if (state->p1.tempoImune == 0 &&
            state->screenBuffer[state->p1.pos.y][state->p1.pos.x] == BOMBA_EXPLOSAO) {

            if (state->p1.escudo) {
                state->p1.escudo = false;

                if (state->hud.itemEscudo > 0)
                    state->hud.itemEscudo--;

                state->p1.tempoImune = 20;
            }
            else if (state->p1.vidas > 1) {
                state->p1.vidas--;

                if (state->hud.itemVidaExtra > 0)
                    state->hud.itemVidaExtra--;

                state->p1.tempoImune = 20;
            }
            else {
                state->p1.alive = false;
            }
        }

        // morreu no inimigo
        if (state->p1.tempoImune == 0 && checkColisaoJogadorInimigo()) {
            if (state->p1.escudo) {
                state->p1.escudo = false;

                if (state->hud.itemEscudo > 0)
                    state->hud.itemEscudo--;

                state->p1.tempoImune = 20;
            }
            else if (state->p1.vidas > 1) {
                state->p1.vidas--;

                if (state->hud.itemVidaExtra > 0)
                    state->hud.itemVidaExtra--;

                state->p1.tempoImune = 20;
            }
            else {
                state->p1.alive = false;
            }
        }

        std::vector<std::pair<int,int>> vivos;

        for (const Enemy& e : state->enemies) {
            if (e.inimigoVivo)
                vivos.push_back({e.pos.x, e.pos.y});
        }

        renderDraw();

        if (!state->p1.alive) {
            venceu = false;
            state->session = false;
        }
        else if (todosInimigosMortos()) {
            if (!bonusVitoriaAplicado) {
                state->hud.pontuacao += 200;
                bonusVitoriaAplicado = true;
            }

            venceu = true;
            state->session = false;
        }

        clock_t end = clock(); 
        double elapsed = (double) (end - start) / CLOCKS_PER_SEC;
        if (elapsed < FRAME_SPEED) 
        {
            req.tv_sec = 0;
            req.tv_nsec = (FRAME_SPEED - elapsed) * 1000000000; // converte para nanosegundos
            nanosleep(&req, nullptr);
        }
    }
    renderResult(venceu);
}


// main
int main() 
{
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

//controle de sinais de interrupt para poder salvar o jogo ao fechar
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
        {    
            Beep(750, 500);
            saveGame();
            limparTela();
            return FALSE;
        }
        default:
            return FALSE;
    }
}