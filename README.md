# Inteligência Artificial para Robô Móvel de Busca :shipit:

### Introdução
Inteligência Artifical que, a partir das entradas: Mapa, e ponto inicial do robô, defina os pontos necessários para o robô visitar, de tal forma que, se o robô passe por todos estes pontos e olhe ao seu redor (FoV 360º), ele alcance visibilidade de todo o mapa. A partir desses pontos gerados, é responsável por estabelecer a melhor rota entre os mesmos (começando do ponto inicial) e verificar com auxílio da ferramenta OpenCV, se a bola vermelha (objetivo) está ao seu alcance, caso esteja, ela traça a melhor rota até ela.

#### Processamento de Visibilidade
A primeira tarefa da IA é definir os pontos de guarda no mapa, para isto, utilizamos o algoritmo de Ray Shooting que basicamente, a partir de um ponto inicial, dispara raios para todas as direções verificando até onde o ponto inicial tem visão. O primeiro ponto é o ponto inicial do Robô Móvel (uma vez que o melhor cenário em questão de tempo e gasto, é a bola vermelha estar visível do deste ponto), calculamos a visibilidade deste ponto, e depois buscamos pelo próximo ponto que ainda não temos visão, e calculamos novamente a visibilidade neste, repetindo estas etapas até que nenhum ponto esteja "não visível". 

![](/img/visibilidade.png)

#### Path-Planning
Uma vez que temos os pontos de guarda necessários para total visibilidade, é necessário estabelecer as rotas entre estes pontos, porém, não podemos simplesmente traças estas rotas sem levar em conta as distâncias entre os mesmos, então planejamos as rotas entre os pontos de guarda X e 1, X e 2 ... X e N, até que encontremos o ponto com caminho mais próximo de X, repetindo esta etapa até planejarmos todas as rotas, de tal meneira, teremos a MENOR ROTA POSSÍVEL para visitar os pontos de guarda.

![](/img/astar.gif)

#### Machine Learning
Com os pontos de guarda e as rotas estabelecidas, a tarefa restante é verificar se a bola vermelha está ao alcance do Robô. Com a ferramenta OpenCV, detectamos apenas os pixels vermelhos das imagens e depois verificamos se estes pixels configuram um círculo.

## Visibilidade + A Star
Dado um mapa simulado e com nosso sistema de IA rodando, podemos visualizar os pontos de guarda e os caminhos entre eles:

![](/img/outputAtt.png)
