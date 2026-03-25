#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <array>

// Detecta a distro
std::string detectarDistro() {
    std::ifstream osfile("/etc/os-release");
    std::string line, distro;
    if (!osfile.is_open()) return "";
    while (std::getline(osfile, line)) {
        if (line.find("ID=") == 0) {
            distro = line.substr(3);
            break;
        }
    }
    return distro;
}

// Imprime cabeçalho estilizado
void imprimirCabecalho() {
    std::cout << "\033[36m===========================\033[0m\n";
    std::cout << "\033[36m        Swiss CLI        \033[0m\n";
    std::cout << "\033[36m===========================\033[0m\n";
}

// Executa comando e captura a saída
std::string executarComando(const std::string& comando) {
    std::array<char, 128> buffer;
    std::string resultado;
    FILE* pipe = popen(comando.c_str(), "r");
    if (!pipe) return "";
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        resultado += buffer.data();
    }
    pclose(pipe);
    return resultado;
}

// Checa se pacote existe no APT
bool pacoteExisteAPT(const std::string& pacote) {
    std::string comando = "apt search " + pacote + " 2>/dev/null";
    std::string output = executarComando(comando);
    return output.find(pacote) != std::string::npos;
}

int main(int argc, char* argv[]) {
    imprimirCabecalho();

    std::string distro = detectarDistro();
    if (distro.empty()) {
        std::cout << "\033[31mErro: Não consegui detectar a distro.\033[0m\n";
        return 1;
    }
    std::cout << "\033[36m💻 Distro detectada: " << distro << "\033[0m\n";

    if (argc < 2) {
        std::cout << "\033[33mUso: swiss <nome_do_pacote>\033[0m\n";
        return 1;
    }

    std::string pacote;
    for (int i = 1; i < argc; ++i) {
        pacote += argv[i];
        if (i != argc - 1) pacote += " ";
    }

    std::cout << "\033[36m===========================\033[0m\n";

    bool aptDisponivel = system("which apt >/dev/null 2>&1") == 0;
    bool flatpakDisponivel = system("which flatpak >/dev/null 2>&1") == 0;

    // Mostra resultados APT
    if (aptDisponivel) {
        std::cout << "\033[34m🔍 Buscando no APT: " << pacote << "\033[0m\n";
        system(("apt search " + pacote).c_str());
    } else {
        std::cout << "\033[33mAPT não encontrado nesta distro.\033[0m\n";
    }

    // Mostra resultados Flatpak
    if (flatpakDisponivel) {
        std::cout << "\033[35m🔍 Buscando no Flatpak: " << pacote << "\033[0m\n";
        system(("flatpak search " + pacote).c_str());
    } else {
        std::cout << "\033[33mFlatpak não encontrado nesta distro.\033[0m\n";
    }

    // Pergunta qual instalar
    int escolha = -1;
    while (escolha != 0 && escolha != 1 && escolha != 2) {
        std::cout << "\033[32mEscolha onde instalar: 1 = APT, 2 = Flatpak, 0 = Não instalar: ";
        std::cin >> escolha;
    }

    if (escolha == 1 && aptDisponivel) {
        std::cout << "⚡ Instalando pelo APT: " << pacote << "\n";
        system(("sudo apt install " + pacote).c_str());
    } else if (escolha == 2 && flatpakDisponivel) {
        std::cout << "⚡ Instalando pelo Flatpak: " << pacote << "\n";
        system(("flatpak install flathub " + pacote).c_str());
    } else {
        std::cout << "Instalação cancelada.\n";
    }

    std::cout << "\033[36m---------------------------------\033[0m\n";
    return 0;
}