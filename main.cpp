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
vector<string> diagrama; //Matriz de letras
vector<string> listaPalavras;
vector<Resultado> resultados; //Lista final de acertos
int linhas, colunas;

// Mutex para proteger a matriz de saída (evitar condição de corrida, se duas threads tentarem escrever no mesmo local e tempo, pode trava ou gerar lixo)
pthread_mutex_t mutex_diagrama = PTHREAD_MUTEX_INITIALIZER;
// Mutex para proteger o vetor de resultados
pthread_mutex_t mutex_resultados = PTHREAD_MUTEX_INITIALIZER;

// Função para marcar a palavra encontrada como MAIÚSCULA no diagrama
void marcarNoDiagrama(int r, int c, int dr, int dc, int tam) {
    pthread_mutex_lock(&mutex_diagrama); // TRANCA O CADEADO (se a thread chega aq e tem uma "chave", ela pega e tranca. Se ja tiver outra la dentro, a atual espera a chave ser devolvida)
    for (int i = 0; i < tam; i++) {
      //Transforma a letra daquela posição em MAIÚSCULA
        diagrama[r + i * dr][c + i * dc] = toupper(diagrama[r + i * dr][c + i * dc]);
        //r + i * dr -> calcula a Linha atual (r = linha de partida) (i = atual) (dr = direção)
        //c + i * dc -> calcula a Coluna atual (c = colula de partida) (i = atual) (dc = direção)
    }
    pthread_mutex_unlock(&mutex_diagrama); // ABRE O CADEADO
}

// Lógica de busca em 8 direções
bool buscar(int r, int c, string palavra, string &dir_nome) {
  //combinações de diração: representam os passos na linhas (dr) e colunas (dc)
    int dr[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int dc[] = {-1,  0,  1, -1, 1, -1, 0, 1};
    string nomes[] = {"esquerda/cima", "cima", "direita/cima", "esquerda", 
                      "direita", "esquerda/baixo", "baixo", "direita/baixo"};
    //(-1,-1) -> esquerda/cima
    //(-1,0) -> cima
    //(-1,1) -> direita/cima
    //(0,-1) -> esquerda
    //(0,1) -> direita
    //(1,-1) -> esquerda/baixo
    //(1,0) -> baixo
    //(1,1) -> direita/baixo

    for (int d = 0; d < 8; d++) { //testa as 8 direções por vez
        int i, rr = r, cc = c;
        for (i = 0; i < palavra.length(); i++) { // usa rr e cc para "andar" no diagrama
            if (rr < 0 || rr >= linhas || cc < 0 || cc >= colunas || //verificar se esta dentro do diagrama (não saiu da borda)
                tolower(diagrama[rr][cc]) != tolower(palavra[i])) break; //a letra do diagrama é igual a letra da palavra?
            rr += dr[d];
            cc += dc[d];
        }
        if (i == palavra.length()) { //se achou, guarda no diagrama
            dir_nome = nomes[d];
            marcarNoDiagrama(r, c, dr[d], dc[d], palavra.length()); //como usamos Threads, usamos o Mutex para garantir que apenas uma Thread mude para MAIÚSCULA
            return true;
        }
    }
    return false;
}

// Função executada por cada Thread
void* tarefaBusca(void* arg) { //void* arg é o pacote de dados que chega para Thread
    ThreadArgs* dados = (ThreadArgs*)arg; //serve para "abrir o pacote" e recuperar a palavra específica que essa Thread deve procurar
    Resultado res;
    res.palavra = dados->palavra;
    res.encontrada = false;

    // Busca a palavra em cada célula do diagrama
    for (int i = 0; i < linhas; i++) { //percorre as linhas de cima p/ baixo
        for (int j = 0; j < colunas; j++) { //dentro da linha, percorre coluna da esquerda p/ direita
            if (buscar(i, j, dados->palavra, res.direcao)) { //busca se a palavra começa na coordenada
                res.linha = i + 1;
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

    pthread_exit(NULL); //finaliza a Thread atual
}

int main() {
    if (!(cin >> linhas >> colunas)) return 1;

    diagrama.resize(linhas); //quantas linhas o diagrama tera
    for (int i = 0; i < linhas; i++) cin >> diagrama[i]; //cada repetição, le uma linha inteira

    string p;
    while (cin >> p) listaPalavras.push_back(p); //le ate o final, e guarda cada palavra no final da lista

    int num_palavras = listaPalavras.size(); //recebe tamanho da lista
    pthread_t threads[num_palavras]; //cria um array pra guardar a "identidade" de cada Thread
    ThreadArgs args[num_palavras]; //cada Thread precisa de uma "mochila" de dados para saber qual palavra deve procurar sem confundir com a outra


    for (int i = 0; i < num_palavras; i++) {
        args[i].palavra = listaPalavras[i]; //coloca a palavra[i]
        args[i].id = i; // na mochila[i]
        pthread_create(&threads[i], NULL, tarefaBusca, (void*)&args[i]);
        //cria a Thread, guarda o ID dela, chama a função que vai executar, e a mochila que vai carregar
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < num_palavras; i++) {
        pthread_join(threads[i], NULL); //para e espera a Thread[i] terminar antes de seguir
    }

    // Saída final formatada
    for (const auto& linha : diagrama) cout << linha << endl; //percorre linha por linha e imprime ja com as MAIUSCULAS
    
    cout << "--- Resultados ---" << endl;
    for (const auto& r : resultados) { //percorre a lista de resultados
        if (r.encontrada)
            cout << r.palavra << " (" << r.linha << "," << r.coluna << "): " << r.direcao << endl;
        else
            cout << r.palavra << ": não encontrada" << endl;
    }

    return 0;
}
