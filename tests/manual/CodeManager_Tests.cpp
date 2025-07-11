// CodeManagerTests.cpp
#include "../../src/CodeManager.h"

int main() {
    if (CodeManager codeManager; codeManager.init())
        codeManager.run();
    return EXIT_SUCCESS;
}
