# Trabalho Pesca-Palavras (Sistemas Operacionais)


## Comandos para Teste no Terminal Linux


### 1. Compilação
Compila o código-fonte e vincula a biblioteca de threads necessária:
```bash
g++ main.cpp -o pesca_palavras -lpthread
```
### 2. Execução
Executa o programa utilizando o arquivo de entrada que contém as dimensões, o diagrama e as palavras:
```bash
./pesca_palavras < cacapalavras.txt
```
### 3. Gerar Arquivo de Saída
Executa a busca e salva todo o resultado em um arquivo de texto:
```bash
./pesca_palavras < cacapalavras.txt > resultado.txt
```
