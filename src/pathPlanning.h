/*

    Declaracao das funcoes do modulo PATH FINDING (2)

*/

void ordenaGuardas(Visibilidade *visibilidade); // Ordena a lista de pontos de guarda de acordo com a heuristica
float heuristica(Ponto a, Ponto b); // Calculo da heuristica de Manhattan
Path * aStar(Ponto inicio, Ponto objetivo, Mapa *mapa); // Algoritmo de Path Finding