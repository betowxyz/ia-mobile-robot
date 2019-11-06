#pragma once
/*

    Declaracao das funcoes do modulo VISIBILIDADE (1)

*/

int processamentoVisibilidade(Mapa *mapa, Visibilidade *visibilidade); // Calcula os pontos necessarios para visao total do mapa
void raio(int xa, int ya, int xb, int yb, Mapa *mapa); // Cria raio verificando a visibilidade do ponto X ao ponto Y
