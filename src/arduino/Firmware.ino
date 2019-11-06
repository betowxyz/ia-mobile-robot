// Bibliotecas 
#include "Arduino.h"
#include "DCMDriverL298.h"
#include "SD.h"

// Bibliotecas Externas (Proprias)
extern "C"{
  #include "general.h"
  #include "visibilidade.h"
  #include "pathPlanning.h"
  #include "robot.h"
};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// Definicao dos PIN's/
#define DCMOTORDRIVERL298_PIN_INT1  4
#define DCMOTORDRIVERL298_PIN_ENB 3
#define DCMOTORDRIVERL298_PIN_INT2  5
#define DCMOTORDRIVERL298_PIN_ENA 2
#define DCMOTORDRIVERL298_PIN_INT3  6
#define DCMOTORDRIVERL298_PIN_INT4  7
#define SDFILE_PIN_CS 53

// Motor and SD

// Inicializacao de objetos
DCMDriverL298 dcMotorDriverL298(DCMOTORDRIVERL298_PIN_ENA,DCMOTORDRIVERL298_PIN_INT1,DCMOTORDRIVERL298_PIN_INT2,DCMOTORDRIVERL298_PIN_ENB,DCMOTORDRIVERL298_PIN_INT3,DCMOTORDRIVERL298_PIN_INT4);

File sdFile;

/*
    CONSTANTES DE CONTROLE
*/

// Guardas
#define MAXGUARDA (10)
// Ordenacao dos pontos de guarda
#define MAXCUSTO (999999)
// Path
#define MAXPATH (40)
// Heap
#define MAXHEAPSIZE (10)
// ClosedList
#define MAXCLOSEDLIST (100)


/*
INICIALIZACAO DE ESTRUTURAS
*/

/*
    Inicializa a estrutura ponto
*/
Ponto initPonto(int x, int y){
    Ponto ponto;
    ponto.x = x;
    ponto.y = y;
    return ponto;
}

/*
    Inicializa a estrutura mapa lendo o arquivo externo "inputMap.txt"
*/
Mapa * initMapa(){
    Serial.println("Inicializando Mapa...");
    int i, a, aa;
    Mapa *mapa=(Mapa*)calloc(1,sizeof(Mapa));
    Ponto ponto;
    // FORMATO DE LEITURA:
        // ALTURA
        // LARGURA
        // MAPA
        // PONTO DE INICIO
    sdFile = SD.open("mapa.txt");
    if(sdFile == NULL){
        Serial.println("Erro abrindo arquivo de entrada do mapa...");
        return NULL;
    }
    // Recebendo Altura e Largura
    mapa->altura = (int)sdFile.parseInt();
    mapa->largura = (int)sdFile.parseInt();
    // Alocando
    mapa->mapa=(int**)calloc(mapa->altura, sizeof(int*));
    for(i; i<mapa->altura; i++){
        mapa->mapa[i] = (int*)calloc(mapa->largura ,sizeof(int));
    }
    // Recebendo valores do mapa
    for(a = 0; a < mapa->altura; a++){
        for(aa = 0; aa < mapa->largura; aa++){
            mapa->mapa[a][aa] = (int)sdFile.parseInt();
        }
    }
    // Coordenada Inicial
    ponto.x = (int)sdFile.parseInt();
    ponto.y = (int)sdFile.parseInt();
    mapa->inicio = ponto;
    // Fecha
    sdFile.close();
    return mapa;
}

/*
    Inicializa a estrutura visibilidade
*/
Visibilidade * initVisibilidade(){
  Serial.println("Inicializando Visibilidade...");
    Visibilidade *visibilidade=(Visibilidade*)calloc(1,sizeof(Visibilidade));
    visibilidade->pontos=(Ponto*)calloc(MAXGUARDA,sizeof(Ponto*)); // Numero maximo de pontos de guarda: MAXGUARDA
    visibilidade->completoVisivel = 0;
    visibilidade->quantidade = 0;
    return visibilidade;
}

/*
    Inicializa a estrutrua node
*/
Node initNode(Ponto posicao){
    Node node, parente;
    node.posicao = posicao;
    node.parente = NULL;
    node.g = 0;
    node.h = 0;
    node.f = 0;
    return node;
}

/*
    Inicializa a estrutrua node VAZIO
*/
Node initEmptyNode(){
    Node node;
    Ponto emptyCoord = initPonto(-1, -1);
    node.posicao = emptyCoord;
    node.parente = NULL;
    node.g = 0;
    node.h = 0;
    node.f = 0;
    return node;
}

/*
    Inicializa a estrutra node a partir de um node existente e o devolve como ponteiro
*/
Node * initNodePointer(Node node){
    Node * nodePointer = (Node*)calloc(1, sizeof(Node));
    nodePointer->f = node.f;
    nodePointer->g = node.g;
    nodePointer->h = node.h;
    nodePointer->posicao = node.posicao;
    nodePointer->parente = node.parente;
    return nodePointer;
}

/*
    Inicializa a estrutrua closedList
*/
ClosedList * initClosedList(){
    ClosedList *closedList = (ClosedList*)calloc(1, sizeof(ClosedList));
    closedList->nodes = (Node*)calloc(MAXCLOSEDLIST, sizeof(Node));
    closedList->tamanho = 0;
    return closedList;
}

/*
    Inicializa a estrutrua path
*/
Path * initPath(){
    Path *path=(Path*)calloc(1, sizeof(Path));
    path->path=(Ponto*)calloc(MAXPATH ,sizeof(Ponto));
    path->tamanho = 0;
    path->custo = 0;
    return path;
}

/*
    Inicializa a estrutura LISTA PATH, /*
    Executa Path Planning entre o ponto inicial e os outros pontos de guarda, procurando pelo ponto com caminho mais proximo
*/
ListaPath * initListaPath(Visibilidade *visibilidade, Mapa *mapa){
    ListaPath * listaPath = (ListaPath*)calloc(1, sizeof(ListaPath));
    listaPath->tamanho = visibilidade->quantidade-1;
    listaPath->paths = (Path*)calloc(listaPath->tamanho, sizeof(Path));
    listaPath->pathsAux = (Path*)calloc(visibilidade->quantidade, sizeof(Path));
    Path *path, *pathOtimo;
    int i, j, k, index;
    Ponto aux;
    float menorCusto;
    // Loop para cada ponto
    for(i=0; i<visibilidade->quantidade-1; i++){
        menorCusto = MAXCUSTO;
        pathOtimo = NULL;
        for(j=i+1; j<visibilidade->quantidade; j++){
            path = aStar(visibilidade->pontos[i], visibilidade->pontos[j], mapa); 
            if(path->custo < menorCusto){
                if(pathOtimo){
                    liberaPath(pathOtimo);
                }
                pathOtimo = path;
                index = j;
                menorCusto = path->custo;
            }
            else{
                liberaPath(path);
            }
        }
        listaPath->paths[i] = *pathOtimo;
        aux = visibilidade->pontos[i+1];
        visibilidade->pontos[i+1] = visibilidade->pontos[index];
        visibilidade->pontos[index] = aux;
    }
    return listaPath;
}

/*
    Inicializa a estrutura MaxHeap (PriorityQueue) (tamanho=0)
*/
PriorityQueue initMaxHeap() {
    PriorityQueue H;
    H = malloc(sizeof ( struct HeapStruct));
    H->Elements = malloc((MAXHEAPSIZE+1)*sizeof (Node)); // Aloca a lista com um espaco a mais para sentinela
    H->capacidade = MAXHEAPSIZE;
    H->tamanho = 0;
    Node emptyNode = initEmptyNode();
    H->Elements[0]= emptyNode;
    return H;
}

/*
    Robot Initialization
*/
Robot * initRobot(){
    /*
        Formato de Leitura:
            x y = (X, Y) (ponto de inicio)
            d = direcao (0->NO, 1->N, 2->NE, 3->L, 4->SE, 5->S, 6->SO, 7->O)
            l = largura
            c = comprimento
    */
    int x, y;
    Robot * robot = (Robot*)calloc(1, sizeof(Robot));
    sdFile = SD.open("robot.txt");
    if(!sdFile){
        Serial.println("\nErro abrindo arquivo de entrada do mapa...");
        return NULL;
    }
    x = (int)sdFile.parseInt();
    y = (int)sdFile.parseInt();
    Ponto inicio = initPonto(x, y);
    robot->inicio = inicio;
    robot->atual = inicio;
    robot->direcao = (int)sdFile.parseInt();
    robot->largura = (int)sdFile.parseInt();
    robot->comprimento = (int)sdFile.parseInt();
    sdFile.close();
    return robot;
}

/* FUNCOES GRAFICAS */

/*
    Printa o mapa
*/
void printMapa(Mapa *mapa){
    Serial.println("- - - - - - - - - - - - - - MAPA - - - - - - - - - - - - - - ");
    int a, aa;
    for(a=0; a<mapa->altura; a++){
        Serial.print("\n");
        for(aa=0; aa<mapa->largura; aa++){
            if(mapa->mapa[a][aa]==2) Serial.print(" -"); /* LIVRE - COM VISAO */
            else if(mapa->mapa[a][aa]==-5) Serial.print(" o"); /* LIVRE - PARTE DA ROTA */
            else if(mapa->mapa[a][aa]==-9) Serial.print(" ~"); /* LIVRE - PARTE DA ROTA */
            else if(mapa->mapa[a][aa]==1) Serial.print(" #"); /* CHEIO - OBSTACULO */
            else if(mapa->mapa[a][aa]==0) Serial.print("  "); /* LIVRE - SEM VISAO */
            else if(mapa->mapa[a][aa]>9) Serial.print(mapa->mapa[a][aa]-10); /* LIVRE - PONTO DE GAURDA */
        }
    }
  Serial.print("\n\n");
}

/*
    Printa os guardas
*/
void printGuardas(Visibilidade *visibilidade){
    printf("\nPrintando Guardas:");
    int i;
    for(i=0; i<visibilidade->quantidade; i++){
        printf("\nGuarda %d, x: %d, y: %d", i, visibilidade->pontos[i].x, visibilidade->pontos[i].y);
    }
    printf("\n");
}

/*
    Printa as rotas geradas pelo AStar
*/
void printListaPath(ListaPath *listaPath){
    for(int i=0; i<listaPath->tamanho; i++){
        printf("\nRota (custo: %f) %d:\n", listaPath->paths[i].custo, i);
        for(int j = listaPath->paths[i].tamanho-1; j>=0; j--){
            printf("(%d, %d)", listaPath->paths[i].path[j].x, listaPath->paths[i].path[j].y, " -> ");
        }
    }
}

/*
    Printa coordenadas do ponto
*/
void printPonto(Ponto ponto){
    printf("\n(%d, %d)", ponto.x, ponto.y);
}

/*
    Printa direcao correspondente ao numero
*/
void printDirecao(int direcao){
    switch(direcao){
    case 0:
        printf("NO");
        break;
    case 1:
        printf("N");
        break;
    case 2:
        printf("NE");
        break;
    case 3:
        printf("L");
        break;
    case 4:
        printf("SE");
        break;
    case 5:
        printf("S");
        break;
    case 6:
        printf("SO");
        break;
    case 7:
        printf("O");
        break;
    default:
        printf("\nError.");
        break;
    }
}

/* FUNCOES GERAIS */

/*
    Posiciona os pontos de GUARDA no mapa
*/
void setGuardas(Mapa *mapa, Visibilidade *visibilidade){
    for(int i=0; i<visibilidade->quantidade; i++){
        mapa->mapa[visibilidade->pontos[i].x][visibilidade->pontos[i].y] = 10+i;
    }
}

/*
    Posiciona os pontos de ROTA no mapa
*/
void setPath(Mapa *mapa, ListaPath *listaPath){
    for(int i=0; i<listaPath->tamanho; i++){
        for(int j=0; j<listaPath->paths[i].tamanho; j++){
            mapa->mapa[listaPath->paths[i].path[j].x][listaPath->paths[i].path[j].y] = -5;
        }
    }
}

/*
    Procura proximo ponto sem visada e se todos os pontos do mapa ja estiverem completamente visiveis, retorna X e Y do ponto como = -1
*/
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

/*
    ESTRUTURAIS
*/

/*
    Insere item em ClosedList
*/
void pushClosedList(ClosedList *closedList, Node node){
    closedList->nodes[closedList->tamanho] = node;
    closedList->tamanho++;
}

/*
    Adiciona Ponto a estrutura Path
*/
void pushPath(Path *path, Ponto ponto){
    path->path[path->tamanho] = ponto;
    path->tamanho++;
}

/*
    Verificar se MaxHeap esta vazia
*/
bool maxHeapVazia(PriorityQueue H) {
    return H->tamanho == 0;
}

/*
    Verificar se MaxHeap esta cheia
*/
bool maxHeapCheia(PriorityQueue H) {
    return H->tamanho == H->capacidade;
}

/*
    Insere no na MaxHeap
*/
void pushMaxHeap(PriorityQueue H, Node X) {
    int i;
    if(maxHeapCheia(H)){
        printf("\nErro: MaxHeap Cheia...");
        return;
    }
    for (i = ++H->tamanho; H->Elements[i/2].f > X.f; i/= 2){
        H->Elements[i] = H->Elements[i/2];
    }
    H->Elements[i] = X;
}

/*
    Retira o menor elemento da MaxHeap e o retorna
*/
Node popMaxHeap(PriorityQueue H) {
    int i, child;
    Node minElement, lastElement;
    if (maxHeapVazia(H)){
        return H->Elements[0];
    }
    minElement = H->Elements[1];
    lastElement = H->Elements[H->tamanho--];
    for (i=1; i*2<=H->tamanho; i=child) {
        child = i*2;
        if (child != H->tamanho && H->Elements[child+1].f < H->Elements[child].f){
            child++;
            }
        if (lastElement.f>H->Elements[child].f){
            H->Elements[i] = H->Elements[child];
        }
        else{
            break;
        }
    }
    H->Elements[i] = lastElement;
    return minElement;
}

/*
    FUNCOES DE LIBERAMENTO DE MEMORIA
*/

/*
    Libera memoria alocada para a estrutura MAPA
*/
void liberaMapa(Mapa *mapa){
    if(mapa){
      int i;
      for(i=0; i<mapa->altura; i++)
          free(mapa->mapa[i]);
      free(mapa->mapa);
      free(mapa);
    }
}

/*
    Libera memoria alocada para a estrutura VISIBILIDADE
*/
void liberaVisibilidade(Visibilidade *visibilidade){
    free(visibilidade->pontos);
    free(visibilidade);
}

/*
    Liberamento de memoria utilizada pela estrutura ClosedList
*/
void liberaClosedList(ClosedList *closedList){
    int i;
    for(i=0; i<closedList->tamanho; i++){
        if(closedList->nodes[i].parente){
            free(closedList->nodes[i].parente);
        }
    }
    free(closedList->nodes);
    free(closedList);
}

/*
    Libera memoria alocada para LISTAPATH
*/
void liberaListaPath(ListaPath *listaPath){
    int i;
    for(i=0; i<listaPath->tamanho; i++){
        free(listaPath->paths[i].path);
        }
    free(listaPath->paths);
    free(listaPath);
}

/*
    Liberamento de Memoria da MaxHeap
*/
void liberaMaxHeap(PriorityQueue H) {
    int i;
    for(i=0; i<H->tamanho; i++){
        if(H->Elements[i].parente){
            free(H->Elements[i].parente);
        }
    }
    free(H->Elements);
    free(H);
}

/*
    Libera memoria alocada pela estrutura Robot
*/
void liberaRobot(Robot *robot){
    free(robot);
}

/*
    Libera memoria alocada pela estrutura Path
*/
void liberaPath(Path *path){
    free(path->path);
    free(path);
}

/*
    M1 VISIBILIDADE
*/

/*
    Faz o processamento da visibilidade disparando raio para todas as direcoes (360)..
    .. e selecionando novos pontos de guarda
*/
int processamentoVisibilidade(Mapa *mapa, Visibilidade *visibilidade){
  Serial.println("Processando Visibilidade");
    int i, j;
    Ponto proximoPonto = mapa->inicio;
    while(visibilidade->completoVisivel==0){
        visibilidade->pontos[visibilidade->quantidade] = proximoPonto;
        visibilidade->quantidade++;
        /* DOIS LOOPS: UM PARA ATIRAR NO SENTIDO VERTICAL, E OUTRO NO HORIZONTAL */
        for(i=0; i<=mapa->largura; i++){ // SENTIDO VERTICAL
            raio(proximoPonto.x, proximoPonto.y, 0, i, mapa); /* CIMA -> 0, I (ITERAR ATE LARGURA) */
            raio(proximoPonto.x, proximoPonto.y, mapa->altura, i, mapa); /* BAIXO -> ALTURA, I (ITERAR ATE LARGURA) */
        }
        for(j=0; j<=mapa->altura; j++){ /* SENTIDO HORIZONTAL */
            raio(proximoPonto.x, proximoPonto.y, j, mapa->largura, mapa); /* DIREITA -> J, LARGURA (ITERAR ATE ATLURA) */
            raio(proximoPonto.x, proximoPonto.y, j, 0, mapa); /* ESQUERDA -> J, 0 (ITERAR ATE ALTURA) */
        }
        proximoPonto = catchNext(mapa);
        if(proximoPonto.x == -1) visibilidade->completoVisivel = 1;
    }
    return 0;
}

/*
    Cria o raio a ser disparado para verificar a visibilidade
*/
void raio(int x0, int y0, int x1, int y1, Mapa *mapa) { /* ~BRESENHAMS LINE ALGORITHM~ */
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;
    for(;;){
        if(x0==x1 && y0==y1){
            break; /* Fim do Raio */
        }
        if(mapa->mapa[x0][y0]==0){
            mapa->mapa[x0][y0] = 2;
        }
        if(mapa->mapa[x0][y0]==1){
            break; /* Fim do Raio - Sem visao a partir desse ponto */
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

/*
    M2 PATH PLANNING
*/

/*
    Calculo de distancia a partir da heuristica
*/
float heuristica(Ponto a, Ponto b){
    return sqrt(pow((b.x - a.x), 2) + pow((b.y - a.y),2));
}

/*
    Procura o caminho mais curto entre dois pontos
*/
Path * aStar(Ponto inicio, Ponto objetivo, Mapa *mapa){
    /* Propriedades de nodes vizinhos */
    int numeroVizinhos = 8;
    int novaPosicaoX[8]= {0, 0, 1, -1, 1, 1, -1, -1};
    int novaPosicaoY[8]= {1, -1, 0, 0, 1, -1, 1, -1};
    /* Variaveis de controle (loop) */
    int i, j, k;
    /* Variaveis de controle de busca das listas */
    bool vizinhoOpen, vizinhoClosed;
    /* Tentativa de novo Score */
    float tentativaG;
    /* Criando nodes para inicio, objetivo, atual e vizinho */
    Node inicioNode = initNode(inicio);
    Node objetivoNode = initNode(objetivo);
    Node atual;
    /* Criando no VIZINHO */
    Node vizinho = initEmptyNode();
    /* Inicializar lista aberta e fechada */
    PriorityQueue openMaxHeap = initMaxHeap();
    ClosedList * closedList = initClosedList();
    /* Inserir ponto de inicio a lista aberta */
    pushMaxHeap(openMaxHeap, inicioNode);
    /* Loop ate encontrar o objetivo */
    while(openMaxHeap->tamanho){
        /* Retirar atualNode da openMaxHeap e adicionar a listaFechada */
        atual = popMaxHeap(openMaxHeap);
        /* Encontrou o objetivo */
        if(atual.posicao.x == objetivoNode.posicao.x && atual.posicao.y == objetivoNode.posicao.y){
            Path *path = initPath();
            path->custo = atual.g;
            while(atual.parente != NULL){
                pushPath(path, atual.posicao);
                atual = *atual.parente;
            }
            pushPath(path, atual.posicao);
            /* Liberando Memoria alocada pelas estruturas */
            liberaMaxHeap(openMaxHeap);
            liberaClosedList(closedList);
            /* Retornando */
            return path;
        }
        /* Inserindo no atual na lista fechada (Ou seja, ele ja foi visitado/expandido) */
        pushClosedList(closedList, atual);
        /* Gerando celulas filhas ((0, -1), (0, 1), (-1, 0), (1, 0), (-1, -1), (-1, 1), (1, -1), (1, 1)) */
        for(i=0; i<numeroVizinhos; i++){
            // Pegando as novas posicoes */
            vizinho.posicao.x = atual.posicao.x + novaPosicaoX[i]; /* X */
            vizinho.posicao.y = atual.posicao.y + novaPosicaoY[i]; /* Y */
            /* Calcular novo custo */
            tentativaG = atual.g + heuristica(atual.posicao, vizinho.posicao);
            /* Verficiar se nao sao "out of bounds", se for o caso, apenas passa para a proxima iteracao */
            if(vizinho.posicao.x > mapa->altura || vizinho.posicao.x < 0 || vizinho.posicao.y > mapa->largura || vizinho.posicao.y < 0){
                continue;
            }
            /* Verificar se nao eh parede, se for o caso, apenas passa para a proxima iteracao */
            if(mapa->mapa[vizinho.posicao.x][vizinho.posicao.y] == 1){
                continue;
            }
            /* Variavel vizinhoClosed para verificar se o no esta na lista fechada */
            vizinhoClosed = false;
            /* Verificar se o vizinho esta na lista fechada */
            for(j=0; j <= closedList->tamanho; j++){
                if(closedList->nodes[j].posicao.x == vizinho.posicao.x && closedList->nodes[j].posicao.y == vizinho.posicao.y){
                    vizinhoClosed = true;
                    break;
                    }
                }
            /* Se vizinho esta na lista fechada e tentativa G >= custo G do vizinho, apenas passa para proxima iteracao */
            if(vizinhoClosed && tentativaG >= vizinho.g){
                continue;
            }
            /* Variavel vizinhoOpen para verificar se o no esta na lista aberta */
            vizinhoOpen = false;
            /* Verificar se o vizinho esta na lista aberta */
                for(k=0; k<=openMaxHeap->tamanho; k++){
                    if(openMaxHeap->Elements[k].posicao.x == vizinho.posicao.x && openMaxHeap->Elements[k].posicao.y == vizinho.posicao.y){
                        vizinhoOpen = true;
                        break;
                    }
                }
            /* Verificar se o custo da tentativa eh menor que o antigo */
            if(tentativaG < vizinho.g || !vizinhoOpen){ /* O problema esta nessa associacao de ponteiro */
                Node * aux = initNodePointer(atual); /* Desalocamos todos os parentes nas funcoes de liberamento de memoria da MaxHeap & da ClosedList */
                vizinho.parente = aux;
                vizinho.g = tentativaG;
                vizinho.f = tentativaG + heuristica(vizinho.posicao, objetivoNode.posicao);
                pushMaxHeap(openMaxHeap, vizinho);
            }
        }
    }
    /* Nao encontrou caminho, retorna caminho vazio */
    Path *path = initPath();
    /* Liberando Memoria Alocada no algoritmo */
    liberaMaxHeap(openMaxHeap);
    liberaClosedList(closedList);
    return path;
}

/*
    Robot System
*/

/*
    Calcula a direcao que o robot tem de seguir para chegar de um ponto a, a um ponto b
*/
int calculaDirecao(Ponto pontoInicial, Ponto pontoProximo){
    int xdif, ydif;
    xdif = pontoProximo.x - pontoInicial.x;
    xdif = pontoProximo.y - pontoInicial.y;
    /* NOROESTE (-1, -1) */
    if(xdif == -1 && ydif == -1){
        return 0;
    }
    /* NORTE (-1, 0) */
    else if(xdif == -1 && ydif == 0){
        return 1;
    }
    /* NORDESTE (-1, 1) */
    else if(xdif == -1 && ydif == 1){
        return 2;
    }
    /* LESTE (0, 1) */
    else if(xdif == 0 && ydif == 1){
        return 3;
    }
    /* SUDESTE (1, 1) */
    else if(xdif == 1 && ydif == 1){
        return 4;
    }
    /* S (1, 0) */
    else if(xdif == 1 && ydif == 0){
        return 5;
    }
    /* SO (1, -1) */
    else if(xdif == 1 && ydif == -1){
        return 6;
    }
    /* O (0, -1) */
    else if(xdif == 0 && ydif == -1){
        return 7;
    }
    /* Erro */
    else{
        return -1;
    }
}

/*
    Rotaciona o Robot para a direcao corrta
*/
void rotaciona(Robot *robot, Ponto proximoPonto){
    int direcaoAux, rotacaoHoraria, rotacaoAntiHoraria;
    int novaDirecao = calculaDirecao(robot->atual, proximoPonto);
    /* 0->NO, 1->N, 2->NE, 3->L, 4->SE, 5->S, 6->SO, 7->O */
    /* Se o Robot ja esta na direcao correta, nao rotacionamos */
    if(robot->direcao == novaDirecao){
        return;
    }
    /* Tentativa de Rotacao Sentido Horario */
    for(int i=0; i<5; i++){
        direcaoAux = robot->direcao + i;
        if(direcaoAux >7){
            direcaoAux -= 8;
        }
        if(direcaoAux == novaDirecao){
            rotacaoHoraria = i;
        }
    }
    /* Tentativa de Rotacao Sentido Antihorario */
    for(int j=0; j<4; j++){
        direcaoAux = robot->direcao - j;
        if(direcaoAux < 0){
            direcaoAux += 8;
        }
        if(direcaoAux == novaDirecao){
            rotacaoAntiHoraria = j;
        }
    }
    // Rotacionar o menor possivel
    if(rotacaoAntiHoraria < rotacaoHoraria){
        /* Rotacionar N=rotacaoAntiHoraria vezes de maneira AntiHoraria */
        rotacionaAntihorario(robot, rotacaoAntiHoraria);
        robot->direcao = novaDirecao;
        return;
    }else{
        /* Rotacioanr N=rotacaoHoraria vezes de maneira Horaria */
        rotacionaHorario(robot, rotacaoHoraria);
        robot->direcao = novaDirecao;
        return;
    }
}

/*
    Percorre os caminhos
*/
void percorrePath(Robot *robot, Mapa *mapa, ListaPath *listaPath){
    /* Robot deve caminhar de ponto a ponto rotacionando quando necessario */
    for(int i=0; i<listaPath->tamanho; i++){
        Serial.println("Movimentacao da Rota ");
        Serial.print(i);
        Serial.print(" (custo: ");
        Serial.print(listaPath->paths[i].custo);
        Serial.print(")");
        for(int j = listaPath->paths[i].tamanho-1; j>=0; j--){
            rotaciona(robot, listaPath->paths[i].path[j]);
            moveFrente(robot);
            robot->atual = listaPath->paths[i].path[j];
            mapa->mapa[robot->atual.x][robot->atual.y] = -9;
        }
    }
}

/*
    Move o Robot para frente
*/
void moveFrente(Robot *robot){
    /* Acionar Motores e andar um bloco */
    return;
}

/*
    Move o Robot para tras
*/
void moveTras(Robot *robot){
    /* Acionar Motores e andar um bloco */
    return;
}

/*
    Rotaciona na direcao horaria
*/
void rotacionaHorario(Robot *robot, int quantidade){
    /* rotaciona n=quantidade vezes */
    /* ativar APENAS o motor *DIREITO* */
} 

/*
    Rotaciona na direcao antihoraria
*/
void rotacionaAntihorario(Robot *robot, int quantidade){
    /* rotaciona n=quantidade vezes */
    /* ativar APENAS o motor *ESQUERDO* */
}

/* Setup - Pre Processamento */
void setup() {
    
    /* Serial Monitor - Monitoramento Serial, utilizado para outputs no ardunio */
    Serial.begin(9600);
    
    while (!Serial); /* Aguardar pela porta serial */
    /*
    Serial.println("Inicio");
    
    // Definicao do Pin do Micro SD
    pinMode(SDFILE_PIN_CS, OUTPUT);
    
    // Verifica se o cartao pode ser inicializado
    if (!SD.begin()) {
      Serial.println(F("Cartao falhou ou nao esta inserido"));
    }
    else{ // O Robo DEPENDE de sua memoria, sem ela, nada funciona, e nem pode funcionar
    Serial.println(F("Cartao inicializado."));
    
    // Inicializa Mapa
    Mapa *mapa = initMapa();
    if(!mapa){
      Serial.println("Erro inicializando mapa.");
      return;
    }
    // Visibilidade
    Visibilidade *visibilidade = initVisibilidade();
    processamentoVisibilidade(mapa, visibilidade);

    // Path Planning
    ListaPath * listaPath = initListaPath(visibilidade, mapa);
    
    // Grafico
    setPath(mapa,listaPath);
    setGuardas(mapa, visibilidade);
    printMapa(mapa);

    // Robot System
    Robot *robot = initRobot();
    percorrePath(robot, mapa, listaPath);
    
    // Liberando Memoria Alocada
    liberaMapa(mapa);
    liberaVisibilidade(visibilidade);
    liberaListaPath(listaPath);
    liberaRobot(robot);
    }
                    */

    
    // MOTOR DIREITO - MOVE PRA FRENTE
    dcMotorDriverL298.setMotorA(255,1);
    delay(400); // 1 transicao - antihorario

    dcMotorDriverL298.stopMotors();

    delay(1000);
    // MOTOR ESQUERDO - MOVE PRA FRENTE
    dcMotorDriverL298.setMotorB(255, 0);
    delay(400); // 1 transicao - horario
    
    dcMotorDriverL298.stopMotors();
    
    /*
    // 2WD -> Frente
    dcMotorDriverL298.setMotorA(255,1);
    dcMotorDriverL298.setMotorB(255,0);
    delay(200); // 10 cm
    //Stop both motors
    dcMotorDriverL298.stopMotors();
    */
    
    Serial.println("- - - - - - - - - - - - - - FIM - - - - - - - - - - - - - - ");
}

void loop(){
  /* for what? */
}
