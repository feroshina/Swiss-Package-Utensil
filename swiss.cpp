#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cstdio>
// ===== EXEC =====
#define VERSION "0.1.2"
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
    std::string check = "which " + cmd + " > /dev/null 2>&1";
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
    if (cmd == "search" || cmd == "s" || cmd == "find" || cmd == "-Ss")
    {
        return "search";
    }

    // UPDATE
    if (cmd == "update" || cmd == "upgrade" || cmd == "up")
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

    if (manager == "apt")
    {
        result = exec("apt search " + pkg);
    }
    else if (manager == "pacman")
    {
        result = exec("pacman -Ss " + pkg);
    }
    else if (manager == "dnf")
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

    return !result.empty();
}

// ===== INSTALL =====
bool installPkg(const std::string& manager, const std::string& pkgList)
{
    int result = 1;

    if (manager == "apt")
    {
        result = system(("sudo apt install -y " + pkgList).c_str());
    }
    else if (manager == "pacman")
    {
        result = system(("sudo pacman -S " + pkgList).c_str());
    }
    else if (manager == "dnf")
    {
        result = system(("sudo dnf install -y " + pkgList).c_str());
    }
    else if (manager == "flatpak")
    {
        result = system(("flatpak install -y flathub " + pkgList).c_str());
    }
    else if (manager == "snap")
    {
        result = system(("sudo snap install " + pkgList).c_str());
    }
    else if (manager == "yay")
    {
        result = system(("yay -S " + pkgList).c_str());
    }
    else if (manager == "xbps")
    {
        result = system(("xbps-install -S " + pkgList).c_str());
    }

    return result == 0;
}

// ===== REMOVE =====
void removePkg(const std::string& manager, const std::string& pkgList)
{
    if (manager == "apt")
    {
        system(("sudo apt remove -y " + pkgList).c_str());
    }
    else if (manager == "pacman")
    {
        system(("sudo pacman -R " + pkgList).c_str());
    }
    else if (manager == "dnf")
    {
        system(("sudo dnf remove -y " + pkgList).c_str());
    }
    else if (manager == "flatpak")
    {
        system(("flatpak uninstall -y " + pkgList).c_str());
    }
    else if (manager == "snap")
    {
        system(("sudo snap remove " + pkgList).c_str());
    }
    else if (manager == "yay")
    {
        system(("yay -R " + pkgList).c_str());
    }
    else if (manager == "xbps")
    {
        system(("xbps-remove " + pkgList).c_str());
    }
        
}

// ===== HELP =====
void showHelp()
{
    std::cout << "Swiss v" << VERSION << "\n\n";

    std::cout << "Uso:\n";
    std::cout << "  swiss <pacote>                (instala automaticamente)\n";
    std::cout << "  swiss install <pacote>\n";
    std::cout << "  swiss add <pacote>\n";
    std::cout << "  swiss i <pacote>\n";
    std::cout << "  swiss in <pacote>\n";
    std::cout << "  swiss get <pacote>\n";
    std::cout << "  swiss -S <pacote>\n\n";

    std::cout << "  swiss search <pacote>\n";
    std::cout << "  swiss s <pacote>\n\n";

    std::cout << "  swiss delete <manager> <pacote>\n";
    std::cout << "  swiss remove <manager> <pacote>\n";
    std::cout << "  swiss rm <manager> <pacote>\n\n";

    std::cout << "  swiss update\n\n";

    std::cout << "  -h, --help, help, h\n";
    std::cout << "  --version\n";
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

    // ===== UPDATE =====
    if (command == "update")
    {
        system("curl -sL https://raw.githubusercontent.com/SEU_USUARIO/swiss/main/install.sh | bash");
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
            "xbps", "apt", "pacman", "dnf", "yay", "flatpak", "snap"
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

        std::cout << "\nInstalar automaticamente? (s, y/n): ";
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
        if (has("pacman")) std::cout << exec("pacman -Ss " + pkg);
        if (has("dnf")) std::cout << exec("dnf search " + pkg);
        if (has("flatpak")) std::cout << exec("flatpak search " + pkg);
        if (has("snap")) std::cout << exec("snap find " + pkg);
        if (has("yay")) std::cout << exec("yay -Ss " + pkg);
        if (has("xbps")) std::cout << exec("xbps-query " + pkg);
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
