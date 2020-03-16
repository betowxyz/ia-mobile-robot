/*
    Robot System
*/

int calculaDirecao(Ponto a, Ponto b); // Calcula a direcao que o robot tem de seguir para chegar de um ponto a, a um ponto b
void rotaciona(Robot *robot, Ponto proximoPonto, int novaDirecao); // Rotaciona o Robot para a direcao correta
void percorrePath(Robot *robot, Mapa *mapa, ListaPath *listaPath); // Percorre os caminhos do mapa

/*
    Funcoes de movimentacao
*/
void moveFrente(Robot *robot); // Move p/ Frente
void moveTras(Robot *robot); // Move p/ Tras
void rotacionaHorario(Robot *robot, int quantidade); // Rotaciona na direcao horaria
void rotacionaAntihorario(Robot *robot, int quantidade); // Rotaciona na direcao antihoraria

// Direcoes
//  NO(0)   N(1)    NE(2)
//  O(7)    *       L(3)
//  SO(6)   S(5)   SE(4)
