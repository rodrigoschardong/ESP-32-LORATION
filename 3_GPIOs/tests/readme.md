sudo apt-get install cpputest

# Clone a versão 4.4.4 do ESP-IDF no diretório esp
cd ~
git clone -b v4.4.4 --recursive https://github.com/espressif/esp-idf.git esp

cd ~/esp
# Execute o script de instalação
./install.sh


# Configurar o ambiente do ESP-IDF
. ~/esp/export.sh

cd ~/esp
git clone --branch v4.4.4 --single-branch https://github.com/espressif/esp-idf.git