#include "backend.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Algorithms.h"

// ==========================================
// 1. DEFINIÇÃO DAS VARIÁVEIS GLOBAIS
// ==========================================
std::vector<Submission> globalSubs;
std::vector<Reviewer> globalRevs;
Config globalConfig;
Graph<std::string> conferenceGraph; // Descomenta quando tiveres o Grafo
std::vector<int> globalRiskyReviewers;

// ==========================================
// 2. FUNÇÕES AUXILIARES DO PARSER
// ==========================================
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> splitCSVLine(const std::string& line) {
    std::vector<std::string> result;
    std::string currentToken;
    bool inQuotes = false;
    for (char c : line) {
        if (c == '\"') inQuotes = !inQuotes;
        else if (c == ',' && !inQuotes) {
            result.push_back(trim(currentToken));
            currentToken.clear();
        } else {
            currentToken += c;
        }
    }
    result.push_back(trim(currentToken));
    return result;
}

// ==========================================
// 3. O PARSER (Lê o CSV e Monta o Grafo)
// ==========================================
bool loadInputAndBuildGraph(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o ficheiro " << filename << std::endl;
        return false;
    }

    std::vector<std::string> nodesToDel;
    for (auto v : conferenceGraph.getVertexSet()) {
        nodesToDel.push_back(v->getInfo());
    }
    for (const auto& n : nodesToDel) {
        conferenceGraph.removeVertex(n);
    }

    globalSubs.clear();
    globalRevs.clear();

    enum State { NONE, SUBMISSIONS, REVIEWERS, PARAMETERS, CONTROL };
    State currentState = NONE;
    std::string line;

    // FASE A: LER O FICHEIRO
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line.find("#Id") == 0) continue;

        if (line == "#Submissions") { currentState = SUBMISSIONS; continue; }
        if (line == "#Reviewers")   { currentState = REVIEWERS; continue; }
        if (line == "#Parameters")  { currentState = PARAMETERS; continue; }
        if (line == "#Control")     { currentState = CONTROL; continue; }

        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos && line[0] != '#') {
            line = trim(line.substr(0, commentPos));
        }
        if (line.empty() || line[0] == '#') continue;

        std::vector<std::string> tokens = splitCSVLine(line);

        if (currentState == SUBMISSIONS && tokens.size() >= 5) {
            Submission s;
            s.id = std::stoi(tokens[0]);
            s.title = tokens[1];
            s.authors = tokens[2];
            s.email = tokens[3];
            s.primaryDomain = std::stoi(tokens[4]);
            s.secondaryDomain = (tokens.size() > 5 && !tokens[5].empty()) ? std::stoi(tokens[5]) : -1;
            globalSubs.push_back(s);
        }
        else if (currentState == REVIEWERS && tokens.size() >= 4) {
            Reviewer r;
            r.id = std::stoi(tokens[0]);
            r.name = tokens[1];
            r.email = tokens[2];
            r.primaryExpertise = std::stoi(tokens[3]);
            r.secondaryExpertise = (tokens.size() > 4 && !tokens[4].empty()) ? std::stoi(tokens[4]) : -1;
            globalRevs.push_back(r);
        }
        else if (currentState == PARAMETERS || currentState == CONTROL) {
            if (tokens.size() >= 2) {
                std::string key = tokens[0], val = tokens[1];
                if (key == "MinReviewsPerSubmission") globalConfig.minReviewsPerSubmission = std::stoi(val);
                else if (key == "MaxReviewsPerReviewer") globalConfig.maxReviewsPerReviewer = std::stoi(val);
                else if (key == "PrimaryReviewerExpertise") globalConfig.primaryReviewerExpertise = std::stoi(val);
                else if (key == "SecondaryReviewerExpertise") globalConfig.secondaryReviewerExpertise = std::stoi(val);
                else if (key == "PrimarySubmissionDomain") globalConfig.primarySubmissionDomain = std::stoi(val);
                else if (key == "SecondarySubmissionDomain") globalConfig.secondarySubmissionDomain = std::stoi(val);
                else if (key == "GenerateAssignments") globalConfig.generateAssignments = std::stoi(val);
                else if (key == "RiskAnalysis") globalConfig.riskAnalysis = std::stoi(val);
                else if (key == "OutputFileName") globalConfig.outputFileName = val;
            }
        }
    }
    file.close();

    // FASE B: CONSTRUIR O GRAFO
    if (globalConfig.generateAssignments == 0) return true;

    conferenceGraph.addVertex("SOURCE");
    conferenceGraph.addVertex("SINK");

    for (const auto& s : globalSubs) {
      std::string subNode = "S_" + std::to_string(s.id);
      conferenceGraph.addVertex(subNode);
      conferenceGraph.addEdge("SOURCE", subNode, globalConfig.minReviewsPerSubmission);
    }

    for (const auto& r : globalRevs) {
      std::string revNode = "R_" + std::to_string(r.id);
      conferenceGraph.addVertex(revNode);
      conferenceGraph.addEdge(revNode, "SINK", globalConfig.maxReviewsPerReviewer);
    }

    for (const auto& s : globalSubs) {
      std::string subNode = "S_" + std::to_string(s.id);
      for (const auto& r : globalRevs) {
          std::string revNode = "R_" + std::to_string(r.id);
          bool match = false;

          if (globalConfig.generateAssignments == 1) {
              if (s.primaryDomain == r.primaryExpertise) match = true;
          }
          else if (globalConfig.generateAssignments == 2) {
              if (s.primaryDomain == r.primaryExpertise ||
                 (s.secondaryDomain != -1 && s.secondaryDomain == r.primaryExpertise)) match = true;
          }
          else if (globalConfig.generateAssignments == 3) {
              if (s.primaryDomain == r.primaryExpertise ||
                 (s.secondaryDomain != -1 && s.secondaryDomain == r.primaryExpertise) ||
                 (r.secondaryExpertise != -1 && s.primaryDomain == r.secondaryExpertise) ||
                 (s.secondaryDomain != -1 && r.secondaryExpertise != -1 && s.secondaryDomain == r.secondaryExpertise))
                 match = true;
          }

          if (match) conferenceGraph.addEdge(subNode, revNode, 1);
      }
    }


    return true;
}

// ==========================================
// 4. FUNCIONALIDADES RESTANTES DO BACKEND
// ==========================================
void runMaxFlowAssignment() {
    if (globalSubs.empty() || globalRevs.empty()) {
        std::cerr << "[ERRO] Base de dados vazia. Carregue um dataset primeiro.\n";
        return;
    }
    std::cout << "\nA executar Algoritmo de Fluxo Maximo (Edmonds-Karp)...\n";
    edmondsKarp(&conferenceGraph, std::string("SOURCE"), std::string("SINK"));
    std::cout << "[SUCESSO] Distribuicao calculada com sucesso!\n";
}

// ==========================================
// FUNÇÕES AUXILIARES PARA A ANÁLISE DE RISCO
// ==========================================

// 1. Limpa os fluxos de todas as arestas para podermos correr o Max-Flow de novo
void resetGraphFlows() {
    for (auto v : conferenceGraph.getVertexSet()) {
        for (auto e : v->getAdj()) {
            e->setFlow(0);
        }
    }
}

// 2. Calcula quanto fluxo saiu da SOURCE
int calculateTotalFlow() {
    int totalFlow = 0;
    auto sourceVertex = conferenceGraph.findVertex("SOURCE");
    if (sourceVertex != nullptr) {
        for (auto e : sourceVertex->getAdj()) {
            totalFlow += e->getFlow();
        }
    }
    return totalFlow;
}

// 3. Função recursiva para testar combinações de K revisores
void testRiskCombinations(int k, int startIdx, std::vector<int>& currentCombo, int targetFlow) {
    // Caso base: Já escolhemos K revisores para "despedir"
    if (k == 0) {
        // Passo 1: Remover as arestas destes revisores para o SINK
        for (int revId : currentCombo) {
            std::string revNode = "R_" + std::to_string(revId);
            conferenceGraph.removeEdge(revNode, "SINK");
        }

        // Passo 2: Limpar fluxos e correr o algoritmo
        resetGraphFlows();
        edmondsKarp(&conferenceGraph, std::string("SOURCE"), std::string("SINK"));

        // Passo 3: Verificar se o sistema falhou (fluxo ficou abaixo do ideal)
        int newFlow = calculateTotalFlow();
        if (newFlow < targetFlow) {
            // Estes revisores deitaram a rede abaixo! Adicionar à lista global (sem duplicados)
            for (int revId : currentCombo) {
                if (std::find(globalRiskyReviewers.begin(), globalRiskyReviewers.end(), revId) == globalRiskyReviewers.end()) {
                    globalRiskyReviewers.push_back(revId);
                }
            }
        }

        // Passo 4: Voltar a adicionar as arestas (Repor o grafo ao normal)
        for (int revId : currentCombo) {
            std::string revNode = "R_" + std::to_string(revId);
            conferenceGraph.addEdge(revNode, "SINK", globalConfig.maxReviewsPerReviewer);
        }
        return;
    }

    // Gerar as combinações recursivamente
    for (size_t i = startIdx; i <= globalRevs.size() - k; ++i) {
        currentCombo.push_back(globalRevs[i].id);
        testRiskCombinations(k - 1, i + 1, currentCombo, targetFlow);
        currentCombo.pop_back();
    }
}

// ==========================================
// FUNCIONALIDADE 4: ANÁLISE DE RISCO PRINCIPAL
// ==========================================
void runRiskAnalysis() {
    if (globalConfig.riskAnalysis == 0) {
        std::cout << "Analise de risco esta desativada (RiskAnalysis = 0).\n";
        return;
    }

    if (globalSubs.empty() || globalRevs.empty()) {
        std::cout << "[ERRO] Base de dados vazia. Carregue um dataset primeiro.\n";
        return;
    }

    std::cout << "\nA iniciar Analise de Risco Nivel " << globalConfig.riskAnalysis << "...\n";

    // 1. Descobrir qual é o Fluxo Máximo "Ideal" (Todas as submissões tratadas)
    int targetFlow = globalSubs.size() * globalConfig.minReviewsPerSubmission;
    globalRiskyReviewers.clear();

    // 2. Iniciar a pesquisa de combinações de tamanho K
    int K = globalConfig.riskAnalysis;
    std::vector<int> currentCombination;

    // Aviso de performance se o K for muito grande
    if (K > 2) {
        std::cout << "[AVISO] K > 2. A gerar " << K << " combinacoes. Pode demorar uns segundos...\n";
    }

    testRiskCombinations(K, 0, currentCombination, targetFlow);

    // 3. Apresentar os resultados na consola
    if (globalRiskyReviewers.empty()) {
        std::cout << "[RESULTADO] A rede e robusta! Nenhum grupo de " << K << " revisores consegue deitar a conferencia abaixo.\n";
    } else {
        std::cout << "[RESULTADO] Foram encontrados " << globalRiskyReviewers.size() << " revisores de risco!\n";
        std::cout << "Va a Opcao 5 para exportar a lista detalhada para o ficheiro CSV.\n";
    }

    // 4. BÓNUS: Voltar a correr o fluxo original para que o grafo fique pronto para exportar os Assignments normais!
    resetGraphFlows();
    edmondsKarp(&conferenceGraph, std::string("SOURCE"), std::string("SINK"));
}

int getMatchDomain(const Submission& s, const Reviewer& r, const Config& config) {
    // Tenta primeiro o match primário vs primário
    if (config.generateAssignments >= 1) {
        if (s.primaryDomain == r.primaryExpertise) return s.primaryDomain;
    }
    // Tenta o match secundário (sub) vs primário (rev)
    if (config.generateAssignments >= 2) {
        if (s.secondaryDomain != -1 && s.secondaryDomain == r.primaryExpertise) return s.secondaryDomain;
    }
    // Tenta os restantes cruzamentos
    if (config.generateAssignments >= 3) {
        if (r.secondaryExpertise != -1 && s.primaryDomain == r.secondaryExpertise) return s.primaryDomain;
        if (s.secondaryDomain != -1 && r.secondaryExpertise != -1 && s.secondaryDomain == r.secondaryExpertise) return s.secondaryDomain;
    }
    return s.primaryDomain; // Retorno de segurança
}

// ==========================================
// EXPORTAÇÃO DE RESULTADOS
// ==========================================
void exportResults(const std::string& filename) {
    if (globalConfig.generateAssignments == 0) {
        std::cout << "[AVISO] GenerateAssignments = 0. A exportacao foi cancelada.\n";
        return;
    }

    std::string filename2 = "../output/" + filename;
    std::ofstream out(filename2);
    if (!out.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel criar o ficheiro " << filename2 << "\n";
        return;
    }

    std::cout << "A exportar resultados para: " << filename2 << "...\n";

    std::vector<AssignRecord> assignments;
    std::vector<MissingRecord> missingReviews;

    // 1. Extrair a informação do Grafo
    for (const auto& s : globalSubs) {
        std::string subNodeId = "S_" + std::to_string(s.id);
        auto v = conferenceGraph.findVertex(subNodeId);

        if (v == nullptr) continue;

        int currentReviews = 0;

        // Procurar todas as arestas de saída da Submissão (que vão para os Revisores)
        for (auto edge : v->getAdj()) {
            if (edge->getFlow() == 1) { // Se o fluxo é 1, há assignment!
                currentReviews++;

                // Extrair o ID do revisor ("R_X" -> X)
                std::string destInfo = edge->getDest()->getInfo();
                int revId = std::stoi(destInfo.substr(2));

                // Encontrar o Revisor na Base de Dados para descobrir o Match
                auto revIt = std::find_if(globalRevs.begin(), globalRevs.end(),
                                          [revId](const Reviewer& r) { return r.id == revId; });

                if (revIt != globalRevs.end()) {
                    int matchDom = getMatchDomain(s, *revIt, globalConfig);
                    assignments.push_back({s.id, revId, matchDom});
                }
            }
        }

        // Verificar se ficaram revisões em falta
        if (currentReviews < globalConfig.minReviewsPerSubmission) {
            missingReviews.push_back({s.id, s.primaryDomain, globalConfig.minReviewsPerSubmission - currentReviews});
        }
    }

    // 2. Imprimir a Primeira Secção: Submission -> Reviewer
    // Ordenar por SubmissionId crescente e depois ReviewerId
    std::sort(assignments.begin(), assignments.end(), [](const AssignRecord& a, const AssignRecord& b) {
        if (a.subId != b.subId) return a.subId < b.subId;
        return a.revId < b.revId;
    });

    out << "#SubmissionId,ReviewerId,Match\n";
    for (const auto& a : assignments) {
        out << a.subId << ", " << a.revId << ", " << a.matchDomain << "\n";
    }

    // 3. Imprimir a Segunda Secção: Reviewer -> Submission (Dual Information)
    // Reordenar a mesma lista, agora por ReviewerId crescente e depois SubmissionId
    std::sort(assignments.begin(), assignments.end(), [](const AssignRecord& a, const AssignRecord& b) {
        if (a.revId != b.revId) return a.revId < b.revId;
        return a.subId < b.subId;
    });

    out << "#ReviewerId,SubmissionId,Match\n";
    for (const auto& a : assignments) {
        out << a.revId << ", " << a.subId << ", " << a.matchDomain << "\n";
    }

    // 4. Imprimir o Total
    out << "#Total: " << assignments.size() << "\n";

    // 5. Imprimir Unsuccessful Assignments (se existirem)
    if (!missingReviews.empty()) {
        std::sort(missingReviews.begin(), missingReviews.end(), [](const MissingRecord& a, const MissingRecord& b) {
            return a.subId < b.subId;
        });

        out << "#SubmissionId,Domain,MissingReviews\n";
        for (const auto& m : missingReviews) {
            out << m.subId << ", " << m.domain << ", " << m.missing << "\n";
        }
    }

    if (globalConfig.riskAnalysis > 0) {
        // 1. Imprime SEMPRE o cabeçalho se a análise de risco estiver ativa
        out << "#Risk Analysis: " << globalConfig.riskAnalysis << "\n";

        // 2. SÓ imprime os números se a lista NÃO estiver vazia
        if (!globalRiskyReviewers.empty()) {
            std::sort(globalRiskyReviewers.begin(), globalRiskyReviewers.end());
            for (size_t i = 0; i < globalRiskyReviewers.size(); i++) {
                out << globalRiskyReviewers[i];
                if (i < globalRiskyReviewers.size() - 1) out << ", ";
            }
            out << "\n"; // Quebra de linha no fim dos números
        }
    }


    out.close();
    std::cout << "[SUCESSO] Exportacao concluida com " << assignments.size() << " atribuicoes!\n";
}

// ==========================================
// FUNÇÃO DE DEBUG: MOSTRAR O GRAFO
// ==========================================
void displayGraph() {
    if (conferenceGraph.getNumVertex() == 0) {
        std::cout << "[AVISO] O grafo esta vazio. Carregue um ficheiro primeiro.\n";
        return;
    }

    std::cout << "\n=== ESTRUTURA DO GRAFO (DEBUG) ===\n";
    std::cout << "Total de Vertices: " << conferenceGraph.getNumVertex() << "\n\n";

    // Iterar por todos os vértices do grafo
    for (auto v : conferenceGraph.getVertexSet()) {
        std::cout << "Vertice [" << v->getInfo() << "] liga a:\n";

        // Se não tiver ligações, avisa
        if (v->getAdj().empty()) {
            std::cout << "  -> (Sem ligacoes de saida)\n";
        }

        // Iterar por todas as arestas que saem deste vértice
        for (auto e : v->getAdj()) {
            std::cout << "  -> [" << e->getDest()->getInfo() << "] "
                      << "| Capacidade: " << e->getWeight()
                      << " | Fluxo: " << e->getFlow() << "\n";
        }
        std::cout << "-----------------------------------\n";
    }
    std::cout << "===================================\n";
}