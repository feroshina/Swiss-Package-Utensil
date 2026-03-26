#!/bin/bash

set -e

# ===== CONFIG =====
NAME="swiss"
SRC="swiss.cpp"
REPO="https://raw.githubusercontent.com/feroshina/Swiss-Package-Utensil/main"
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

# ===== DETECTAR COMPILADOR =====
if command -v clang++ &> /dev/null
then
    COMPILER="clang++"
elif command -v g++ &> /dev/null
then
    COMPILER="g++"
else
    echo "Erro: nenhum compilador encontrado (clang++ ou g++)"
    echo "Instale com:"
    echo "  Ubuntu/Debian: sudo apt install clang ou g++"
    echo "  Arch: sudo pacman -S clang ou gcc"
    echo "  Fedora: sudo dnf install clang ou gcc-c++"
    exit 1
fi

echo "Usando compilador: $COMPILER"

# ===== BAIXAR CÓDIGO =====
echo "Baixando código..."
$DOWNLOAD "$REPO/$SRC" > "$SRC"

# ===== VERIFICAR HTML =====
if grep -q "<html" "$SRC"
then
    echo "Erro: arquivo inválido (HTML detectado)"
    exit 1
fi

# ===== COMPILAR =====
echo "Compilando..."
$COMPILER -O2 -std=c++17 "$SRC" -o "$NAME"

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
fiecho "Baixando código..."
$DOWNLOAD "$REPO/$SRC" > "$SRC"

# ===== VERIFICAR SE NÃO É HTML =====
if grep -q "<html" "$SRC"
then
    echo "Erro: arquivo baixado não é código fonte (HTML detectado)"
    exit 1
fi

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
fiecho "Baixando código..."
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
