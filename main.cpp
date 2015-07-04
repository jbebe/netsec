#include "Netsec.hpp"

int main(int argc, char** argv) {
    NETSEC::Netsec ns({"eth0"});
    ns.start();
    return 0;
}
