#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cstdio>

#define VERSION "0.2.0"

// ===== EXEC =====
std::string exec(const std::string& cmd)
{
    std::array<char, 256> buffer;
    std::string result;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }

    pclose(pipe);
    return result;
}

// ===== CHECK COMMAND =====
bool has(const std::string& cmd)
{
    std::string check = "command -v " + cmd + " > /dev/null 2>&1";
    return system(check.c_str()) == 0;
}

// ===== MANAGER STRUCT =====
struct Manager
{
    std::string name;
    std::string install;
    std::string remove;
    std::string update;
    std::string update_all;
};

// ===== MANAGERS =====
std::vector<Manager> managers =
{
    {"apt", "apt install -y ", "apt remove -y ", "apt install --only-upgrade -y ", "apt update && apt upgrade -y"},
    {"pacman", "pacman -S ", "pacman -R ", "pacman -S ", "pacman -Syu"},
    {"dnf", "dnf install -y ", "dnf remove -y ", "dnf upgrade -y ", "dnf upgrade -y"},
    {"flatpak", "flatpak install -y flathub ", "flatpak uninstall -y ", "flatpak update -y ", "flatpak update -y"},
    {"snap", "snap install ", "snap remove ", "snap refresh ", "snap refresh"},
    {"yay", "yay -S ", "yay -R ", "yay -S ", "yay -Syu"},
    {"paru", "paru -S ", "paru -R ", "paru -S ", "paru -Syu"},
    {"xbps", "xbps-install -S ", "xbps-remove ", "xbps-install -Su ", "xbps-install -Su"},
    {"pkg", "pkg install ", "pkg uninstall ", "pkg upgrade ", "pkg upgrade -y"}
};

// ===== GET MANAGER =====
Manager* getManager(const std::string& name)
{
    for (auto& m : managers)
    {
        if (m.name == name)
        {
            return &m;
        }
    }
    return nullptr;
}

// ===== COMMAND ALIASES =====
std::string resolveCommand(const std::string& cmd)
{
    if (cmd == "install" || cmd == "i" || cmd == "add" || cmd == "-S" || cmd == "get")
        return "install";

    if (cmd == "remove" || cmd == "rm" || cmd == "delete")
        return "remove";

    if (cmd == "search" || cmd == "s" || cmd == "find")
        return "search";

    if (cmd == "update" || cmd == "upgrade" || cmd == "up")
        return "update";

    if (cmd == "self-update" || cmd == "update-swiss" || cmd == "upgrade-swiss")
        return "self-update";

    if (cmd == "help" || cmd == "-h" || cmd == "--help")
        return "help";

    return cmd;
}

// ===== HELP =====
void showHelp()
{
    std::cout << "=========== Swiss Package Manager ===========\n\n";
    std::cout << "Versão: " << VERSION << "\n\n";

    std::cout << "📦 Instalar:\n";
    std::cout << "  swiss <pacote>\n";
    std::cout << "  swiss install <pacote>\n\n";

    std::cout << "🔍 Buscar:\n";
    std::cout << "  swiss search <pacote>\n\n";

    std::cout << "🗑️ Remover:\n";
    std::cout << "  swiss remove <manager> <pacote>\n\n";

    std::cout << "🔄 Atualizar:\n";
    std::cout << "  swiss update                (tudo)\n";
    std::cout << "  swiss update <manager>\n";
    std::cout << "  swiss update <manager> <pkg>\n";
    std::cout << "  swiss update <pkg>\n\n";

    std::cout << "⚙️ Sistema:\n";
    std::cout << "  swiss self-update\n";
    std::cout << "  swiss --version\n";
    std::cout << "  swiss --help\n\n";
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

    // VERSION
    if (std::string(argv[1]) == "--version")
    {
        std::cout << "Swiss v" << VERSION << "\n";
        return 0;
    }

    // HELP
    if (command == "help")
    {
        showHelp();
        return 0;
    }

    // FALLBACK (swiss firefox)
    if (command == argv[1])
    {
        command = "install";
    }

    // ===== SELF UPDATE =====
    if (command == "self-update")
    {
        std::cout << "🔄 Atualizando Swiss...\n";
        system("curl -sL https://raw.githubusercontent.com/feroshina/Swiss-Package-Utensil/main/install.sh | bash");
        return 0;
    }

    // ===== UPDATE =====
    if (command == "update")
    {
        if (argc == 2)
        {
            for (auto& m : managers)
            {
                if (has(m.name))
                {
                    system(m.update_all.c_str());
                }
            }
            return 0;
        }

        std::string target = argv[2];
        Manager* m = getManager(target);

        if (m && has(m->name))
        {
            if (argc >= 4)
            {
                system((m->update + std::string(argv[3])).c_str());
            }
            else
            {
                system(m->update_all.c_str());
            }
            return 0;
        }

        for (auto& m2 : managers)
        {
            if (has(m2.name))
            {
                system((m2.update + target).c_str());
            }
        }

        return 0;
    }

    // ===== INSTALL =====
    if (command == "install")
    {
        std::vector<std::string> available;

        for (auto& m : managers)
        {
            if (has(m.name))
            {
                available.push_back(m.name);
            }
        }

        if (available.empty())
        {
            std::cout << "Nenhum gerenciador encontrado.\n";
            return 1;
        }

        std::string pkgList;
        for (int i = 1; i < argc; i++)
        {
            pkgList += std::string(argv[i]) + " ";
        }

        std::cout << "Métodos disponíveis:\n";
        for (int i = 0; i < available.size(); i++)
        {
            std::cout << i + 1 << ") " << available[i] << "\n";
        }

        std::cout << "\nInstalar automaticamente? (y/n): ";
        char c;
        std::cin >> c;

        std::string chosen;

        if (c == 'y' || c == 'Y')
        {
            chosen = available[0];
        }
        else
        {
            int opt;
            std::cin >> opt;
            chosen = available[opt - 1];
        }

        Manager* m = getManager(chosen);

        if (m)
        {
            system((m->install + pkgList).c_str());
        }

        return 0;
    }

    // ===== REMOVE =====
    if (command == "remove")
    {
        if (argc < 4)
        {
            std::cout << "Uso: swiss remove <manager> <pacote>\n";
            return 1;
        }

        Manager* m = getManager(argv[2]);

        if (m)
        {
            system((m->remove + std::string(argv[3])).c_str());
        }

        return 0;
    }

    // ===== SEARCH =====
    if (command == "search")
    {
        if (argc < 3)
        {
            std::cout << "Uso: swiss search <pacote>\n";
            return 1;
        }

        std::string pkg = argv[2];

        for (auto& m : managers)
        {
            if (has(m.name))
            {
                std::cout << "\n[" << m.name << "]\n";
                std::cout << exec(m.install + pkg);
            }
        }

        return 0;
    }

    showHelp();
    return 0;
}
