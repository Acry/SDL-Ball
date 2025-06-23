#include "CodeManager.h"

int main() {
    CodeManager codeManager;
    codeManager.initialize();
    codeManager.run();
    codeManager.shutdown();
    return EXIT_SUCCESS;
}
