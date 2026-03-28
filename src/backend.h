//
// Created by tomas on 28/03/2026.
//

#ifndef PROJETO_BACKEND_H
#define PROJETO_BACKEND_H
#include <string>
#include <vector>
#include "../data_structures/Graph.h" // Descomenta quando tiveres o Graph.h

// Estruturas de Dados
struct Submission {
    int id;
    std::string title;
    std::string authors;
    std::string email;
    int primaryDomain;
    int secondaryDomain;
};

struct Reviewer {
    int id;
    std::string name;
    std::string email;
    int primaryExpertise;
    int secondaryExpertise;
};

// --- ESTRUTURAS AUXILIARES PARA A EXPORTAÇÃO ---
struct AssignRecord {
    int subId;
    int revId;
    int matchDomain;
};

struct MissingRecord {
    int subId;
    int domain;
    int missing;
};

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

// Variáveis Globais (Declaradas como extern)
extern std::vector<Submission> globalSubs;
extern std::vector<Reviewer> globalRevs;
extern Config globalConfig;
extern Graph<std::string> conferenceGraph;
extern std::vector<int> globalRiskyReviewers;

// Funções do Parser (implementadas no parser.cpp)
bool loadInputAndBuildGraph(const std::string& filename);

// Funções do Backend (implementadas no backend.cpp)
void runMaxFlowAssignment();
void runRiskAnalysis();
void exportResults(const std::string& filename);
void displayGraph();

#endif //PROJETO_BACKEND_H