// Roberto Marinheiro
// ROBO AUTONOMO
// include
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constante
#define MAX 100

// Definicao das estruturas

// Ponto->X, Y
typedef struct{
    int x, y;
    }Ponto;

// VISIBILIDADE->PONTOS[], COMPLETOVISIVEL(INT 0/1), QUANTIDADE
typedef struct{
    Ponto *pontos;
    int completoVisivel, quantidade;
}Visibilidade;

// MAPA->MATRIX[][], ALTURA(M), LARGURA(N), INICIO(PONTO)
typedef struct{
    int **mapa;
    int altura, largura;
    Ponto inicio;
}Mapa;

// Definicao das funcoes
// INICIALIZACAO
Mapa * initMapa(); // ok
Visibilidade * initVisibilidade(); // ok
// GRAFICA
void printMapa(Mapa *mapa); // ok
void printPontos(Visibilidade *visibilidade); // ok
// GERAL
void setGuards(Mapa *mapa, Visibilidade *visibilidade); // ok
Ponto catchNext(Mapa *mapa); // ok
// LIBERAMENTO DE MEMORIA
void liberaMapa(Mapa *mapa); // ok
void liberaVisibilidade(Visibilidade *visibilidade); // ok
// M1
int processamentoVisibilidade(Mapa *mapa, Visibilidade *visibilidade); // ok
void raio(int xa, int ya, int xb, int yb, Mapa *mapa); // ok

// FUNCOES
// INICIALIZACAO
// Inicializa a estrutura mapa lendo o arquivo externo "inputMap.txt"
Mapa * initMapa(){
    int i, a, aa;
    Mapa *mapa=(Mapa*)calloc(1,sizeof(Mapa));
    Ponto ponto;
    // FORMATO DE LEITURA: 
        // ALTURA
        // LARGURA
        // MAPA
        // PONTO DE INICIO
    FILE *file = fopen("inputMap.txt","r");
    if(file == NULL){
        perror("Error in opening file");
        return NULL;
    }
    // Recebendo Altura e Largura
    fscanf(file,"%d", &mapa->altura);
    fscanf(file,"%d", &mapa->largura);
    // Alocando
    mapa->mapa=(int**)calloc(mapa->altura, sizeof(int*));
    for(i; i<mapa->altura; i++)
        mapa->mapa[i] = (int*)calloc(mapa->largura ,sizeof(int));
    // Recebendo valores do mapa
    for(a = 0; a < mapa->altura; a++){
        for(aa = 0; aa < mapa->largura; aa++){
            fscanf(file,"%d", &mapa->mapa[a][aa]);
        }
    }
    fscanf(file,"%d %d", &ponto.x, &ponto.y);
    mapa->inicio = ponto;
    fclose(file);
    return mapa;
}

// Inicializa a estrutura visibilidade
Visibilidade * initVisibilidade(){
    Visibilidade *visibilidade=(Visibilidade*)calloc(1,sizeof(Visibilidade));
    visibilidade->pontos=(Ponto*)calloc(MAX,sizeof(Ponto*)); // Numero maximo de pontos de guarda: 100
    visibilidade->completoVisivel = 0;
    visibilidade->quantidade = -1;
}

// GRAFICA
// Printa o mapa
void printMapa(Mapa *mapa){
    int a, aa;
    for(a=0; a<mapa->altura; a++){
        printf("\n");
        for(aa=0; aa<mapa->largura; aa++){
            if(mapa->mapa[a][aa]==0) printf("  "); // VAZIO SEM VISAO
            if(mapa->mapa[a][aa]==1) printf(" #"); // OBSTACULO
            if(mapa->mapa[a][aa]==2) printf(" o"); // VAZIO COM VISAO
            if(mapa->mapa[a][aa]==8) printf(" @"); // PONTO DE GAURDA
        }
    }
    printf("\nLegenda: \n' ' = espaco vazio sem visao\n'#' = espaco com obstaculo\n'o' = espaco vazio com visao\n'@' = ponto de guarda\n");
}
// Printa os guardas
void printPontos(Visibilidade *visibilidade){
    int i;
    for(i=0; i<visibilidade->quantidade; i++){
        printf("\nGuarda %d, x: %d, y: %d", i, visibilidade->pontos[i].x, visibilidade->pontos[i].y);
    }
    printf("\n");
}

// GERAL
void setGuards(Mapa *mapa, Visibilidade *visibilidade){
    int i;
    for(i=0; i<visibilidade->quantidade; i++){
        mapa->mapa[visibilidade->pontos[i].x][visibilidade->pontos[i].y] = 8;
    }
    printf("\n");
}

// Procura proximo ponto a ser visto e caso toda a matrix ja esteja completamente visivel retorna X e Y do ponto como = -1
Ponto catchNext(Mapa *mapa){
    int aa, a;
    Ponto proximo;
    for(a=0; a<mapa->altura; a++){
        for(aa=0; aa<mapa->largura; aa++){
            if(mapa->mapa[a][aa]==0){
                proximo.x = a;
                proximo.y = aa;
                return proximo;
            }
        }
    }
    proximo.x = -1;
    proximo.y = -1;
    return proximo;
}

// LIBERAMENTO DE MEMORIA
// Libera memoria alocada para a estrutura Mapa
void liberaMapa(Mapa *mapa){
    printf("\nLiberando memoria alocada pela estrutura Mapa...");
    int i;
    for(i=0; i<mapa->altura; i++)
        free(mapa->mapa[i]);
    free(mapa->mapa);
    free(mapa);
}

// Libera memoria alocada para a estrutura visibilidade
void liberaVisibilidade(Visibilidade *visibilidade){
    printf("\nLiberando memoria alocada pela estrutura Visibilidade...");
    free(visibilidade->pontos);
    free(visibilidade);
}

// M1
// Faz o processamento da visibilidade disparando raio para todas as direcoes e pegando novos pontos de guarda
int processamentoVisibilidade(Mapa *mapa, Visibilidade *visibilidade){
    printf("\nMODULO-1: Visibilidade...");
    int i, j;
    Ponto proximoPonto = mapa->inicio;
    while(visibilidade->completoVisivel==0){
        visibilidade->quantidade++;
        visibilidade->pontos[visibilidade->quantidade] = proximoPonto;
        // DOIS LOOPS: UM PARA ATIRAR NO SENTIDO VERTICAL, E OUTRO NO HORIZONTAL  
        for(i=0; i<mapa->largura; i++){ // SENTIDO VERTICAL
            raio(proximoPonto.x, proximoPonto.y, 0, i, mapa); // CIMA -> 0, I (ITERAR ATE LARGURA)
            raio(proximoPonto.x, proximoPonto.y, mapa->altura, i, mapa); // BAIXO -> ALTURA, I (ITERAR ATE LARGURA)
        }
        for(j=0; j<mapa->altura; j++){ // SENTIDO HORIZONTAL
            raio(proximoPonto.x, proximoPonto.y, j, mapa->largura, mapa); // DIREITA -> J, LARGURA (ITERAR ATE ATLURA) 
            raio(proximoPonto.x, proximoPonto.y, j, 0, mapa); // ESQUERDA -> J, 0 (ITERAR ATE ALTURA)
        }
        proximoPonto = catchNext(mapa);
        if(proximoPonto.x == -1) visibilidade->completoVisivel = 1;
    }
    return 0;
}

// Cria o raio a ser disparado para verificar a visibilidade
void raio(int x0, int y0, int x1, int y1, Mapa *mapa) { // ~BRESEHANS LINE ALGORITHM~ 
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;
    for(;;){ // Esse loop infinito pode ser alterado por um While((x0==x1 && y0==y1)) || mapa->mapa[x0][y0]==1)
        // E como medida de seguranca para esse loop, podemos adicionar algum "timeout", para o programa jamais travar nesse loop (apesar de ser muito improvavel)
        if(x0==x1 && y0==y1){
            break; // Fim do Raio
        }
        if(mapa->mapa[x0][y0]==0){
            mapa->mapa[x0][y0] = 2;
        }
        if(mapa->mapa[x0][y0]==1){
            break; // Fim do Raio - Sem visao a partir desse ponto
        }
        e2 = err;
        if(e2 >-dx){
            err -= dy; x0 += sx;
            }
        if(e2 < dy){
            err += dx; y0 += sy;
        }
    }
}

// MAIN 
int main(){
    // Iniciailizando estruturas
    Mapa *mapa = initMapa();
    if(mapa == NULL) return 0;
    Visibilidade *visibilidade = initVisibilidade();
    processamentoVisibilidade(mapa, visibilidade);
    setGuards(mapa, visibilidade);
    printMapa(mapa);
    printPontos(visibilidade);
    // Liberando Memoria
    liberaMapa(mapa);
    liberaVisibilidade(visibilidade);
    return 0;
}
