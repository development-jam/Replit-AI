#include "replit_core.hpp"
#include <iostream>
#include <stdexcept>

namespace replit {

VM::VM() {
    reset_stack();
}

VM::~VM() {
    // Cleanup if needed
}

void VM::reset_stack() {
    stack.clear();
}

void VM::push(Value value) {
    stack.push_back(value);
}

Value VM::pop() {
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow");
    }
    Value value = stack.back();
    stack.pop_back();
    return value;
}

Value VM::peek(int distance) {
    if (distance >= static_cast<int>(stack.size())) {
        throw std::runtime_error("Stack peek out of bounds");
    }
    return stack[stack.size() - 1 - distance];
}

bool VM::is_falsey(Value value) {
    if (std::holds_alternative<std::nullptr_t>(value)) return true;
    if (std::holds_alternative<bool>(value)) return !std::get<bool>(value);
    return false;
}

bool VM::values_equal(Value a, Value b) {
    if (a.index() != b.index()) return false;
    
    if (std::holds_alternative<double>(a)) {
        return std::get<double>(a) == std::get<double>(b);
    }
    if (std::holds_alternative<std::string>(a)) {
        return std::get<std::string>(a) == std::get<std::string>(b);
    }
    if (std::holds_alternative<bool>(a)) {
        return std::get<bool>(a) == std::get<bool>(b);
    }
    if (std::holds_alternative<std::nullptr_t>(a)) {
        return true; // Both are nil
    }
    
    return false;
}

void VM::runtime_error(const std::string& message) {
    std::cerr << "Runtime error: " << message << std::endl;
    
    size_t instruction = ip - chunk->code.data() - 1;
    if (instruction < chunk->lines.size()) {
        std::cerr << "[line " << chunk->lines[instruction] << "] in script" << std::endl;
    }
    
    reset_stack();
}

void print_value(Value value) {
    if (std::holds_alternative<double>(value)) {
        double num = std::get<double>(value);
        if (num == static_cast<int>(num)) {
            std::cout << static_cast<int>(num);
        } else {
            std::cout << num;
        }
    } else if (std::holds_alternative<std::string>(value)) {
        std::cout << std::get<std::string>(value);
    } else if (std::holds_alternative<bool>(value)) {
        std::cout << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::nullptr_t>(value)) {
        std::cout << "nil";
    }
}

VM::InterpretResult VM::run() {
    #define READ_BYTE() (*ip++)
    #define READ_CONSTANT() (chunk->constants[READ_BYTE()])
    #define BINARY_OP(value_type, op) \
        do { \
            if (!std::holds_alternative<double>(peek(0)) || \
                !std::holds_alternative<double>(peek(1))) { \
                runtime_error("Operands must be numbers"); \
                return InterpretResult::RUNTIME_ERROR; \
            } \
            double b = std::get<double>(pop()); \
            double a = std::get<double>(pop()); \
            push(value_type(a op b)); \
        } while (false)
    
    try {
        while (true) {
            uint8_t instruction = READ_BYTE();
            
            switch (static_cast<OpCode>(instruction)) {
                case OpCode::OP_CONSTANT: {
                    Value constant = READ_CONSTANT();
                    push(constant);
                    break;
                }
                case OpCode::OP_ADD: {
                    if (std::holds_alternative<std::string>(peek(0)) || 
                        std::holds_alternative<std::string>(peek(1))) {
                        std::string b = std::holds_alternative<std::string>(peek(0)) ? 
                                       std::get<std::string>(pop()) : 
                                       std::to_string(std::get<double>(pop()));
                        std::string a = std::holds_alternative<std::string>(peek(0)) ? 
                                       std::get<std::string>(pop()) : 
                                       std::to_string(std::get<double>(pop()));
                        push(a + b);
                    } else {
                        BINARY_OP(Value, +);
                    }
                    break;
                }
                case OpCode::OP_SUBTRACT: BINARY_OP(Value, -); break;
                case OpCode::OP_MULTIPLY: BINARY_OP(Value, *); break;
                case OpCode::OP_DIVIDE: BINARY_OP(Value, /); break;
                case OpCode::OP_NEGATE: {
                    if (!std::holds_alternative<double>(peek())) {
                        runtime_error("Operand must be a number");
                        return InterpretResult::RUNTIME_ERROR;
                    }
                    push(-std::get<double>(pop()));
                    break;
                }
                case OpCode::OP_NOT:
                    push(is_falsey(pop()));
                    break;
                case OpCode::OP_EQUAL: {
                    Value b = pop();
                    Value a = pop();
                    push(values_equal(a, b));
                    break;
                }
                case OpCode::OP_GREATER: BINARY_OP(bool, >); break;
                case OpCode::OP_LESS: BINARY_OP(bool, <); break;
                case OpCode::OP_PRINT: {
                    print_value(pop());
                    std::cout << std::endl;
                    break;
                }
                case OpCode::OP_POP: pop(); break;
                case OpCode::OP_RETURN: {
                    return InterpretResult::OK;
                }
                default:
                    runtime_error("Unknown opcode");
                    return InterpretResult::RUNTIME_ERROR;
            }
        }
    } catch (const std::exception& e) {
        runtime_error(e.what());
        return InterpretResult::RUNTIME_ERROR;
    }
    
    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

VM::InterpretResult VM::interpret(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens;
    
    // Tokenize
    while (true) {
        Token token = lexer.scan_token();
        tokens.push_back(token);
        if (token.type == TokenType::EOF_TOKEN) break;
    }
    
    Chunk chunk;
    Parser parser;
    
    if (!parser.compile(tokens, &chunk)) {
        return InterpretResult::COMPILE_ERROR;
    }
    
    this->chunk = &chunk;
    this->ip = chunk.code.data();
    
    InterpretResult result = run();
    return result;
}

}