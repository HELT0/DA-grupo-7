//
// Created by Elton on 3/28/2026.
//

#ifndef PROJETO_ALGORITHMS_H
#define PROJETO_ALGORITHMS_H

#include "../data_structures/Graph.h"
#include <queue>
#include <algorithm>
#include <limits>
#include <string>

/**
 * @brief Função auxiliar para testar e visitar um nó adjacente.
 * * Esta função verifica se um nó ainda não foi visitado e se existe capacidade residual
 * suficiente para passar fluxo. Se ambas as condições se verificarem, o nó é marcado
 * como visitado, guarda a aresta pela qual foi alcançado e é colocado na fila da BFS.
 * * @tparam T Tipo de dado armazenado no vértice.
 * @param q Fila (queue) utilizada pela BFS.
 * @param e Aresta que liga o nó atual ao nó destino.
 * @param w Nó destino a ser testado.
 * @param residual Capacidade residual atual da aresta.
 * * @par Time Complexity
 * \f$\mathcal{O}(1)\f$ - A verificação e inserção na fila operam em tempo constante.
 */
template <class T>
void testAndVisit(std::queue<Vertex<T>*> &q, Edge<T> *e, Vertex<T> *w, double residual) {
    if (!w->isVisited() && residual > 0) {
        w->setVisited(true);
        w->setPath(e);
        q.push(w);
    }
}

/**
 * @brief Procura um caminho aumentante (Augmenting Path) no grafo residual.
 * * Utiliza o algoritmo de Busca em Largura (BFS - Breadth-First Search) para encontrar
 * o caminho mais curto (em número de arestas) desde o nó origem (source) até ao nó destino (sink)
 * no grafo residual.
 * * @tparam T Tipo de dado armazenado no vértice.
 * @param g Apontador para o grafo.
 * @param s Vértice de origem (Source).
 * @param t Vértice de destino (Sink).
 * @return true Se encontrou um caminho aumentante até ao destino.
 * @return false Se não encontrou nenhum caminho (o fluxo máximo foi atingido).
 * * @par Time Complexity
 * \f$\mathcal{O}(V + E)\f$ no pior cenário, onde \f$V\f$ é o número de vértices e \f$E\f$ é o número de arestas.
 */
template <class T>
bool findAugmentingPath(Graph<T> *g, Vertex<T> *s, Vertex<T> *t) {
    for(auto v : g->getVertexSet()) {
        v->setVisited(false);
    }
    std::queue<Vertex<T>*> q;
    s->setVisited(true);
    q.push(s);

    while (!q.empty()) {
        auto u = q.front(); q.pop();

        for (auto e : u->getAdj()) {
            auto v = e->getDest();
            double residual = e->getWeight() - e->getFlow();
            testAndVisit(q, e, v, residual);
        }

        for (auto e : u->getIncoming()) {
            auto v = e->getOrig();
            double residual = e->getFlow();
            testAndVisit(q, e, v, residual);
        }
    }
    return t->isVisited();
}

/**
 * @brief Calcula a capacidade de gargalo (bottleneck) de um caminho aumentante.
 * * Percorre de trás para a frente o caminho encontrado pela BFS (seguindo os ponteiros `path`),
 * e descobre o valor mínimo de capacidade residual entre todas as arestas que compõem o caminho.
 * * @tparam T Tipo de dado armazenado no vértice.
 * @param s Vértice de origem (Source).
 * @param t Vértice de destino (Sink).
 * @return double O valor do estrangulamento (fluxo máximo que pode passar por este caminho específico).
 * * @par Time Complexity
 * \f$\mathcal{O}(V)\f$, onde \f$V\f$ é o número de vértices, pois o caminho mais longo possível num grafo
 * não tem ciclos e percorre no máximo \f$V-1\f$ arestas.
 */
template <class T>
double findMinResidualAlongPath(Vertex<T> *s, Vertex<T> *t) {
    double f = INF;
    Vertex<T>* curr = t;
    while (curr != s) {
        auto e = curr->getPath();
        // Verifica se a aresta é direta ou inversa para calcular o residual corretamente
        double residual = (e->getDest() == curr) ? (e->getWeight() - e->getFlow()) : e->getFlow();
        f = std::min(f, residual);
        curr = (e->getDest() == curr) ? e->getOrig() : e->getDest();
    }
    return f;
}

/**
 * @brief Atualiza o fluxo de todas as arestas pertencentes ao caminho aumentante encontrado.
 * * Percorre o caminho encontrado e atualiza o fluxo com base no valor de gargalo fornecido.
 * Se a aresta for atravessada no seu sentido original, o fluxo é somado. Se for atravessada
 * no sentido inverso (aresta residual), o fluxo é subtraído.
 * * @tparam T Tipo de dado armazenado no vértice.
 * @param s Vértice de origem (Source).
 * @param t Vértice de destino (Sink).
 * @param f O valor de fluxo a incrementar/decrementar (valor de gargalo calculado previamente).
 * * @par Time Complexity
 * \f$\mathcal{O}(V)\f$, pelos mesmos motivos da função `findMinResidualAlongPath`.
 */
template <class T>
void augmentFlowAlongPath(Vertex<T> *s, Vertex<T> *t, double f) {
    Vertex<T>* cur = t;
    while (cur != s) {
        auto e = cur->getPath();
        if (e->getDest() == cur) {
            e->setFlow(e->getFlow() + f);
            cur = e->getOrig();
        } else {
            e->setFlow(e->getFlow() - f);
            cur = e->getDest();
        }
    }
}

/**
 * @brief Algoritmo Edmonds-Karp para calcular o Fluxo Máximo.
 * * Este é o algoritmo principal. Inicializa o fluxo do grafo a zero e, iterativamente,
 * procura caminhos aumentantes através da BFS, atualizando o fluxo máximo da rede até
 * não existirem mais caminhos possíveis entre a origem e o destino.
 * * @tparam T Tipo de dado armazenado no vértice.
 * @param g Apontador para o grafo da conferência.
 * @param sourceInfo Informação (ID) do nó de origem (ex: "SOURCE").
 * @param targetInfo Informação (ID) do nó de destino (ex: "SINK").
 * * @par Time Complexity
 * \f$\mathcal{O}(V \cdot E^2)\f$, onde \f$V\f$ é o número de vértices e \f$E\f$ é o número de arestas.
 * O algoritmo encontra no máximo \f$\mathcal{O}(V \cdot E)\f$ caminhos aumentantes, e a busca
 * de cada caminho custa \f$\mathcal{O}(E)\f$ com a BFS.
 */
template <class T>
void edmondsKarp(Graph<T> *g, T sourceInfo, T targetInfo) {
    Vertex<T>* s = g->findVertex(sourceInfo);
    Vertex<T>* t = g->findVertex(targetInfo);

    // SEGURANÇA: Se a fonte ou o poço não existirem, paramos aqui
    if (s == nullptr || t == nullptr) {
        return;
    }

    // Inicializa o fluxo de todas as arestas a 0 antes de começar
    for (auto v : g->getVertexSet()) {
        for (auto e : v->getAdj()) {
            e->setFlow(0);
        }
    }

    // Enquanto houver caminho aumentante, o fluxo continua a ser somado
    while (findAugmentingPath(g, s, t)) {
        double f = findMinResidualAlongPath(s, t);
        augmentFlowAlongPath(s, t, f);
    }
}

#endif //PROJETO_ALGORITHMS_H