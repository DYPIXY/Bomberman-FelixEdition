#pragma once

#include "bomberman.h"

#define TICKS_UNTIL_NEXT_POS_ENEMY 30

// cria inimigo
void criarInimigo(int x, int y) {
    state.enemies.push_back(Enemy(x, y));
}

// Sobrecarga: Spawna uma quantidade 'qtd' de inimigos em posições aleatórias totalmente vazias
void criarInimigo(int mapa[hMax][wMax], int qtd) {
    int inimigosCriados = 0;
    int tentativasMaximas = 1000; // Evita loop infinito caso o mapa esteja lotado
    int tentativas = 0;

    while (inimigosCriados < qtd && tentativas < tentativasMaximas) {
        tentativas++;

        // Sorteia uma coordenada aleatória dentro do mapa
        int x = rand() % wMax;
        int y = rand() % hMax;

        // 1. O tile precisa ser chão comum (0)
        if (mapa[y][x] != 0) {
            continue;
        }

        // 2. Não pode spawnar em cima do Player (posição inicial dele é 1,1)
        if (x == 1 && y == 1) {
            continue;
        }

        // 3. Não pode spawnar onde já existe outro inimigo criado
        bool jaTemInimigo = false;
        for (const Enemy& e : state.enemies) {
            if (e.pos.x == x && e.pos.y == y) {
                jaTemInimigo = true;
                break;
            }
        }

        if (jaTemInimigo) {
            continue;
        }

        // Se passou em todas as validações, o lugar é seguro!
        criarInimigo(x, y); // Chama a função original baseada em (x, y)
        inimigosCriados++;
    }
}


// Sobrecarga: Procura um ID específico no mapa e spawna inimigos nessas posições
void criarInimigo(int mapa[hMax][wMax]) {
    for (int i = 0; i < hMax; i++) {
        for (int j = 0; j < wMax; j++) {
            // Se encontrar o número 99, cria o inimigo ali
            if (mapa[i][j] == 99) {
                criarInimigo(j, i); // Chama a função original (x=j, y=i)
                
                // Opcional: Limpa o '99' do mapa e coloca chão (0) 
                // para o inimigo não nascer "dentro" de um bloco sólido
                mapa[i][j] = 0; 
            }
        }
    }
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

    // Calcula a posição para onde o inimigo deseja ir
    int proxX = inimigo.pos.x + inimigo.dx;
    int proxY = inimigo.pos.y + inimigo.dy;

    // 1. Check de colisão com o cenário (blocos, paredes e bombas)
    bool bloqueado =
        checkColisao(BLOCO_SOLIDO, inimigo.pos.x, inimigo.pos.y, inimigo.dx, inimigo.dy) ||
        checkColisao(PAREDE_DESTRUTIVEL, inimigo.pos.x, inimigo.pos.y, inimigo.dx, inimigo.dy) ||
        checkColisao(BOMBA, inimigo.pos.x, inimigo.pos.y, inimigo.dx, inimigo.dy);

    // 2. Check de colisão com OUTROS inimigos vivos
    if (!bloqueado) {
        for (const Enemy& outro : state.enemies) {
            // Ignora o próprio inimigo que está se movendo (compara o endereço de memória)
            if (&outro == &inimigo) {
                continue;
            }
            
            // Se o outro inimigo estiver vivo e na mesma posição de destino, bloqueia o movimento
            if (outro.inimigoVivo && outro.pos.x == proxX && outro.pos.y == proxY) {
                bloqueado = true;
                break;
            }
        }
    }

    // Executa a movimentação ou cancela os passos
    if (!bloqueado) {
        inimigo.pos.x = proxX;
        inimigo.pos.y = proxY;
        inimigo.passosRestantes--; // anda 1 passo por frame
    }
    else {
        // se bateu em algo (cenário ou outro inimigo), cancela movimento atual
        // para recalcular uma nova direção no próximo update
        inimigo.passosRestantes = 0;
    }
}

// ganhou o jogo?
bool todosInimigosMortos() {
    for (const Enemy& e : state.enemies) {
        if (e.inimigoVivo)
            return false;
    }
    return true;
}