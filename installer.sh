#!/bin/bash

set -e

NAME="swiss"
SRC="swiss.cpp"
REPO="https://raw.githubusercontent.com/feroshina/Swiss-Package-Utensil/main"

is_termux() {
    [[ -n "${PREFIX:-}" && "$PREFIX" == /data/data/com.termux/files/usr* ]] || [[ -n "${TERMUX_VERSION:-}" ]]
}

detect_compiler() {
    if command -v clang++ >/dev/null 2>&1; then
        echo "clang++"
        return 0
    fi

    if command -v g++ >/dev/null 2>&1; then
        echo "g++"
        return 0
    fi

    return 1
}

detect_downloader() {
    if command -v curl >/dev/null 2>&1; then
        echo "curl"
        return 0
    fi

    if command -v wget >/dev/null 2>&1; then
        echo "wget"
        return 0
    fi

    return 1
}

download_file() {
    local url="$1"
    local out="$2"

    if command -v curl >/dev/null 2>&1; then
        curl -fsSL "$url" -o "$out"
        return
    fi

    if command -v wget >/dev/null 2>&1; then
        wget -qO "$out" "$url"
        return
    fi

    echo "Erro: precisa de curl ou wget."
    exit 1
}

cleanup_old_install() {
    local targets=(
        "/bin/$NAME"
        "/usr/bin/$NAME"
        "/usr/local/bin/$NAME"
        "$HOME/.local/bin/$NAME"
        "$PREFIX/bin/$NAME"
    )

    for path in "${targets[@]}"; do
        if [ -e "$path" ] || [ -L "$path" ]; then
            if [ -w "$path" ] || [ ! -e "$path" ]; then
                rm -f "$path" 2>/dev/null || true
            else
                sudo rm -f "$path" 2>/dev/null || true
            fi
        fi
    done
}

install_target() {
    if is_termux; then
        mkdir -p "$PREFIX/bin"
        echo "$PREFIX/bin/$NAME"
    else
        echo "/usr/local/bin/$NAME"
    fi
}

echo "== Swiss Installer =="

if ! compiler="$(detect_compiler)"; then
    echo "Erro: nenhum compilador C++ encontrado."
    if is_termux; then
        echo "No Termux, instale com:"
        echo "  pkg install clang"
    else
        echo "Instale clang++ ou g++ antes de continuar."
    fi
    exit 1
fi

echo "Usando compilador: $compiler"

if ! is_termux && ! command -v sudo >/dev/null 2>&1; then
    echo "Aviso: sudo não encontrado. A instalação em local do sistema pode falhar."
fi

workdir="$(mktemp -d)"
trap 'rm -rf "$workdir"' EXIT

cd "$workdir"

echo "Baixando código..."
download_file "$REPO/$SRC" "$SRC"

if grep -q "<html" "$SRC"; then
    echo "Erro: o arquivo baixado parece ser HTML, não código-fonte."
    exit 1
fi

echo "Compilando..."
"$compiler" -O2 -std=c++17 "$SRC" -o "$NAME"

if [ ! -x "./$NAME" ]; then
    echo "Erro: falha ao gerar o binário."
    exit 1
fi

target="$(install_target)"

echo "Removendo instalação antiga, se existir..."
cleanup_old_install

echo "Instalando em: $target"
if is_termux; then
    cp "$NAME" "$target"
    chmod +x "$target"
else
    if [ "$target" = "/usr/local/bin/$NAME" ]; then
        if [ "$(id -u)" -eq 0 ]; then
            cp "$NAME" "$target"
            chmod +x "$target"
        else
            sudo cp "$NAME" "$target"
            sudo chmod +x "$target"
        fi
    else
        cp "$NAME" "$target"
        chmod +x "$target"
    fi
fi

echo ""
echo "✔ Swiss instalado com sucesso!"
echo "Use: swiss --help"
