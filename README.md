# Trabalho Pesca-Palavras (Sistemas Operacionais)


## Comandos para Teste no Terminal Linux

Siga os passos abaixo para compilar e executar o projeto no ambiente de defesa (Codespaces):

### 1. Compilação
Este comando compila o código-fonte e vincula a biblioteca de threads necessária:
```bash
g++ main.cpp -o pesca_palavras -lpthread
```
### 2. Execução
Este comando executa o programa utilizando o arquivo de entrada que contém as dimensões, o diagrama e as palavras:
```bash
./pesca_palavras < cacapalavras.txt
```
### 3. Gerar Arquivo de Saída
Este comando executa a busca e salva todo o resultado em um arquivo de texto, facilitando a conferência do diagrama:
```bash
./pesca_palavras < cacapalavras.txt > resultado.txt
```
