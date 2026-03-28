#include <iostream>
#include <string>
#include <vector>
#include "backend.h"
void runBatchMode(const std::string& input, const std::string& output);
void handleInteractiveMenu();

void showMenu() {
    std::cout << "--- Scientific Conference Tool 2026 ---" << std::endl;
    std::cout << "1. Load Dataset (.csv)" << std::endl;
    std::cout << "2. View/Edit Parameters" << std::endl;
    std::cout << "3. Run Max-Flow Assignment" << std::endl;
    std::cout << "4. Risk Analysis (R1/RK)" << std::endl;
    std::cout << "5. Export Results" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Escolha uma opcao: ";
}

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

void runBatchMode(const std::string& input, const std::string& output) {
    std::cout << "Modo Batch Ativado!" << std::endl;
    std::cout << "A ler de: " << input << std::endl;
    std::cout << "A gravar em: " << output << std::endl;
    // Aqui virá a lógica de abrir os ficheiros CSV
}

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
                // runRiskAnalysis();
                break;
            case 5:
                exportResults(globalConfig.outputFileName);
                break;
            case 0:
                std::cout << "A sair...\n";
                break;
            default:
                std::cout << "Opcao invalida.\n";
        }
    }
}