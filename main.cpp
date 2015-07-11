#include "Netsec/Netsec.hpp"

int main(int argc, char** argv) {
    Netsec ns{"wlan0"};
    ns.start();
    
    return 0;
}

