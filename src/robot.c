/**
    robotAutomation.c
    @author: Roberto Marinheiro
    @version: 6.1 12/09/19
    Purpose: Sistema de Veiculo Autonomo, dado um Mapa e a localizacao inicial do robo,
    definir os pontos tais que, se o robo visitar todos estes, ele tera visibilidade total do Mapa
*/

/*
    INCLUDE
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

/*
    HEADERS
*/
#include "general.h" // M0 - General
#include "visibilidade.h" // M1 - Visibilidade
#include "pathPlanning.h" // M2 - Path Planning

/*
    CONSTANTES DE CONTROLE
*/

// Guardas
#define MAXGUARDA (100000)
// Ordenacao dos pontos de guarda
#define MAXCUSTO (1000000)
// Path
#define MAXPATH (100000)
// Heap
#define MAXHEAPSIZE (100000)
// ClosedList
#define MAXCLOSEDLIST (100000)

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
    int i, a, aa;
    Mapa *mapa=(Mapa*)calloc(1,sizeof(Mapa));
    Ponto ponto;
    // FORMATO DE LEITURA:
        // ALTURA
        // LARGURA
        // MAPA
        // PONTO DE INICIO
    FILE *file = fopen("mapa90x90_1.txt","r");
    if(file == NULL){
        perror("\nErro abrindo arquivo de entrada do mapa...");
        return NULL;
    }
    // Recebendo Altura e Largura
    fscanf(file,"%d", &mapa->altura);
    fscanf(file,"%d", &mapa->largura);
    // Alocando
    mapa->mapa=(int**)calloc(mapa->altura, sizeof(int*));
    for(i; i<mapa->altura; i++){
        mapa->mapa[i] = (int*)calloc(mapa->largura ,sizeof(int));
    }
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

/*
    Inicializa a estrutura visibilidade
*/
Visibilidade * initVisibilidade(){
    Visibilidade *visibilidade=(Visibilidade*)calloc(1,sizeof(Visibilidade));
    visibilidade->pontos=(Ponto*)calloc(MAXGUARDA,sizeof(Ponto*)); // Numero maximo de pontos de guarda: MAXGUARDA
    visibilidade->completoVisivel = 0;
    visibilidade->quantidade = 0;
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
    Path *path, *pathOtimo;
    int i, j, index;
    Ponto aux;
    float menorCusto;
    // Loop para cada ponto
    for(i=0; i<visibilidade->quantidade-1; i++){
        menorCusto = MAXCUSTO;
        for(j=i+1; j<visibilidade->quantidade; j++){
            path = aStar(visibilidade->pontos[i], visibilidade->pontos[j], mapa);
            if(path->custo < menorCusto){
                pathOtimo = path;
                index = j;
                menorCusto = path->custo;
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

// FUNCOES GRAFICAS

/*
    Printa o mapa
*/
void printMapa(Mapa *mapa){
    printf("\n- - - - - - - - - - - - - - MAPA - - - - - - - - - - - - - - ");
    int a, aa;
    for(a=0; a<mapa->altura; a++){
        printf("\n");
        for(aa=0; aa<mapa->largura; aa++){
            if(mapa->mapa[a][aa]==2) printf(" -"); // LIVRE - COM VISAO
            else if(mapa->mapa[a][aa]==-5) printf(" o"); // LIVRE - PARTE DA ROTA
            else if(mapa->mapa[a][aa]==1) printf(" #"); // CHEIO - OBSTACULO
            else if(mapa->mapa[a][aa]==0) printf("  "); // LIVRE - SEM VISAO
            else if(mapa->mapa[a][aa]>9) printf(" %d", mapa->mapa[a][aa]-10); // LIVRE - PONTO DE GAURDA
        }
    }
    printf("\n\nLegenda: \n' ' = Sem visao (Livre)\n'#' = Obstaculo\n'-' = Com Visao (Livre)\n'0-N' = Guarda (Livre)\n'o' = Rota (Livre)\n");
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

// FUNCOES GERAIS

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
    Exporta mapa para arquivo externo output.txt (Formato numpy)
*/
void exportaMapa(Mapa *mapa){
    FILE *f = fopen("output.txt", "r+");
    int a, aa, numeroGuarda;
    if (f == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    for(a=0; a<mapa->altura; a++){
        fputs("[", f);
        for(aa=0; aa<mapa->largura; aa++){
            if(mapa->mapa[a][aa]==0) fputs("  0,", f); // LIVRE - SEM VISAO
            if(mapa->mapa[a][aa]==2) fputs(" 0,", f); // LIVRE - COM VISAO
            if(mapa->mapa[a][aa]==1) fputs(" -255,", f); // CHEIO - OBSTACULO
            if(mapa->mapa[a][aa]==-5) fputs(" 200,", f); // LIVRE - PARTE DA ROTA
            if(mapa->mapa[a][aa]>9) fputs(" 400,", f); // LIVRE - PONTO DE GAURDA
        }
    fputs("],\n", f);
    }
    fclose(f);
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
    int i;
    for(i=0; i<mapa->altura; i++)
        free(mapa->mapa[i]);
    free(mapa->mapa);
    free(mapa);
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
    for(int i=0; i<closedList->tamanho; i++){
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
    for(int i=0; i<listaPath->tamanho; i++){
        free(listaPath->paths[i].path);
        }
    free(listaPath->paths);
    free(listaPath);
}

/*
    Liberamento de Memoria da MaxHeap
*/
void liberaMaxHeap(PriorityQueue H) {
    for(int i=0; i<H->tamanho; i++){
        if(H->Elements[i].parente){
            free(H->Elements[i].parente);
        }
    }
    free(H->Elements);
    free(H);
}

/*
    M1 VISIBILIDADE
*/

/*
    Faz o processamento da visibilidade disparando raio para todas as direcoes e selecionando novos pontos de guarda
*/
int processamentoVisibilidade(Mapa *mapa, Visibilidade *visibilidade){
    int i, j;
    Ponto proximoPonto = mapa->inicio;
    while(visibilidade->completoVisivel==0){
        visibilidade->pontos[visibilidade->quantidade] = proximoPonto;
        visibilidade->quantidade++;
        // DOIS LOOPS: UM PARA ATIRAR NO SENTIDO VERTICAL, E OUTRO NO HORIZONTAL
        for(i=0; i<=mapa->largura; i++){ // SENTIDO VERTICAL
            raio(proximoPonto.x, proximoPonto.y, 0, i, mapa); // CIMA -> 0, I (ITERAR ATE LARGURA)
            raio(proximoPonto.x, proximoPonto.y, mapa->altura, i, mapa); // BAIXO -> ALTURA, I (ITERAR ATE LARGURA)
        }
        for(j=0; j<=mapa->altura; j++){ // SENTIDO HORIZONTAL
            raio(proximoPonto.x, proximoPonto.y, j, mapa->largura, mapa); // DIREITA -> J, LARGURA (ITERAR ATE ATLURA)
            raio(proximoPonto.x, proximoPonto.y, j, 0, mapa); // ESQUERDA -> J, 0 (ITERAR ATE ALTURA)
        }
        proximoPonto = catchNext(mapa);
        if(proximoPonto.x == -1) visibilidade->completoVisivel = 1;
    }
    return 0;
}

/*
    Cria o raio a ser disparado para verificar a visibilidade
*/
void raio(int x0, int y0, int x1, int y1, Mapa *mapa) { // ~BRESEHANS LINE ALGORITHM~
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;
    int dist=0;
    for(;;){
        dist++; // Podemos usar esse dist
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
    // Propriedades de nodes vizinhos
    int numeroVizinhos = 8;
    int novaPosicaoX[8]= {0, 0, 1, -1, 1, 1, -1, -1};
    int novaPosicaoY[8]= {1, -1, 0, 0, 1, -1, 1, -1};
    // Variaveis de controle (loop)
    int i, j, k;
    // Variaveis de controle de busca das listas
    bool vizinhoOpen, vizinhoClosed;
    // Tentativa de novo Score
    float tentativaG;
    // Criando nodes para inicio, objetivo, atual e vizinho
    Node inicioNode = initNode(inicio);
    Node objetivoNode = initNode(objetivo);
    Node atual;
    // Criando no VIZINHO
    Node vizinho = initEmptyNode();
    // Inicializar lista aberta e fechada
    PriorityQueue openMaxHeap = initMaxHeap();
    ClosedList * closedList = initClosedList();
    // Inserir ponto de inicio a lista aberta
    pushMaxHeap(openMaxHeap, inicioNode);
    // Loop ate encontrar o objetivo
    while(openMaxHeap->tamanho){
        // Retirar atualNode da openMaxHeap e adicionar a listaFechada
        atual = popMaxHeap(openMaxHeap);
        // Encontrou o objetivo
        if(atual.posicao.x == objetivoNode.posicao.x && atual.posicao.y == objetivoNode.posicao.y){
            Path *path = initPath();
            path->custo = atual.g;
            while(atual.parente != NULL){
                pushPath(path, atual.posicao);
                atual = *atual.parente;
            }
            pushPath(path, atual.posicao);
            // Liberando Memoria alocada pelas estruturas
            liberaMaxHeap(openMaxHeap);
            liberaClosedList(closedList);
            // Retornando
            return path;
        }
        // Inserindo no atual na lista fechada (Ou seja, ele ja foi visitado/expandido)
        pushClosedList(closedList, atual);
        // Gerando celulas filhas ((0, -1), (0, 1), (-1, 0), (1, 0), (-1, -1), (-1, 1), (1, -1), (1, 1))
        for(i=0; i<numeroVizinhos; i++){
            // Pegando as novas posicoes
            vizinho.posicao.x = atual.posicao.x + novaPosicaoX[i]; // X
            vizinho.posicao.y = atual.posicao.y + novaPosicaoY[i]; // Y
            // Calcular novo custo
            tentativaG = atual.g + heuristica(atual.posicao, vizinho.posicao);
            // Verficiar se nao sao "out of bounds", se for o caso, apenas passa para a proxima iteracao
            if(vizinho.posicao.x > mapa->altura || vizinho.posicao.x < 0 || vizinho.posicao.y > mapa->largura || vizinho.posicao.y < 0){
                continue;
            }
            // Verificar se nao eh parede, se for o caso, apenas passa para a proxima iteracao
            if(mapa->mapa[vizinho.posicao.x][vizinho.posicao.y] == 1){
                continue;
            }
            // Variavel vizinhoClosed para verificar se o no esta na lista fechada
            vizinhoClosed = false;
            // Verificar se o vizinho esta na lista fechada
            for(j=0; j <= closedList->tamanho; j++){
                if(closedList->nodes[j].posicao.x == vizinho.posicao.x && closedList->nodes[j].posicao.y == vizinho.posicao.y){
                    vizinhoClosed = true;
                    break;
                    }
                }
            // Se vizinho esta na lista fechada e tentativa G >= custo G do vizinho, apenas passa para proxima iteracao
            if(vizinhoClosed && tentativaG >= vizinho.g){
                continue;
            }
            // Variavel vizinhoOpen para verificar se o no esta na lista aberta
            vizinhoOpen = false;
            // Verificar se o vizinho esta na lista aberta
                for(k=0; k<=openMaxHeap->tamanho; k++){
                    if(openMaxHeap->Elements[k].posicao.x == vizinho.posicao.x && openMaxHeap->Elements[k].posicao.y == vizinho.posicao.y){
                        vizinhoOpen = true;
                        break;
                    }
                }
            // Verificar se o custo da tentativa eh menor que o antigo
            if(tentativaG < vizinho.g || !vizinhoOpen){ // O problema esta nessa associacao de ponteiro
                Node * aux = initNodePointer(atual); // Desalocamos todos os parentes nas funcoes de liberamento de memoria da MaxHeap & da ClosedList
                vizinho.parente = aux;
                vizinho.g = tentativaG;
                vizinho.f = tentativaG + heuristica(vizinho.posicao, objetivoNode.posicao);
                pushMaxHeap(openMaxHeap, vizinho);
            }
        }
    }
    // Nao encontrou caminho, retorna caminho vazio
    Path *path = initPath();
    // Liberando Memoria Alocada no algoritmo
    liberaMaxHeap(openMaxHeap);
    liberaClosedList(closedList);
    return path;
}

/*
    Funcao Principal
*/
int main(){
    // M1
    Mapa *mapa = initMapa();
    if(mapa == NULL) return 0;
    Visibilidade *visibilidade = initVisibilidade();
    processamentoVisibilidade(mapa, visibilidade);
    printGuardas(visibilidade);
    // M2
    ListaPath * listaPath = initListaPath(visibilidade, mapa);

    // Grafico
    setPath(mapa, listaPath);
    setGuardas(mapa, visibilidade);
    printMapa(mapa);
    exportaMapa(mapa);
    
    // Libera
    liberaMapa(mapa);
    liberaVisibilidade(visibilidade);
    liberaListaPath(listaPath);

    // 0
    return 0;
}
