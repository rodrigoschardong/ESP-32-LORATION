#!/bin/bash

# Altere a permissão desse arquivo com: chmod +x run_tests.sh

# Nome do arquivo de código fonte
SOURCE_FILE="test.cpp"

# Nome do executável gerado
OUTPUT_FILE="test"

# Caminhos para os cabeçalhos e bibliotecas
INCLUDE_PATH="/usr/include/CppUTest"
LIB_PATH="/usr/lib/x86_64-linux-gnu"
MAIN_DIR="../main"  # Caminho onde está o arquivo main.c com a implementação da função Timeout_Expired

# Verifica se os arquivos existem
if [ ! -f "$SOURCE_FILE" ]; then
    echo "Erro: Arquivo $SOURCE_FILE não encontrado."
    exit 1
fi

if [ ! -f "$MAIN_DIR/main.c" ]; then
    echo "Erro: Arquivo $MAIN_DIR/main.h não encontrado."
    exit 1
fi

# Verifica se o diretório de inclusão e bibliotecas existe
if [ ! -d "$INCLUDE_PATH" ] || [ ! -d "$LIB_PATH" ]; then
    echo "Erro: Diretórios de inclusão ou bibliotecas do CppUTest não encontrados."
    exit 1
fi

# Remove o executável antigo
if [ -f "$OUTPUT_FILE" ]; then
    rm "$OUTPUT_FILE"
fi

# Compila o código-fonte
echo "Compilando o arquivo $SOURCE_FILE..."
g++ -g -Wall -I"$INCLUDE_PATH" -I"$MAIN_DIR" -L"$LIB_PATH" "$SOURCE_FILE" "$MAIN_DIR/main.c" -lCppUTest -lCppUTestExt -o "$OUTPUT_FILE"

# Verifica se a compilação foi bem-sucedida
if [ $? -eq 0 ]; then
    echo "Compilação bem-sucedida. Executando testes..."
    # Executa o arquivo gerado
    ./"$OUTPUT_FILE"
else
    echo "Erro durante a compilação."
fi
