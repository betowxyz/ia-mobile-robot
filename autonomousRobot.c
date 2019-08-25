// Roberto Marinheiro
// ROBO AUTONOMO -> Pc troll fdp ;D
// include
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
Mapa * initMapa();
Visibilidade * initVisibilidade();
// GRAFICA
void printMap(Map *mapa); // ok
// GERAL
void setGuards(Mapa *mapa, Visibilidade *visibilidade);
Ponto catchNext(Mapa *mapa); // ok
// LIBERAMENTO DE MEMORIA
void liberaMapa(Mapa *mapa);
void liberaVisibilidade(Visibilidade *visibilidade);
// M1
int processamentoVisibilidade(Mapa *mapa, Visibilidade *visibilidade); // ok
void raio(int xa, int ya, int xb, int yb, Mapa *mapa); // ok

// FUNCOES
// INICIALIZACAO
// Inicializa a estrutura mapa lendo o arquivo externo "inputMap.txt"
Mapa * initMapa(){
    int i = 0, a = 0, aa = 0, valor, altura, largura, xInicio, yInicio;
    Mapa *mapa=(Mapa*)calloc(1,sizeof(Mapa));
    Ponto *ponto=(Ponto*)calloc(1,sizeof(Ponto));
    // Reading File
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
    fscanf(file,"%d", &altura);
    fscanf(file,"%d", &largura);
    mapa->largura = largura;
    mapa->altura = altura;

    // Alocando
    mapa->mapa=(int**)calloc(mapa->altura,sizeof(int*));
    for(i; i<mapa->largura; i++)
        mapa->mapa[i] = (int*)calloc(mapa->largura,sizeof(int));
    // Recebendo valores do mapa
    for(a;a<mapa->altura;a++){
        for(aa;aa<mapa->largura;aa++){
        fscanf(file,"%d", &valor);
            mapa->mapa[a][aa] = valor;
        }
    }
    fscanf(file,"%d %d", &xInicio, &yInicio);
    ponto->x = xInicio;
    ponto->y = yInicio;

    return mapa;
}

// Inicializa a estrutura visibilidade
// Visibilidade * initVisibilidade(){
//     Visibilidade *visibilidade=(Visibilidade*)calloc(1,sizeof(Visibilidade));
// }

// GRAFICA
// Printa o mapa
void printMap(Map *mapa, Visibilidade *visibilidade){
    for(a;a<mapa->altura;a++){
        printf("\n");
        for(aa;aa<mapa->largura;aa++){
            if(mapa->mapa[a][aa]==0) printf("  "); // VAZIO SEM VISAO
            if(mapa->mapa[a][aa]==1) printf(" #"); // OBSTACULO
            if(mapa->mapa[a][aa]==2) printf(" o"); // VAZIO COM VISAO
            if(mapa->mapa[a][aa]==8) printf("  @"); // PONTO DE GAURDA
        }
    }
    printf("\nLegenda: \n' ' = espaco vazio sem visao\n'#' = espaco com obstaculo\n'o' = espaco vazio com visao\n'@' = ponto de guarda\n");
}

// GERAL
// Procura proximo ponto a ser visto e caso toda a matrix ja esteja completamente visivel retorna X e Y do ponto como = -1
Ponto catchNext(Mapa *mapa){
    int aa=0, a=0;
    Ponto proximo;
    for(a;a<mapa->altura;a++){
        for(aa;aa<mapa->largura;aa++){
            if(mapa->mapa[a][aa]==0){
                proximo->x = a;
                proximo->y = aa;
                return proximo;
            }
        }
    }
    proximo->x = -1;
    proximo->y = -1;
    return proximo;
}

// LIBERAMENTO DE MEMORIA
// Libera memoria alocada para a estrutura Mapa
void liberaMapa(Mapa *mapa){
    for(int i=0; i<mapa->altura; i++)
        free(mapa->mapa[i]);
    free(mapa->mapa);
    free(mapa);
}
// Libera memoria alocada para a estrutura visibilidade
void liberaVisibilidade(Visibilidade *visibilidade){
    free(visibilidade->pontos);
    free(visibilidade);
}

// M1
// Faz o processamento da visibilidade disparando raio para todas as direcoes e pegando novos pontos de guarda
int processamentoVisibilidade(Mapa *mapa, Visibilidade *visibilidade){
    int i, j; 
    Ponto proximoPonto = mapa->inicio; 
    while(visibilidade->completoVisivel==0){
        visibilidade->quantidade++;
        visibilidade->pontos[visibilidade->quantidade] = proximoPonto;
        // DOIS LOOPS: UM PARA ATIRAR NO SENTIDO VERTICAL, E OUTRO NO HORIZONTAL  
        for(i=0;i<mapa->largura;i++){ // SENTIDO VERTICAL
            raio(proximoPonto->x, proximoPonto->y, 0, i, mapa); // CIMA -> 0, I (ITERAR ATE LARGURA)
            raio(proximoPonto->x, proximoPonto->y, mapa->altura, i, , mapa); // BAIXO -> ALTURA, I (ITERAR ATE LARGURA)
        }
        for(j=0;j<mapa->altura;j++){ // SENTIDO HORIZONTAL
            raio(proximoPonto->x, proximoPonto->y, j, mapa->largura, mapa); // DIREITA -> J, LARGURA (ITERAR ATE ATLURA) 
            raio(proximoPonto->x, proximoPonto->y, 0, j, mapa); // ESQUERDA -> 0, J (ITERAR ATE ALTURA)
        }
        proximoPonto = catchNext(mapa);
        if(proximoPonto->x == -1) visibilidade->completoVisivel = 1;
    }
    return 0;
}

void raio(int xa, int ya, int xb, int yb, Mapa *mapa) { // ~BRESEHANS LINE ALGORITHM~ 
int dx = abs(xb-xa), sx = xa<xb ? 1 : -1;
int dy = abs(yb-ya), sy = ya<yb ? 1 : -1;
int err = (dx>dy ? dx : -dy)/2, e2;
for(;;){ // -> xa, ya
    if(mapa->mapa[xa][ya]==0) mapa->mapa[xa][ya]=2;
    if(xa==xb && ya==yb || mapa->mapa[xa][ya]==1) break; // Fim do raio
    e2 = err;
    if(e2 >-dx) err -= dy; xa += sx;
    if(e2 < dy) err += dx; ya += sy;
    }
}

// MAIN 
int main(){
    // Iniciailizando estruturas
    Mapa *mapa = InitMapa();
    if(mapa == NULL) return 0;
    Visibilidade *visibilidade = initVisibilidade();
    printMapa(mapa);
    processamentoVisibilidade(mapa, visibilidade);
    printMapa(mapa);
    // Liberando Memoria
    liberaMapa(mapa);
    liberaVisibilidade(visibilidade);
    return 0;
}
