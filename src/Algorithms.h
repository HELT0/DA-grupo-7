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

// Função auxiliar para visitar um nó se houver capacidade residual
template <class T>
void testAndVisit(std::queue<Vertex<T>*> &q, Edge<T> *e, Vertex<T> *w, double residual) {
    if (!w->isVisited() && residual > 0) {
        w->setVisited(true);
        w->setPath(e);
        q.push(w);
    }
}

// Encontra um caminho aumentante usando BFS
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

// Calcula o gargalo (bottleneck) do caminho encontrado
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

// Atualiza o fluxo ao longo do caminho
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

// Implementação principal de Edmonds-Karp
// T representa o tipo de info do vértice (neste caso, std::string)
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