# Inteligência Artificial para Robô Móvel de Busca :shipit:

### Introdução
Inteligência Artifical que, a partir das entradas: Mapa, e ponto inicial do robô, defina os pontos necessários para o robô visitar, de tal forma que, se o robô passe por todos estes pontos e olhe ao seu redor (FoV 360º), ele alcance visibilidade de todo o mapa. A partir desses pontos gerados, é responsável por estabelecer a melhor rota entre os mesmos (começando do ponto inicial) e verificar com auxílio da ferramenta OpenCV, se a bola vermelha (objetivo) está ao seu alcance, caso esteja, ela estabelece a melhor rota até ela.

#### Processamento de Visibilidade
A primeira tarefa da IA é definir os pontos de guarda no mapa, para isto, utilizamos o algoritmo de Ray Shooting que basicamente, a partir de um ponto inicial, dispara raios para todas as direções verificando até onde o ponto inicial tem visão. O primeiro ponto é o ponto inicial do Robô Móvel (uma vez que o melhor cenário em questão de tempo e gasto, é a bola vermelha estar visível do deste ponto), calculamos a visibilidade deste ponto, e depois buscamos pelo próximo ponto que ainda não temos visão, e calculamos novamente a visibilidade neste, repetindo estas etapas até que nenhum ponto esteja "não visível". 

#### Path-Planning
Uma vez que temos os pontos de guarda, necessários para total visibilidade, é necessário estabelecer as rotas dentre estes pontos, porém, não podemos simplesmente estabelecer as rotas sem levar em conta as distâncias entre estes pontos, então antes de tudo, ordenamos a lista de pontos de guarda de acordo com a distância entre o ponto de guarda N, e o ponto de guarda N+1. Feito isto...
..
#### Machine Learning
Machine Learning para detectar o objeto a partir de imagens provenientes dos pontos de guarda.
