/*

    Declaracao das funcoes do modulo VISIBILIDADE (1)

*/

int processamentoVisibilidade(Mapa *mapa, Visibilidade *visibilidade); // Calcula os pontos necessarios para visao total do mapa
int processamentoVisibilidadePlot(Mapa *mapa, Visibilidade *visibilidade); // Calcula os pontos necessarios para visao total do mapa - FUNCAO PROCESSAMENTOVISIBILIDADE MODIFICADA PARA PLOTS -
void raio(int xa, int ya, int xb, int yb, Mapa *mapa); // Cria raio verificando a visibilidade do ponto X ao ponto Y
void raioPlot(int xa, int ya, int xb, int yb, Mapa *mapa, int z); // Cria raio verificando a visibilidade do ponto X ao ponto Y - FUNCAO RAIO MODIFICADA PARA PLOTS -