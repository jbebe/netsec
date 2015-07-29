#include "Netsec/Core/Netsec.hpp"

int main(int argc, char** argv) {
    system("clear");
    Netsec ns{"wlan0"};
    ns.start();
    
    return 0;
}

