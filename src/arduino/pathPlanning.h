#pragma once
/*

    Declaracao das funcoes do modulo PATH FINDING (2)

*/

float heuristica(Ponto a, Ponto b); // Calculo da heuristica de Manhattan
Path * aStar(Ponto inicio, Ponto objetivo, Mapa *mapa); // Algoritmo de Path Finding