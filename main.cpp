
#include "Netsec.hpp"

int main(int argc, char** argv) {
    
    NETSEC::Netsec ns{{"wlan0"}};
    ns.start();
   
    return 0;
}

