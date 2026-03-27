#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <thread>
#include <chrono>

#define VERSION "1.0.0"

// ===== EXEC =====
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

std::string trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
    {
        return "";
    }

    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::vector<std::string> splitWords(const std::string& s)
{
    std::istringstream iss(s);
    std::vector<std::string> out;
    std::string word;

    while (iss >> word)
    {
        out.push_back(word);
    }

    return out;
}

// ===== VISUAL =====
void status(const std::string& text)
{
    std::cout << "📦 " << text << "...\n";
}

void progress(const std::string& text)
{
    std::vector<std::string> frames = {"📦", "📦✂️", "📦✂️📃", "📦✂️📃📃", "📦✂️📃📃📃"};

    for (const auto& frame : frames)
    {
        std::cout << "\r" << text << " " << frame << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
    }

    std::cout << "\n";
}

// ===== CHECK COMMAND =====
bool has(const std::string& cmd)
{
    std::string check = "command -v " + cmd + " > /dev/null 2>&1";
    return system(check.c_str()) == 0;
}

// ===== OS-RELEASE PARSING =====
std::string getOsReleaseValue(const std::string& key)
{
    std::ifstream file("/etc/os-release");
    if (!file.is_open())
    {
        return "";
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.rfind(key + "=", 0) == 0)
        {
            std::string value = line.substr(key.size() + 1);
            value = trim(value);

            if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
            {
                value = value.substr(1, value.size() - 2);
            }

            return value;
        }
    }

    return "";
}

bool osReleaseHasToken(const std::string& key, const std::string& token)
{
    std::string value = getOsReleaseValue(key);
    if (value.empty())
    {
        return false;
    }

    value = toLower(value);
    std::string tokenLower = toLower(token);

    for (const auto& word : splitWords(value))
    {
        if (word == tokenLower)
        {
            return true;
        }
    }

    return false;
}

// ===== DISTRO / FAMILY DETECTION =====
std::string getDistroID()
{
    std::string id = toLower(getOsReleaseValue("ID"));
    if (!id.empty())
    {
        return id;
    }

    return "unknown";
}

std::string getFamily()
{
    if (osReleaseHasToken("ID_LIKE", "arch"))
    {
        return "arch";
    }

    if (osReleaseHasToken("ID_LIKE", "debian"))
    {
        return "debian";
    }

    if (osReleaseHasToken("ID_LIKE", "rhel") || osReleaseHasToken("ID_LIKE", "fedora"))
    {
        return "fedora";
    }

    if (osReleaseHasToken("ID_LIKE", "gentoo"))
    {
        return "gentoo";
    }

    std::string id = getDistroID();

    if (id == "ubuntu" || id == "debian" || id == "linuxmint" || id == "pop" || id == "kali" || id == "elementary")
    {
        return "debian";
    }

    if (id == "arch" || id == "manjaro" || id == "endeavouros" || id == "garuda" || id == "cachyos")
    {
        return "arch";
    }

    if (id == "fedora" || id == "rhel" || id == "centos" || id == "rocky" || id == "almalinux")
    {
        return "fedora";
    }

    if (id == "gentoo")
    {
        return "gentoo";
    }

    if (id == "void")
    {
        return "void";
    }

    if (id == "nixos")
    {
        return "nixos";
    }

    return "unknown";
}

// ===== MANAGER STRUCT =====
struct Manager
{
    std::string name;
    std::string install;
    std::string remove;
    std::string update;
    std::string update_all;
    std::string search;
};

// ===== MANAGERS =====
std::vector<Manager> managers =
{
    {"apt",     "sudo apt install -y ",              "sudo apt remove -y ",            "sudo apt install --only-upgrade -y ", "sudo apt update && sudo apt upgrade -y", "apt search "},
    {"pacman",  "sudo pacman -S ",                   "sudo pacman -R ",                 "sudo pacman -S ",                     "sudo pacman -Syu",                      "pacman -Ss "},
    {"dnf",     "sudo dnf install -y ",              "sudo dnf remove -y ",             "sudo dnf upgrade -y ",                "sudo dnf upgrade -y",                   "dnf search "},
    {"emerge",  "sudo emerge ",                      "sudo emerge --unmerge ",          "sudo emerge --update ",               "sudo emerge --update @world",           "emerge --search "},
    {"xbps",    "sudo xbps-install -S ",             "sudo xbps-remove ",               "sudo xbps-install -Su ",              "sudo xbps-install -Su",                 "xbps-query -Rs "},
    {"nix-env", "nix-env -iA nixpkgs.",              "nix-env -e ",                     "nix-env -u ",                         "nix-env -u",                            "nix-env -qaP "},
    {"yay",     "yay -S ",                           "yay -R ",                         "yay -S ",                              "yay -Syu",                              "yay -Ss "},
    {"paru",    "paru -S ",                          "paru -R ",                        "paru -S ",                             "paru -Syu",                             "paru -Ss "},
    {"pkg",     "pkg install ",                      "pkg uninstall ",                  "pkg upgrade ",                        "pkg upgrade -y",                        "pkg search "},
    {"flatpak", "flatpak install -y flathub ",       "flatpak uninstall -y ",           "flatpak update -y ",                  "flatpak update -y",                     "flatpak search "},
    {"snap",    "sudo snap install ",                "sudo snap remove ",                "sudo snap refresh ",                  "sudo snap refresh",                     "snap find "}
};

// ===== PACKAGE MAP =====
std::map<std::string, std::map<std::string, std::string>> packageMap =
{
    {
        "discord",
        {
            {"flatpak", "com.discordapp.Discord"},
            {"nix-env", "discord"}
        }
    },
    {
        "firefox",
        {
            {"flatpak", "org.mozilla.firefox"},
            {"nix-env", "firefox"}
        }
    },
    {
        "steam",
        {
            {"flatpak", "com.valvesoftware.Steam"},
            {"nix-env", "steam"}
        }
    }
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

bool isKnownManager(const std::string& name)
{
    return getManager(name) != nullptr;
}

// ===== COMMAND ALIASES =====
std::string resolveCommand(const std::string& cmd)
{
    if (cmd == "install" || cmd == "i" || cmd == "add" || cmd == "-S" || cmd == "get")
        return "install";

    if (cmd == "remove" || cmd == "rm" || cmd == "delete" || cmd == "-R")
        return "remove";

    if (cmd == "search" || cmd == "s" || cmd == "find" || cmd == "-Ss")
        return "search";

    if (cmd == "update" || cmd == "upgrade" || cmd == "up" || cmd == "-Syu")
        return "update";

    if (cmd == "self-update" || cmd == "update-swiss" || cmd == "upgrade-swiss")
        return "self-update";

    if (cmd == "help" || cmd == "-h" || cmd == "--help" || cmd == "h" || cmd == "-help")
        return "help";

    return cmd;
}

// ===== PRIORITY =====
std::vector<std::string> getPriority()
{
    std::string family = getFamily();

    if (family == "arch")
        return {"pacman", "yay", "paru", "flatpak", "snap"};

    if (family == "debian")
        return {"apt", "flatpak", "snap"};

    if (family == "fedora")
        return {"dnf", "flatpak", "snap"};

    if (family == "gentoo")
        return {"emerge", "flatpak", "snap"};

    if (family == "void")
        return {"xbps", "flatpak", "snap"};

    if (family == "nixos")
        return {"nix-env", "flatpak"};

    return {"apt", "pacman", "dnf", "flatpak", "snap"};
}

std::vector<std::string> getAvailable()
{
    std::vector<std::string> available;
    std::vector<std::string> priority = getPriority();

    for (auto& name : priority)
    {
        if (has(name))
        {
            available.push_back(name);
        }
    }

    return available;
}

// ===== TRANSLATE PACKAGE =====
std::string resolvePackageName(const std::string& pkg, const std::string& manager)
{
    auto pkgIt = packageMap.find(pkg);

    if (pkgIt != packageMap.end())
    {
        auto mgrIt = pkgIt->second.find(manager);
        if (mgrIt != pkgIt->second.end())
        {
            return mgrIt->second;
        }
    }

    if (manager == "nix-env")
    {
        if (pkg.rfind("nixpkgs.", 0) == 0)
        {
            return pkg;
        }

        return "nixpkgs." + pkg;
    }

    return pkg;
}

// ===== AUTO FLATPAK DISCOVERY =====
std::string autoFlatpakPackage(const std::string& pkg)
{
    std::string output = exec("flatpak search \"" + pkg + "\" | awk 'NR > 1 && NF {print $1; exit}'");
    output = trim(output);

    if (output.empty())
    {
        return pkg;
    }

    return output;
}

// ===== HELP =====
void showHelp()
{
    std::cout << "=========== Swiss Package Manager ===========\n\n";
    std::cout << "Versao: " << VERSION << "\n\n";

    std::cout << "Instalar:\n";
    std::cout << "  swiss <pacote>\n";
    std::cout << "  swiss install <pacote>\n";
    std::cout << "  swiss install <pacote> --auto\n";
    std::cout << "  swiss install <pacote> --flatpak\n";
    std::cout << "  swiss apt <pacote>\n";
    std::cout << "  swiss flatpak <pacote>\n\n";

    std::cout << "Buscar:\n";
    std::cout << "  swiss search <pacote>\n\n";

    std::cout << "Remover:\n";
    std::cout << "  swiss remove <manager> <pacote>\n\n";

    std::cout << "Atualizar:\n";
    std::cout << "  swiss update\n";
    std::cout << "  swiss update <manager>\n";
    std::cout << "  swiss update <manager> <pacote>\n";
    std::cout << "  swiss update <pacote>\n\n";

    std::cout << "Sistema:\n";
    std::cout << "  swiss self-update\n";
    std::cout << "  swiss --version\n";
    std::cout << "  swiss --help\n\n";
}

// ===== BUILD PACKAGE LIST =====
std::string buildPackageList(const std::vector<std::string>& packages, const std::string& manager)
{
    std::string list;

    for (const auto& p : packages)
    {
        std::string translated = resolvePackageName(p, manager);

        if (manager == "flatpak" && translated == p)
        {
            translated = autoFlatpakPackage(p);
        }

        list += translated + " ";
    }

    return list;
}

// ===== INSTALL =====
int installPackages(const std::vector<std::string>& args)
{
    bool autoMode = false;
    std::string forcedManager;
    std::vector<std::string> packages;

    for (size_t i = 0; i < args.size(); i++)
    {
        const std::string& arg = args[i];

        if (arg == "--auto")
        {
            autoMode = true;
        }
        else if (arg == "--apt" || arg == "apt")
        {
            forcedManager = "apt";
        }
        else if (arg == "--pacman" || arg == "pacman")
        {
            forcedManager = "pacman";
        }
        else if (arg == "--dnf" || arg == "dnf")
        {
            forcedManager = "dnf";
        }
        else if (arg == "--emerge" || arg == "emerge")
        {
            forcedManager = "emerge";
        }
        else if (arg == "--xbps" || arg == "xbps")
        {
            forcedManager = "xbps";
        }
        else if (arg == "--nix" || arg == "nix-env" || arg == "nix")
        {
            forcedManager = "nix-env";
        }
        else if (arg == "--yay" || arg == "yay")
        {
            forcedManager = "yay";
        }
        else if (arg == "--paru" || arg == "paru")
        {
            forcedManager = "paru";
        }
        else if (arg == "--pkg" || arg == "pkg")
        {
            forcedManager = "pkg";
        }
        else if (arg == "--flatpak" || arg == "flatpak")
        {
            forcedManager = "flatpak";
        }
        else if (arg == "--snap" || arg == "snap")
        {
            forcedManager = "snap";
        }
        else
        {
            packages.push_back(arg);
        }
    }

    if (packages.empty())
    {
        std::cout << "Uso: swiss <pacote>\n";
        return 1;
    }

    std::vector<std::string> available = getAvailable();

    if (available.empty())
    {
        std::cout << "Nenhum gerenciador encontrado.\n";
        return 1;
    }

    std::cout << "Familia detectada: " << getFamily() << "\n";

    auto tryInstall = [&](const std::string& managerName) -> bool
    {
        Manager* m = getManager(managerName);

        if (!m)
        {
            return false;
        }

        std::string list = buildPackageList(packages, managerName);
        progress("📦 Instalando via " + managerName);

        return system((m->install + list).c_str()) == 0;
    };

    if (!forcedManager.empty())
    {
        if (has(forcedManager))
        {
            if (tryInstall(forcedManager))
            {
                return 0;
            }

            std::cout << "Falhou. Tentando fallback...\n";
        }
    }

    if (autoMode)
    {
        for (const auto& managerName : available)
        {
            if (tryInstall(managerName))
            {
                std::cout << "Sucesso com " << managerName << "\n";
                return 0;
            }
        }

        std::cout << "Falha total.\n";
        return 1;
    }

    std::cout << "Metodos disponiveis:\n";
    for (size_t i = 0; i < available.size(); i++)
    {
        std::cout << i + 1 << ") " << available[i] << "\n";
    }

    std::cout << "\nInstalar automaticamente? (s/y/n): ";
    char c;
    std::cin >> c;

    if (c == 'y' || c == 'Y' || c == 's' || c == 'S')
    {
        for (const auto& managerName : available)
        {
            if (tryInstall(managerName))
            {
                std::cout << "Sucesso com " << managerName << "\n";
                return 0;
            }
        }

        std::cout << "Falha total.\n";
        return 1;
    }

    std::cout << "Escolha: ";
    int opt;
    std::cin >> opt;

    if (opt < 1 || static_cast<size_t>(opt) > available.size())
    {
        std::cout << "Opcao invalida.\n";
        return 1;
    }

    if (tryInstall(available[opt - 1]))
    {
        std::cout << "Sucesso com " << available[opt - 1] << "\n";
        return 0;
    }

    std::cout << "Falha ao instalar.\n";
    return 1;
}

// ===== UPDATE PACKAGE =====
int updatePackage(const std::string& managerName, const std::string& pkg)
{
    Manager* m = getManager(managerName);

    if (!m)
    {
        return 1;
    }

    std::string translated = resolvePackageName(pkg, managerName);

    if (managerName == "flatpak" && translated == pkg)
    {
        translated = autoFlatpakPackage(pkg);
    }

    progress("🔄 Atualizando pacote via " + managerName);
    return system((m->update + translated).c_str());
}

// ===== UPDATE MANAGER =====
int updateAllOfManager(const std::string& managerName)
{
    Manager* m = getManager(managerName);

    if (!m)
    {
        return 1;
    }

    progress("🔄 Atualizando via " + managerName);
    return system(m->update_all.c_str());
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

    if (std::string(argv[1]) == "--version")
    {
        std::cout << "Swiss v" << VERSION << "\n";
        return 0;
    }

    if (command == "help")
    {
        showHelp();
        return 0;
    }

    if (command == argv[1] && !isKnownManager(argv[1]))
    {
        command = "install";
    }

    if (command == "self-update")
    {
        progress("🔄 Atualizando Swiss");
        system("curl -sL https://raw.githubusercontent.com/feroshina/Swiss-Package-Utensil/main/install.sh | bash");
        return 0;
    }

    if (command == "install")
    {
        std::vector<std::string> args;
        size_t start = (resolveCommand(argv[1]) == "install") ? 2 : 1;

        for (int i = static_cast<int>(start); i < argc; i++)
        {
            args.push_back(argv[i]);
        }

        return installPackages(args);
    }

    if (command == "update")
    {
        if (argc == 2)
        {
            status("Atualizando sistema");

            std::vector<std::string> available = getAvailable();
            for (const auto& managerName : available)
            {
                updateAllOfManager(managerName);
            }

            return 0;
        }

        if (argc == 3)
        {
            std::string target = argv[2];

            if (isKnownManager(target))
            {
                return updateAllOfManager(target);
            }

            std::vector<std::string> available = getAvailable();
            for (const auto& managerName : available)
            {
                if (updatePackage(managerName, target) == 0)
                {
                    return 0;
                }
            }

            std::cout << "Nao foi possivel atualizar o pacote.\n";
            return 1;
        }

        if (argc >= 4)
        {
            std::string managerName = argv[2];
            std::string pkg = argv[3];

            if (!isKnownManager(managerName))
            {
                std::cout << "Gerenciador invalido.\n";
                return 1;
            }

            return updatePackage(managerName, pkg);
        }

        return 0;
    }

    if (command == "remove")
    {
        if (argc < 4)
        {
            std::cout << "Uso: swiss remove <manager> <pacote>\n";
            return 1;
        }

        std::string managerName = argv[2];
        std::string pkg = argv[3];

        Manager* m = getManager(managerName);
        if (!m)
        {
            std::cout << "Gerenciador invalido.\n";
            return 1;
        }

        progress("🗑️ Removendo pacote");
        return system((m->remove + pkg).c_str());
    }

    if (command == "search")
    {
        if (argc < 3)
        {
            std::cout << "Uso: swiss search <pacote>\n";
            return 1;
        }

        std::string pkg = argv[2];

        for (const auto& m : managers)
        {
            if (has(m.name))
            {
                std::cout << "\n[" << m.name << "]\n";
                std::cout << exec(m.search + pkg);
            }
        }

        return 0;
    }

    showHelp();
    return 0;
}
