#include "CodeManager.h"

int main() {
    if (CodeManager codeManager; codeManager.init())
        codeManager.run();
    //codeManager.shutdown();
    return EXIT_SUCCESS;
}
