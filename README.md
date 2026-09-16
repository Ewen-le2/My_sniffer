cmake -S . -B build

cd build

cmake --build .

sudo ./My_sniffer


windows:

cmake -S . -B build -A x64
cmake --build build --config Debug||Release


You need to install npcap for the program to work, you can find it [HERE](https://npcap.com/#download)