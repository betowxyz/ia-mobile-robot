#pragma once
/*

    Declaracao de estruturas

*/

/*
    Ponto: coordenadas x e y
*/
typedef struct{
    int x, y;
    }Ponto;

/*
    Mapa: armazeana matrix, onde o mapa eh alocado
*/
typedef struct{
    int **mapa; // Array 2D que carrega o Mapa
    int altura, largura; // altura -> N, largura -> M
    Ponto inicio; // Ponto de inicio do Robo
}Mapa;

/*
    Visibilidade: armazeana as propriedades da visibilidade do robo
*/
typedef struct{
    Ponto *pontos; // Lista de pontos necessarios para total visao do mapa
    int completoVisivel, quantidade; // CompletoVisivel -> Indicativo se o robo tem total visao do mapa (0 = NAO/FALSE, 1 = SIM/TRUE), quantidade -> tamanho da lista pontos
}Visibilidade;

/*
Node: utilizada para armazenar valores referentes a cada No no algoritmo A Star
*/
typedef struct _Node{
    float g, h, f;
    Ponto posicao;
    struct _Node *parente;
}Node;

/*
    ClosedList: lista de nos utilizada no A Star
*/
typedef struct{
    Node *nodes;
    int tamanho;
}ClosedList;

/*
    Path: armazeana o Path produzido pelo A Star
*/
typedef struct{
    Ponto *path;
    int tamanho;
    float custo;
}Path;

/*
    ListaPath: armazeana os Paths produzidos pelo A Star
*/
typedef struct{
    Path *paths, *pathsAux;
    int tamanho;
}ListaPath;

/*
    MaxHeap: Lista de prioridade - chamada sempre por (PRIORITY QUEUE)
*/
typedef struct HeapStruct *PriorityQueue;
struct HeapStruct{
    int capacidade;
    int tamanho;
    Node *Elements;
};

/*
    Robot: variaveis de controle do robo
    Direcoes:
    NO(0)   N(1)    NE(2)
    O(7)    *       L(3)
    SO(6)   S(5)   SE(4)
*/
typedef struct{
    Ponto inicio, atual;
    int direcao;
    float comprimento, largura;
}Robot;

/*

    Definicao das funcoes

*/

/*
INICIALIZACAO
*/
Ponto initPonto(int x, int y); // Inicializacao da estrutura PONTO
Mapa * initMapa(); // Inicializacao da estrutura MAPA com leitura do arquivo externo "inputMap.txt"
Visibilidade * initVisibilidade(); // Inicializacao da estrutura VISIBILIDADE
Node initNode(Ponto posicao); // Inicializacao da estrutura NODE
Node initEmptyNode(); // Inicializacao da estrutura NODE vazio
Node * initNodePointer(Node node); // Inicializacao da estrutura NODE como ponteiro a partir de um node existente
ClosedList * initClosedList(); // Inicializacao da estrutura CLOSEDLIST
Path * initPath(); // Inicializacao da estrutura PATH
ListaPath * initListaPath(Visibilidade *visibilidade, Mapa *mapa); // Inicializa a estrutura LISTA PATH e planeja repetidamente as rotas entre PN e proximo ponto mais proximo
PriorityQueue initMaxHeap(); // Inicializacao da estrutura MAXHEAP
Robot * initRobot(); // Initialize Robot

/*
    GRAFICA
*/
void printMapa(Mapa *mapa); // Printa o mapa no console
void printMapaAbsoluto(Mapa *mapa); // Printa o mapa - Valores absolutos
void printGuardas(Visibilidade *visibilidade); // Printa os pontos necessarios para total visibiliade
void printListaPath(ListaPath *listaPath); // Printa as rotas geradas pelo PlanejamentoMestre & AStar
void printPonto(Ponto ponto); // Printa coordenadas do ponto
void printDirecao(int direcao); // Printa direcao correspondente ao numero

/*
    GERAL
*/
void setGuardas(Mapa *mapa, Visibilidade *visibilidade); // Posiciona os guardas no mapa como "8"
void setPath(Mapa *mapa, ListaPath *listaPath); // Posiciona os pontos de ROTA no mapa
Ponto catchNext(Mapa *mapa); // Pesquisa pelo proximo ponto sem visibilidade do Robo

/*
    ESTRUTURAIS
*/
void pushClosedList(ClosedList *closedList, Node node); // Adiciona No a closedList
void pushPath(Path *path, Ponto ponto); // Adiciona o ponto ao Path
bool maxHeapVazia(PriorityQueue H); // Testa se a Heap esta vazia
bool maxHeapCheia(PriorityQueue H); // Testa se a Heap esta cheia
void pushMaxHeap(PriorityQueue H, Node X); // Inserir Nos a MaxHeap
Node popMaxHeap(PriorityQueue H); // Retira o elemento com maior prioridade (no caso, menor custo F do no) da MaxHeap e o retorna

/*
    LIBERAMENTO DE MEMORIA
*/
void liberaMapa(Mapa *mapa); // Libera memoria armazenada pela estutura MAPA
void liberaVisibilidade(Visibilidade *visibilidade); // Libera memoria armazenada pela estutura VISIBILIDADE
void liberaClosedList(ClosedList *closedList); // Libera memoria armazenada pela estrutura ClosedList
void liberaListaPath(ListaPath *path); // Libera memoria armazenada pela estrutura LISTAPATH
void liberaMaxHeap(PriorityQueue H); // Libera memoria armazenada pela estrutura MAXHEAP
void liberaRobot(Robot *robot); // Libera memoria alocada pela estrutura Robot
void liberaPath(Path *path); // Libera memoria alocada pela estrutura Path