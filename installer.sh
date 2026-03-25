#!/bin/bash

set -e

# ===== CONFIG =====
NAME="swiss"
SRC="swiss.cpp"
REPO="https://raw.githubusercontent.com/SEU_USUARIO/swiss/main"
DEST="/usr/local/bin/$NAME"

echo "== Swiss Installer =="

# ===== DETECTAR DOWNLOAD =====
if command -v curl &> /dev/null
then
    DOWNLOAD="curl -sL"
elif command -v wget &> /dev/null
then
    DOWNLOAD="wget -qO-"
else
    echo "Erro: precisa de curl ou wget"
    exit 1
fi

# ===== VERIFICAR COMPILADOR =====
if ! command -v g++ &> /dev/null
then
    echo "Erro: g++ não encontrado"
    echo "Instale com:"
    echo "  Ubuntu/Debian: sudo apt install g++"
    echo "  Arch: sudo pacman -S gcc"
    echo "  Fedora: sudo dnf install gcc-c++"
    exit 1
fi

# ===== BAIXAR CÓDIGO =====
echo "Baixando código..."
$DOWNLOAD "$REPO/$SRC" > "$SRC"

# ===== COMPILAR =====
echo "Compilando..."
g++ -O2 -std=c++17 "$SRC" -o "$NAME"

# ===== PERMISSÃO =====
chmod +x "$NAME"

# ===== INSTALAR =====
echo "Instalando em $DEST..."
sudo mv "$NAME" "$DEST"

# ===== LIMPEZA =====
rm -f "$SRC"

# ===== VERIFICAÇÃO =====
if command -v swiss &> /dev/null
then
    echo ""
    echo "✔ Swiss instalado com sucesso!"
    echo "Use: swiss --help"
else
    echo "Erro na instalação."
fi
