//
// Created by tomas on 28/03/2026.
//

#ifndef PROJETO_BACKEND_H
#define PROJETO_BACKEND_H

#include <string>
#include <vector>
#include "../data_structures/Graph.h" // Certifica-te que o caminho esta correto para a tua estrutura de pastas

/**
 * @brief Estrutura que representa uma submissao de um artigo cientifico.
 * Armazena as informacoes lidas do ficheiro CSV relativas a cada paper.
 */
struct Submission {
    int id;
    std::string title;
    std::string authors;
    std::string email;
    int primaryDomain;
    int secondaryDomain;
};

/**
 * @brief Estrutura que representa um revisor da conferencia.
 * Armazena os dados pessoais e as areas de pericia (primaria e secundaria).
 */
struct Reviewer {
    int id;
    std::string name;
    std::string email;
    int primaryExpertise;
    int secondaryExpertise;
};

/**
 * @brief Estrutura auxiliar usada durante a exportacao dos resultados.
 * Associa uma submissao a um revisor e guarda o dominio de match.
 */
struct AssignRecord {
    int subId;
    int revId;
    int matchDomain;
};

/**
 * @brief Estrutura auxiliar para registar submissoes que nao obtiveram revisoes suficientes.
 */
struct MissingRecord {
    int subId;
    int domain;
    int missing;
};

/**
 * @brief Estrutura que armazena as configuracoes e parametros globais do programa.
 * Estes parametros ditam as regras do fluxo maximo e da analise de risco.
 */
struct Config {
    int minReviewsPerSubmission = 0;
    int maxReviewsPerReviewer = 0;
    int primaryReviewerExpertise = 0;
    int secondaryReviewerExpertise = 0;
    int primarySubmissionDomain = 0;
    int secondarySubmissionDomain = 0;
    int generateAssignments = 0;
    int riskAnalysis = 0;
    std::string outputFileName = "output.csv";
};

// ==========================================
// VARIAVEIS GLOBAIS (Declaradas como extern)
// ==========================================
extern std::vector<Submission> globalSubs;
extern std::vector<Reviewer> globalRevs;
extern Config globalConfig;
extern Graph<std::string> conferenceGraph;
extern std::vector<int> globalRiskyReviewers;

// ==========================================
// ASSINATURAS DAS FUNCOES (Implementadas no backend.cpp)
// ==========================================

// Funcao do Parser (Leitura e Construcao do Grafo)
bool loadInputAndBuildGraph(const std::string& filename);

// Funcoes Principais de Atribuicao e Risco
void runMaxFlowAssignment();
void runRiskAnalysis();

// Funcoes de Exportacao e Debug
void exportResults(const std::string& filename);
void displayGraph();

#endif //PROJETO_BACKEND_H