#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cstdio>
// ===== EXEC =====
#define VERSION "0.1.5"
std::string exec(const std::string& cmd)
{
    std::array<char, 256> buffer;
    std::string result;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        return "";
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }

    pclose(pipe);
    return result;
}

// ===== SYSTEM CHECK =====
bool has(const std::string& cmd)
{
    std::string check = "command -v " + cmd + " > /dev/null 2>&1";
    return system(check.c_str()) == 0;
}

// ===== COMMAND ALIASES =====
std::string resolveCommand(const std::string& cmd)
{
    // INSTALL
    if (cmd == "install" || cmd == "i" || cmd == "add" || cmd == "-S" || cmd == "get" || cmd == "in")
    {
        return "install";
    }

    // REMOVE
    if (cmd == "remove" || cmd == "rm" || cmd == "uninstall" || cmd == "-R" || cmd == "delete")
    {
        return "remove";
    }

    // SEARCH
    if (cmd == "search" || cmd == "s" || cmd == "find" || cmd == "-Ss" || cmd == "look")
    {
        return "search";
    }

    // UPDATE
    if (cmd == "update" || cmd == "upgrade" || cmd == "up" || cmd == "-Syu")
    {
        return "update";
    }

    // HELP
    if (cmd == "help" || cmd == "-h" || cmd == "--help" || cmd == "h" || cmd == "-help")
    {
        return "help";
    }

    return cmd;
}

// ===== SEARCH =====
bool search(const std::string& manager, const std::string& pkg)
{
    std::string result;
    std::cout << "  🔎  Pesquisando pacote(s)..." <<std::endl;
    if (manager == "apt")
    {
        result = exec("apt search " + pkg);
    }
    else if (manager == "pacman")
    {
        result = exec("pacman -Ss " + pkg);
    }
    else if (manager == "dnf" || "dnf5")
    {
        result = exec("dnf search " + pkg);
    }
    else if (manager == "flatpak")
    {
        result = exec("flatpak search " + pkg);
    }
    else if (manager == "snap")
    {
        result = exec("snap find " + pkg);
    }
    else if (manager == "yay")
    {
        result = exec("yay -Ss " + pkg);
    }
    else if (manager == "xbps")
    {
        result = exec("xbps-query " + pkg);
    }
    else if (manager == "paru")
    {
        result = exec("paru -Ss " + pkg);
    }
    else if (manager == "pkg")
    {
        result = exec("pkg search " + pkg);
    }

    return !result.empty();
}

// ===== INSTALL =====
bool installPkg(const std::string& manager, const std::string& pkgList)
{
    int result = 1;
    std::cout << "  📦  Instalando pacote(s)..." <<std::endl;
    if (manager == "apt")
    {
        result = system(("apt install -y " + pkgList).c_str());
    }
    else if (manager == "pacman")
    {
        result = system(("pacman -S " + pkgList).c_str());
    }
    else if (manager == "dnf" || "dnf5")
    {
        result = system(("dnf install -y " + pkgList).c_str());
    }
    else if (manager == "flatpak")
    {
        result = system(("flatpak install -y flathub " + pkgList).c_str());
    }
    else if (manager == "snap")
    {
        result = system(("snap install " + pkgList).c_str());
    }
    else if (manager == "yay")
    {
        result = system(("yay -S " + pkgList).c_str());
    }
    else if (manager == "xbps")
    {
        result = system(("xbps-install -S " + pkgList).c_str());
    }
    else if (manager == "paru")
    {
        result = system(("paru -S " + pkgList).c_str());
    }
    else if (manager == "pkg")
    {
        result = system(("pkg install " + pkgList).c_str());
    }

    return result == 0;
}

// ===== REMOVE =====
void removePkg(const std::string& manager, const std::string& pkgList)
{
    std::cout << "  🗑️  Removendo pacote(s)..." << std::endl;
    if (manager == "apt")
    {
        system(("apt remove -y " + pkgList).c_str());
    }
    else if (manager == "pacman")
    {
        system(("pacman -R " + pkgList).c_str());
    }
    else if (manager == "dnf" || "dnf5")
    {
        system(("dnf remove -y " + pkgList).c_str());
    }
    else if (manager == "flatpak")
    {
        system(("flatpak uninstall -y " + pkgList).c_str());
    }
    else if (manager == "snap")
    {
        system(("snap remove " + pkgList).c_str());
    }
    else if (manager == "yay")
    {
        system(("yay -R " + pkgList).c_str());
    }
    else if (manager == "xbps")
    {
        system(("xbps-remove " + pkgList).c_str());
    }
    else if (manager == "paru")
    {
        system(("paru -R " + pkgList).c_str());
    }
    else if (manager == "pkg")
    {
        system(("pkg uninstall " + pkgList).c_str());
    }
        
}

// ===== HELP =====
void showHelp()
{
    std::cout << "============== Swiss, sua ferramenta Linux ==============" << "\n\n";
    std::cout << "Swiss v" << VERSION << "\n\n";

    std::cout << "Instalação 📦 \n\n";
    std::cout << "  swiss <pacote>                (instala automaticamente)\n";
    std::cout << "  swiss install <pacote>\n";
    std::cout << "  swiss add <pacote>\n";
    std::cout << "  swiss i <pacote>\n";
    std::cout << "  swiss in <pacote>\n";
    std::cout << "  swiss get <pacote>\n";
    std::cout << "  swiss -S <pacote>\n\n";

    std::cout << "Pesquisa 🔍 \n\n";
    std::cout << "  swiss search <pacote>\n";
    std::cout << "  swiss -Ss <pacote>\n";
    std::cout << "  swiss find <pacote>\n";
    std::cout << "  swiss look <pacote>\n";
    std::cout << "  swiss s <pacote>\n\n";

    std::cout << "Remoção 🗑️ \n\n";
    std::cout << "  swiss delete <manager> <pacote>\n";
    std::cout << "  swiss remove <manager> <pacote>\n";
    std::cout << "  swiss rm <manager> <pacote>\n\n";

    std::cout << "  swiss up <pacote>\n";
    std::cout << "  swiss -Syu <pacote>\n";
    std::cout << "  swiss upgrade <pacote>\n";
    std::cout << "  swiss update <pacote>\n\n";

    std::cout << "  -h, --help, help, h\n";
    std::cout << "  --version\n";
}
// ===== UPDATE PACKAGE =====
void updatePkg(const std::string& manager, const std::string& pkg)
{
    std::cout << "  🔄  Atualizando pacote...\n";

    if (manager == "apt")
    {
        system(("apt install --only-upgrade -y " + pkg).c_str());
    }
    else if (manager == "pacman")
    {
        system(("pacman -S " + pkg).c_str());
    }
    else if (manager == "dnf" || manager == "dnf5")
    {
        system(("dnf upgrade -y " + pkg).c_str());
    }
    else if (manager == "flatpak")
    {
        system(("flatpak update -y " + pkg).c_str());
    }
    else if (manager == "snap")
    {
        system(("snap refresh " + pkg).c_str());
    }
    else if (manager == "yay")
    {
        system(("yay -S " + pkg).c_str());
    }
    else if (manager == "paru")
    {
        system(("paru -S " + pkg).c_str());
    }
    else if (manager == "xbps")
    {
        system(("xbps-install -Su " + pkg).c_str());
    }
    else if (manager == "pkg")
    {
        system(("pkg upgrade " + pkg).c_str());
    }
}
// ===== UPDATE ALL (1 MANAGER) =====
void updateAll(const std::string& manager)
{
    std::cout << "  🔄  Atualizando via " << manager << "...\n";

    if (manager == "apt")
    {
        system("apt update && apt upgrade -y");
    }
    else if (manager == "pacman")
    {
        system("pacman -Syu");
    }
    else if (manager == "dnf" || manager == "dnf5")
    {
        system("dnf upgrade -y");
    }
    else if (manager == "flatpak")
    {
        system("flatpak update -y");
    }
    else if (manager == "snap")
    {
        system("snap refresh");
    }
    else if (manager == "yay")
    {
        system("yay -Syu");
    }
    else if (manager == "paru")
    {
        system("paru -Syu");
    }
    else if (manager == "xbps")
    {
        system("xbps-install -Su");
    }
    else if (manager == "pkg")
    {
        system("pkg upgrade -y");
    }
}
// ===== UPDATE SYSTEM =====
void updateSystem(const std::vector<std::string>& managers)
{
    for (auto& m : managers)
    {
        if (has(m))
        {
            updateAll(m);
        }
    }
}
// SELF UPDATE
if (cmd == "self-update" || cmd == "update-swiss" || cmd == "upgrade-swiss")
{
    return "self-update";
}
// ===== MAIN =====
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        showHelp();
        return 0;
    }

    std::string command = resolveCommand(argv[1]);

    // ===== VERSION =====
    if (std::string(argv[1]) == "--version")
    {
        std::cout << "Swiss v" << VERSION << "\n";
        return 0;
    }

    // ===== HELP =====
    if (command == "help")
    {
        showHelp();
        return 0;
    }

    // ===== FALLBACK (swiss firefox) =====
    if (command == argv[1])
    {
        command = "install";
    }
// ===== SELF UPDATE =====
if (command == "self-update")
{
    std::cout << "Atualizando Swiss...\n";

    system("curl -sL https://raw.githubusercontent.com/feroshina/Swiss-Package-Utensil/main/install.sh | bash");

    return 0;
}
// ===== UPDATE =====
if (command == "update")
{
    std::vector<std::string> managers =
    {
        "xbps", "pkg", "apt", "pacman", "dnf", "dnf5", "paru", "yay", "flatpak", "snap"
    };

    // swiss update → tudo
    if (argc == 2)
    {
        updateSystem(managers);
        return 0;
    }

    std::string target = argv[2];

    // swiss update apt
    if (has(target))
    {
        updateAll(target);
        return 0;
    }

    // swiss update apt firefox
    if (argc >= 4)
    {
        std::string manager = argv[2];
        std::string pkg = argv[3];

        updatePkg(manager, pkg);
        return 0;
    }

    // swiss update firefox (auto)
    for (auto& m : managers)
    {
        if (has(m))
        {
            updatePkg(m, target);
        }
    }

    return 0;
}
    // ===== INSTALL =====
    if (command == "install")
    {
        if (argc < 2)
        {
            std::cout << "Uso: swiss install <pacote>\n";
            return 1;
        }

        std::vector<std::string> packages;

        int start = (resolveCommand(argv[1]) == "install" && argc > 2) ? 2 : 1;

        for (int i = start; i < argc; i++)
        {
            packages.push_back(argv[i]);
        }

        std::string pkgList;
        for (auto& p : packages)
        {
            pkgList += p + " ";
        }

        std::vector<std::string> priority =
        {
            "xbps", "pkg", "apt", "pacman", "dnf", "dnf5", "paru", "yay", "flatpak", "snap"
        };

        std::vector<std::string> available;

        for (auto& m : priority)
        {
            if (has(m) && search(m, packages[0]))
            {
                available.push_back(m);
            }
        }

        if (available.empty())
        {
            std::cout << "Nenhum gerenciador encontrou o pacote.\n";
            return 1;
        }

        std::cout << "Métodos disponíveis:\n";

        for (int i = 0; i < available.size(); i++)
        {
            std::cout << i + 1 << ") " << available[i] << "\n";
        }

        std::cout << "\n  📥  Instalar do repositório recomendado? (s, y/n): ";
        char choice;
        std::cin >> choice;

        if (choice == 'y' || choice == 'Y' || choice == 's' || choice == 'S')
        {
            std::cout << "Usando: " << available[0] << "\n";
            installPkg(available[0], pkgList);
        }
        else
        {
            std::cout << "Escolha: ";
            int opt;
            std::cin >> opt;

            if (opt < 1 || opt > available.size())
            {
                std::cout << "Opção inválida\n";
                return 1;
            }

            installPkg(available[opt - 1], pkgList);
        }
    }

    // ===== SEARCH =====
    else if (command == "search")
    {
        if (argc < 3)
        {
            std::cout << "Uso: swiss search <pacote>\n";
            return 1;
        }

        std::string pkg = argv[2];

        if (has("apt")) std::cout << exec("apt search " + pkg);
        if (has("pacman")) std::cout << exec("sudo pacman -Ss " + pkg);
        if (has("dnf5")) std::cout << exec("dnf search " + pkg);
        if (has("dnf")) std::cout << exec("dnf search " + pkg);
        if (has("flatpak")) std::cout << exec("flatpak search " + pkg);
        if (has("snap")) std::cout << exec("snap find " + pkg);
        if (has("yay")) std::cout << exec("yay -Ss " + pkg);
        if (has("xbps")) std::cout << exec("xbps-query " + pkg);
        if (has("paru")) std::cout << exec("paru -Ss " + pkg);
        if (has("pkg")) std::cout << exec("pkg search " + pkg);
    }
    // ===== REMOVE =====
    else if (command == "remove")
    {
        if (argc < 4)
        {
            std::cout << "Uso: swiss remove <manager> <pacote>\n";
            return 1;
        }

        std::string manager = argv[2];
        std::string pkg = argv[3];

        removePkg(manager, pkg);
    }

    else
    {
        showHelp();
    }

    return 0;
}
