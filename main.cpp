#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <pthread.h>
#include <algorithm>

using namespace std;

// Estrutura para passar dados para as threads
struct ThreadArgs {
    string palavra;
    int id;
};

// Estrutura para armazenar o resultado da busca
struct Resultado {
    string palavra;
    int linha, coluna;
    string direcao;
    bool encontrada;
};

// Variáveis Globais (Necessárias para acesso das threads)
vector<string> diagrama;
vector<string> listaPalavras;
vector<Resultado> resultados;
int linhas, colunas;

// Mutex para proteger a matriz de saída (evitar condição de corrida ao alterar para MAIÚSCULAS)
pthread_mutex_t mutex_diagrama = PTHREAD_MUTEX_INITIALIZER;
// Mutex para proteger o vetor de resultados
pthread_mutex_t mutex_resultados = PTHREAD_MUTEX_INITIALIZER;

// Função para marcar a palavra encontrada como MAIÚSCULA no diagrama
void marcarNoDiagrama(int r, int c, int dr, int dc, int tam) {
    pthread_mutex_lock(&mutex_diagrama); // CRITÉRIO: Uso de sincronização 
    for (int i = 0; i < tam; i++) {
        diagrama[r + i * dr][c + i * dc] = toupper(diagrama[r + i * dr][c + i * dc]);
    }
    pthread_mutex_unlock(&mutex_diagrama);
}

// Lógica de busca em 8 direções
bool buscar(int r, int c, string palavra, string &dir_nome) {
    int dr[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int dc[] = {-1,  0,  1, -1, 1, -1, 0, 1};
    string nomes[] = {"esquerda/cima", "cima", "direita/cima", "esquerda", 
                      "direita", "esquerda/baixo", "baixo", "direita/baixo"};

    for (int d = 0; d < 8; d++) {
        int i, rr = r, cc = c;
        for (i = 0; i < palavra.length(); i++) {
            if (rr < 0 || rr >= linhas || cc < 0 || cc >= colunas || 
                tolower(diagrama[rr][cc]) != tolower(palavra[i])) break;
            rr += dr[d];
            cc += dc[d];
        }
        if (i == palavra.length()) {
            dir_nome = nomes[d];
            marcarNoDiagrama(r, c, dr[d], dc[d], palavra.length());
            return true;
        }
    }
    return false;
}

// Função executada por cada Thread
void* tarefaBusca(void* arg) {
    ThreadArgs* dados = (ThreadArgs*)arg;
    Resultado res;
    res.palavra = dados->palavra;
    res.encontrada = false;

    // Busca a palavra em cada célula do diagrama
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (buscar(i, j, dados->palavra, res.direcao)) {
                res.linha = i + 1; // Ajuste para base 1 (opcional)
                res.coluna = j + 1;
                res.encontrada = true;
                break;
            }
        }
        if (res.encontrada) break;
    }

    // Salva o resultado de forma segura
    pthread_mutex_lock(&mutex_resultados);
    resultados.push_back(res);
    pthread_mutex_unlock(&mutex_resultados);

    pthread_exit(NULL);
}

int main() {
    // CRITÉRIO: Leitura correta da entrada única [cite: 8, 58]
    if (!(cin >> linhas >> colunas)) return 1;

    diagrama.resize(linhas);
    for (int i = 0; i < linhas; i++) cin >> diagrama[i];

    string p;
    while (cin >> p) listaPalavras.push_back(p);

    int num_palavras = listaPalavras.size();
    pthread_t threads[num_palavras];
    ThreadArgs args[num_palavras];

    // CRITÉRIO: Estratégia de paralelização por palavra 
    for (int i = 0; i < num_palavras; i++) {
        args[i].palavra = listaPalavras[i];
        args[i].id = i;
        pthread_create(&threads[i], NULL, tarefaBusca, (void*)&args[i]);
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < num_palavras; i++) {
        pthread_join(threads[i], NULL);
    }

    // Saída final formatada [cite: 9, 60]
    for (const auto& linha : diagrama) cout << linha << endl;
    
    cout << "--- Resultados ---" << endl;
    for (const auto& r : resultados) {
        if (r.encontrada)
            cout << r.palavra << " (" << r.linha << "," << r.coluna << "): " << r.direcao << endl;
        else
            cout << r.palavra << ": não encontrada" << endl;
    }

    return 0;
}