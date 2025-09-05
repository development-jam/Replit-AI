#include "replit_core.hpp"
#include <cctype>

namespace replit {

Lexer::Lexer(const std::string& source) : source(source) {}

bool Lexer::is_at_end() {
    return current >= source.length();
}

char Lexer::advance() {
    if (is_at_end()) return '\0';
    char c = source[current++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

char Lexer::peek() {
    if (is_at_end()) return '\0';
    return source[current];
}

char Lexer::peek_next() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (is_at_end() || source[current] != expected) return false;
    current++;
    column++;
    return true;
}

Token Lexer::make_token(TokenType type) {
    return {type, source.substr(start, current - start), line, static_cast<int>(column - (current - start))};
}

Token Lexer::error_token(const std::string& message) {
    return {TokenType::ERROR, message, line, column};
}

void Lexer::skip_whitespace() {
    while (!is_at_end()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
        } else if (c == '/' && peek_next() == '/') {
            while (peek() != '\n' && !is_at_end()) advance();
        } else {
            break;
        }
    }
}

Token Lexer::string_token() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') line++;
        advance();
    }
    
    if (is_at_end()) return error_token("Unterminated string");
    
    advance(); // closing quote
    return make_token(TokenType::STRING);
}

Token Lexer::number_token() {
    while (std::isdigit(peek())) advance();
    
    if (peek() == '.' && std::isdigit(peek_next())) {
        advance(); // consume '.'
        while (std::isdigit(peek())) advance();
    }
    
    return make_token(TokenType::NUMBER);
}

TokenType Lexer::identifier_type() {
    std::string text = source.substr(start, current - start);
    
    if (text == "let") return TokenType::LET;
    if (text == "fn") return TokenType::FN;
    if (text == "if") return TokenType::IF;
    if (text == "else") return TokenType::ELSE;
    if (text == "while") return TokenType::WHILE;
    if (text == "for") return TokenType::FOR;
    if (text == "return") return TokenType::RETURN;
    if (text == "print") return TokenType::PRINT;
    if (text == "true") return TokenType::TRUE;
    if (text == "false") return TokenType::FALSE;
    if (text == "nil") return TokenType::NIL;
    if (text == "and") return TokenType::AND;
    if (text == "or") return TokenType::OR;
    
    return TokenType::IDENTIFIER;
}

Token Lexer::identifier_token() {
    while (std::isalnum(peek()) || peek() == '_') advance();
    return make_token(identifier_type());
}

Token Lexer::scan_token() {
    skip_whitespace();
    start = current;
    
    if (is_at_end()) return make_token(TokenType::EOF_TOKEN);
    
    char c = advance();
    
    if (std::isalpha(c) || c == '_') return identifier_token();
    if (std::isdigit(c)) return number_token();
    
    switch (c) {
        case '(': return make_token(TokenType::LPAREN);
        case ')': return make_token(TokenType::RPAREN);
        case '{': return make_token(TokenType::LBRACE);
        case '}': return make_token(TokenType::RBRACE);
        case ';': return make_token(TokenType::SEMICOLON);
        case ',': return make_token(TokenType::COMMA);
        case '+': return make_token(TokenType::PLUS);
        case '-': return make_token(TokenType::MINUS);
        case '*': return make_token(TokenType::MULTIPLY);
        case '/': return make_token(TokenType::DIVIDE);
        case '%': return make_token(TokenType::MODULO);
        case '!':
            return make_token(match('=') ? TokenType::NOT_EQUAL : TokenType::NOT);
        case '=':
            return make_token(match('=') ? TokenType::EQUAL : TokenType::ASSIGN);
        case '<':
            return make_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>':
            return make_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '"': return string_token();
        case '\n': return make_token(TokenType::NEWLINE);
    }
    
    return error_token("Unexpected character");
}

}