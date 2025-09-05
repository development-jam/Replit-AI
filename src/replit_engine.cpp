#include "replit_core.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace replit {

std::string ReplitEngine::run_code(const std::string& code) {
    std::ostringstream output;
    std::streambuf* orig = std::cout.rdbuf();
    std::cout.rdbuf(output.rdbuf());
    
    VM::InterpretResult result = vm.interpret(code);
    
    std::cout.rdbuf(orig);
    
    if (result == VM::InterpretResult::COMPILE_ERROR) {
        return "Compile error occurred";
    } else if (result == VM::InterpretResult::RUNTIME_ERROR) {
        return "Runtime error occurred";
    }
    
    return output.str();
}

bool ReplitEngine::run_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return false;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    VM::InterpretResult result = vm.interpret(source);
    
    if (result == VM::InterpretResult::COMPILE_ERROR) {
        std::cerr << "Compile error in file: " << filename << std::endl;
        return false;
    }
    if (result == VM::InterpretResult::RUNTIME_ERROR) {
        std::cerr << "Runtime error in file: " << filename << std::endl;
        return false;
    }
    
    return true;
}

void ReplitEngine::start_repl() {
    std::cout << "Replit Programming Language v1.0" << std::endl;
    std::cout << "Type 'exit' to quit" << std::endl;
    
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        
        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;
        
        VM::InterpretResult result = vm.interpret(line);
        
        if (result == VM::InterpretResult::COMPILE_ERROR) {
            std::cout << "Compile error" << std::endl;
        } else if (result == VM::InterpretResult::RUNTIME_ERROR) {
            std::cout << "Runtime error" << std::endl;
        }
    }
}

}