#include "replit_core.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    replit::ReplitEngine engine;
    
    if (argc == 1) {
        // Start REPL
        engine.start_repl();
    } else if (argc == 2) {
        // Run file
        std::string filename = argv[1];
        if (!engine.run_file(filename)) {
            return 1;
        }
    } else {
        std::cerr << "Usage: " << argv[0] << " [filename]" << std::endl;
        return 1;
    }
    
    return 0;
}