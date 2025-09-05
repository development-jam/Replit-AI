#include "replit_core.hpp"
#include <iostream>

namespace replit {

void Chunk::write(uint8_t byte, int line) {
    code.push_back(byte);
    lines.push_back(line);
}

int Chunk::add_constant(Value value) {
    constants.push_back(value);
    return static_cast<int>(constants.size() - 1);
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::peek() {
    return tokens[current];
}

bool Parser::check(TokenType type) {
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

Token Parser::advance() {
    if (current < tokens.size() - 1) current++;
    return previous();
}

void Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) {
        advance();
        return;
    }
    error_at(peek(), message);
}

void Parser::error_at(Token token, const std::string& message) {
    if (panic_mode) return;
    panic_mode = true;
    std::cerr << "[line " << token.line << "] Error";
    
    if (token.type == TokenType::EOF_TOKEN) {
        std::cerr << " at end";
    } else if (token.type == TokenType::ERROR) {
        // Nothing
    } else {
        std::cerr << " at '" << token.lexeme << "'";
    }
    
    std::cerr << ": " << message << std::endl;
    had_error = true;
}

void Parser::error(const std::string& message) {
    error_at(previous(), message);
}

void Parser::synchronize() {
    panic_mode = false;
    
    while (peek().type != TokenType::EOF_TOKEN) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::FN:
            case TokenType::LET:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

void Parser::emit_byte(uint8_t byte) {
    compiling_chunk->write(byte, previous().line);
}

void Parser::emit_bytes(uint8_t byte1, uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
}

void Parser::emit_return() {
    emit_byte(static_cast<uint8_t>(OpCode::OP_RETURN));
}

uint8_t Parser::make_constant(Value value) {
    int constant = compiling_chunk->add_constant(value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk");
        return 0;
    }
    return static_cast<uint8_t>(constant);
}

void Parser::emit_constant(Value value) {
    emit_bytes(static_cast<uint8_t>(OpCode::OP_CONSTANT), make_constant(value));
}

void Parser::primary() {
    if (match(TokenType::TRUE)) {
        emit_constant(true);
        return;
    }
    
    if (match(TokenType::FALSE)) {
        emit_constant(false);
        return;
    }
    
    if (match(TokenType::NIL)) {
        emit_constant(nullptr);
        return;
    }
    
    if (match(TokenType::NUMBER)) {
        double value = std::stod(previous().lexeme);
        emit_constant(value);
        return;
    }
    
    if (match(TokenType::STRING)) {
        std::string value = previous().lexeme;
        // Remove quotes
        value = value.substr(1, value.length() - 2);
        emit_constant(value);
        return;
    }
    
    if (match(TokenType::LPAREN)) {
        expression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return;
    }
    
    error("Expected expression");
}

void Parser::unary() {
    if (match(TokenType::NOT) || match(TokenType::MINUS)) {
        TokenType operator_type = previous().type;
        unary();
        
        switch (operator_type) {
            case TokenType::MINUS: emit_byte(static_cast<uint8_t>(OpCode::OP_NEGATE)); break;
            case TokenType::NOT: emit_byte(static_cast<uint8_t>(OpCode::OP_NOT)); break;
            default: return;
        }
        return;
    }
    
    primary();
}

void Parser::factor() {
    unary();
    
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) || match(TokenType::MODULO)) {
        TokenType operator_type = previous().type;
        unary();
        
        switch (operator_type) {
            case TokenType::MULTIPLY: emit_byte(static_cast<uint8_t>(OpCode::OP_MULTIPLY)); break;
            case TokenType::DIVIDE: emit_byte(static_cast<uint8_t>(OpCode::OP_DIVIDE)); break;
            default: break;
        }
    }
}

void Parser::term() {
    factor();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        TokenType operator_type = previous().type;
        factor();
        
        switch (operator_type) {
            case TokenType::PLUS: emit_byte(static_cast<uint8_t>(OpCode::OP_ADD)); break;
            case TokenType::MINUS: emit_byte(static_cast<uint8_t>(OpCode::OP_SUBTRACT)); break;
            default: break;
        }
    }
}

void Parser::comparison() {
    term();
    
    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || 
           match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {
        TokenType operator_type = previous().type;
        term();
        
        switch (operator_type) {
            case TokenType::GREATER: emit_byte(static_cast<uint8_t>(OpCode::OP_GREATER)); break;
            case TokenType::GREATER_EQUAL: 
                emit_bytes(static_cast<uint8_t>(OpCode::OP_LESS), static_cast<uint8_t>(OpCode::OP_NOT));
                break;
            case TokenType::LESS: emit_byte(static_cast<uint8_t>(OpCode::OP_LESS)); break;
            case TokenType::LESS_EQUAL:
                emit_bytes(static_cast<uint8_t>(OpCode::OP_GREATER), static_cast<uint8_t>(OpCode::OP_NOT));
                break;
            default: break;
        }
    }
}

void Parser::equality() {
    comparison();
    
    while (match(TokenType::NOT_EQUAL) || match(TokenType::EQUAL)) {
        TokenType operator_type = previous().type;
        comparison();
        
        switch (operator_type) {
            case TokenType::NOT_EQUAL:
                emit_bytes(static_cast<uint8_t>(OpCode::OP_EQUAL), static_cast<uint8_t>(OpCode::OP_NOT));
                break;
            case TokenType::EQUAL: emit_byte(static_cast<uint8_t>(OpCode::OP_EQUAL)); break;
            default: break;
        }
    }
}

void Parser::and_expression() {
    equality();
    // TODO: Implement short-circuit evaluation for AND
}

void Parser::or_expression() {
    and_expression();
    // TODO: Implement short-circuit evaluation for OR
}

void Parser::expression() {
    or_expression();
}

void Parser::expression_statement() {
    expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    emit_byte(static_cast<uint8_t>(OpCode::OP_POP));
}

void Parser::print_statement() {
    expression();
    consume(TokenType::SEMICOLON, "Expected ';' after value");
    emit_byte(static_cast<uint8_t>(OpCode::OP_PRINT));
}

void Parser::statement() {
    if (match(TokenType::PRINT)) {
        print_statement();
    } else {
        expression_statement();
    }
}

void Parser::var_declaration() {
    // TODO: Implement variable declarations
}

void Parser::declaration() {
    if (match(TokenType::LET)) {
        var_declaration();
    } else {
        statement();
    }
    
    if (panic_mode) synchronize();
}


bool Parser::compile(const std::vector<Token>& tokens, Chunk* chunk) {
    this->tokens = tokens;
    this->compiling_chunk = chunk;
    this->current = 0;
    this->had_error = false;
    this->panic_mode = false;
    
    while (!check(TokenType::EOF_TOKEN)) {
        // Skip newlines at the top level
        if (match(TokenType::NEWLINE)) {
            continue;
        }
        declaration();
    }
    
    emit_return();
    return !had_error;
}

}