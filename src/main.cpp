#include <iostream>
#include <string>
#include <vector>
#include "backend.h"

void runBatchMode(const std::string& input, const std::string& output);
void handleInteractiveMenu();

/**
 * @brief Exibe o menu interativo principal na consola.
 */
void showMenu() {
    std::cout << "--- Scientific Conference Tool 2026 ---" << std::endl;
    std::cout << "1. Load Dataset (.csv)" << std::endl;
    std::cout << "2. View/Edit Parameters" << std::endl;
    std::cout << "3. Run Max-Flow Assignment" << std::endl;
    std::cout << "4. Risk Analysis (R1/RK)" << std::endl;
    std::cout << "5. Export Results" << std::endl;
    std::cout << "6. Run Batch Mode" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Escolha uma opcao: ";
}

/**
 * @brief Funcao principal (Ponto de entrada do programa).
 * Verifica se existem argumentos de linha de comandos para ativar o modo batch.
 * Caso contrario, inicia o menu interativo.
 */
int main(int argc, char* argv[]) {
    if (argc >= 2 && std::string(argv[1]) == "-b") {
        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " -b <input.csv> <output.csv>" << std::endl;
            return 1;
        }
        runBatchMode(argv[2], argv[3]);
    }
    else {
        handleInteractiveMenu();
    }
    return 0;
}

/**
 * @brief Executa o programa de forma automatica (Modo Linha de Comandos).
 * * Processa a leitura, atribuicao, analise de risco e exportacao numa unica
 * * @param input O caminho para o ficheiro CSV de entrada.
 * @param output O caminho para o ficheiro CSV de saida.
 */
void runBatchMode(const std::string& input, const std::string& output) {
    std::cout << "Modo Batch Ativado!" << std::endl;
    std::cout << "A ler de: " << input << std::endl;
    std::cout << "A gravar em: " << output << std::endl;

    if (loadInputAndBuildGraph(input)) {
        runMaxFlowAssignment();

        if (globalConfig.riskAnalysis > 0) {
            runRiskAnalysis();
        }

        exportResults(output);
    }
    else {
        std::cerr << "[ERRO] Falha ao processar ficheiro no modo batch." << std::endl;
    }
}

/**
 * @brief Gere o ciclo principal de interacao com o utilizador.
 * * Le as escolhas do utilizador, faz a validacao de inputs e invoca
 */
void handleInteractiveMenu() {
    int choice = -1;
    while (choice != 0) {
        showMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n'); continue;
        }

        switch (choice) {
            case 1: {
                std::string filename;
                std::cout << "Introduza o nome do ficheiro (ex: dataset.csv): ";
                std::cin >> filename;
                std::string filename2 = "../input/" + filename;
                if (loadInputAndBuildGraph(filename2)) {
                    std::cout << "-> Submissoes lidas: " << globalSubs.size() << "\n";
                    std::cout << "-> Revisores lidos: " << globalRevs.size() << "\n";
                    displayGraph();
                    std::cout << "\n";
                }
                break;
            }
            case 2:
                std::cout << "\n--- PARAMETROS ATUAIS ---\n";
                std::cout << "Min Reviews/Submission: " << globalConfig.minReviewsPerSubmission << "\n";
                std::cout << "Max Reviews/Reviewer: " << globalConfig.maxReviewsPerReviewer << "\n";
                std::cout << "Output File: " << globalConfig.outputFileName << "\n";
                std::cout << "\n";
                break;
            case 3:
                runMaxFlowAssignment();
                break;
            case 4:
                runRiskAnalysis();
                break;
            case 5:
                exportResults(globalConfig.outputFileName);
                break;
            case 6: {
                std::string inFilename, outFilename;

                std::cout << "Introduza o nome do ficheiro de ENTRADA (ex: dataset.csv): ";
                std::cin >> inFilename;

                std::cout << "Introduza o nome do ficheiro de SAIDA (ex: output.csv): ";
                std::cin >> outFilename;

                std::string inputPath = "../input/" + inFilename;

                std::cout << "\n[BATCH MODE] A processar tudo automaticamente...\n";
                runBatchMode(inputPath, outFilename);

                break;
            }
            case 0:
                std::cout << "A sair...\n";
                break;
            default:
                std::cout << "Opcao invalida.\n";
        }
    }
}