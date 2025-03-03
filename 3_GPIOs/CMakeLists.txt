# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# Incluir o arquivo de configuração do ESP-IDF
set(IDF_PATH "/home/schardong/esp/esp-idf")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Definir o nome do projeto
project(ESP32_LORATION)

# Incluir o diretório main e os componentes
#add_subdirectory(main)
#add_subdirectory(tests)

# Se houver outros componentes personalizados, adicione-os aqui
# add_subdirectory(components)
