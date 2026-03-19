#include <iostream>
#include <string>
#include <vector>
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
    int choice;
    showMenu();
    std::cout << "Escolha uma opcao: ";
    std::cin >> choice;
    // Lógica do menu...
}