#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
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

    // Mensagem de status mostrando a distro detectada
    std::cout << "\033[36m💻 Distro detectada: " << distro << "\033[0m\n";

    if (argc < 3) {
        std::cout << "\033[33mUso: swiss <s/i> <nome_do_pacote>\033[0m\n";
        std::cout << "s = buscar pacote | i = instalar pacote\n";
        return 1;
    }

    std::string acao = argv[1];
    std::string pacote;
    for (int i = 2; i < argc; ++i) {
        pacote += argv[i];
        if (i != argc - 1) pacote += " ";
    }

    bool achou = false;

    // Primeiro tenta APT para distros compatíveis
    if ((distro == "linuxmint" || distro == "ubuntu" || distro == "debian")) {
        if (pacoteExisteAPT(pacote)) {
            achou = true;
            if (acao == "s") {
                std::cout << "\033[34m🔍 Buscando no APT: " << pacote << "\033[0m\n";
                system(("apt search " + pacote).c_str());
            } else if (acao == "i") {
                std::cout << "\033[32m⚡ Instalando pelo APT: " << pacote << "\033[0m\n";
                system(("sudo apt install " + pacote).c_str());
            }
        }
    }

    // Se não achou no APT, tenta Flatpak
    if (!achou) {
        std::cout << "\033[33mPacote não encontrado no APT, buscando no Flatpak...\033[0m\n";
        if (acao == "s") {
            std::cout << "\033[35m🔍 Buscando no Flatpak: " << pacote << "\033[0m\n";
            system(("flatpak search " + pacote).c_str());
        } else if (acao == "i") {
            std::cout << "\033[32m⚡ Instalando pelo Flatpak: " << pacote << "\033[0m\n";
            system(("flatpak install flathub " + pacote).c_str());
        }
    }

    std::cout << "\033[36m---------------------------------\033[0m\n";
    return 0;
}