#pragma once 

#include <iostream>
#include "Render.h"
#include <windows.h> // Sleep()
#include <conio.h> // _kbhit() e _getch()
using namespace std;

int renderMenu() {

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    const int TOTAL_OPCOES = 4;

    std::string opcoes[TOTAL_OPCOES] = {
        "Jogar",
        "Dificuldades",
        "Creditos",
        "Sair"
    };

    int selecionado = 0;

    limparTela(); // limpa só uma vez

    while (true) {

        // volta cursor pro inicio
        renderCursorHandler();

        SetConsoleTextAttribute(h, COLOR_BOMB);

        std::cout << R"(
  ____                  _                 __  __             
 |  _ \                | |               |  \/  |            
 | |_) | ___  _ __ ___ | |__   ___ _ __  | \  / | __ _ _ __  
 |  _ < / _ \| '_ ` _ \| '_ \ / _ \ '__| | |\/| |/ _` | '_ \ 
 | |_) | (_) | | | | | | |_) |  __/ |    | |  | | (_| | | | |
 |____/ \___/|_| |_| |_|_.__/ \___|_|    |_|  |_|\__,_|_| |_|
                                                             
)";

        SetConsoleTextAttribute(h, COLOR_DEFAULT);

        std::cout << "\nUse W/S para navegar\n";
        std::cout << "ENTER para selecionar\n\n";

        for (int i = 0; i < TOTAL_OPCOES; i++) {

            if (i == selecionado) {

                SetConsoleTextAttribute(h, COLOR_PLAYER);
                std::cout << " > " << opcoes[i] << " <\n";

            } else {

                SetConsoleTextAttribute(h, COLOR_DEFAULT);
                std::cout << "   " << opcoes[i] << "   \n";
            }
        }

        SetConsoleTextAttribute(h, COLOR_DEFAULT);

        // espaços extras pra apagar lixo visual
        std::cout << "\n\n\n\n\n";

        if (_kbhit()) {

            char tecla = _getch();

            switch (tecla) {

                case 'w':
                case 'W':

                    selecionado--;

                    if (selecionado < 0)
                        selecionado = TOTAL_OPCOES - 1;

                    break;

                case 's':
                case 'S':

                    selecionado++;

                    if (selecionado >= TOTAL_OPCOES)
                        selecionado = 0;

                    break;

                case 13: // ENTER
                    return selecionado + 1;
            }
        }

        Sleep(50);
    }
}