# Definir a versão mínima do CMake
cmake_minimum_required(VERSION 3.5)

# Nome do projeto de testes
project(esp32_timeout_tests)

# Definir o caminho para o ESP-IDF (confirme se esse caminho está correto)
set(IDF_PATH "/home/schardong/esp/esp-idf")

# Incluir o arquivo de configuração do ESP-IDF
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Incluir os diretórios necessários
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../main)
include_directories(/usr/include/CppUTest)

# Incluir os componentes do ESP-IDF necessários (FreeRTOS e outras bibliotecas)
set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../components)

# Adicionar os arquivos de teste
file(GLOB TEST_SOURCES "*.cpp")

# Criar o executável do teste
add_executable(test_main ${TEST_SOURCES})

# Linkar o executável com as bibliotecas do ESP-IDF e CppUTest
target_link_libraries(test_main 
    freertos    # FreeRTOS, necessário para o seu código
    CppUTest    # Biblioteca de testes
)
