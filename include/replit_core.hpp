#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <variant>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>

namespace replit {

enum class TokenType {
    // Literals
    NUMBER, STRING, IDENTIFIER, CHAR,
    
    // Keywords
    CLASS, STRUCT, ENUM, INTERFACE, NAMESPACE, IMPORT, EXPORT,
    CONST, VAR, LET, FN, LAMBDA, ASYNC, AWAIT,
    IF, ELSE, ELIF, SWITCH, CASE, DEFAULT,
    WHILE, FOR, FOREACH, LOOP, BREAK, CONTINUE,
    RETURN, YIELD, THROW, TRY, CATCH, FINALLY,
    TRUE, FALSE, NULL_TOK, UNDEFINED,
    
    // Types
    INT, FLOAT, DOUBLE, BOOL, STRING_TYPE, CHAR_TYPE,
    ARRAY, MAP, SET, LIST, TUPLE, VECTOR,
    
    // Access modifiers
    PUBLIC, PRIVATE, PROTECTED, STATIC, FINAL, ABSTRACT,
    
    // Operators
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO, POWER,
    ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, MULT_ASSIGN, DIV_ASSIGN,
    EQUAL, NOT_EQUAL, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    AND, OR, NOT, XOR, BIT_AND, BIT_OR, BIT_XOR, BIT_NOT,
    LEFT_SHIFT, RIGHT_SHIFT, INCREMENT, DECREMENT,
    
    // Delimiters
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    SEMICOLON, COMMA, DOT, ARROW, DOUBLE_COLON, QUESTION,
    
    // Graphics/Game keywords
    WINDOW, CANVAS, SPRITE, TEXTURE, SHADER, MESH,
    RENDER, DRAW, FILL, STROKE, COLOR, RGB, RGBA,
    INPUT, MOUSE, KEYBOARD, GAMEPAD, TOUCH,
    COLLISION, PHYSICS, TRANSFORM, VECTOR2, VECTOR3,
    ANIMATION, TWEEN, TIMER, SOUND, MUSIC,
    
    // System keywords
    FILE, DIRECTORY, NETWORK, HTTP, TCP, UDP,
    THREAD, MUTEX, SEMAPHORE, ATOMIC,
    
    NEWLINE, EOF_TOKEN, ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

// Forward declarations
class ReplitObject;
class ReplitClass;
class ReplitFunction;
class Window;
class Sprite;
class Vector2D;
class Color;

using Value = std::variant<
    double, float, int64_t, 
    std::string, char, bool, std::nullptr_t,
    std::shared_ptr<ReplitObject>,
    std::shared_ptr<ReplitClass>,
    std::shared_ptr<ReplitFunction>,
    std::shared_ptr<Window>,
    std::shared_ptr<Sprite>,
    std::shared_ptr<Vector2D>,
    std::shared_ptr<Color>,
    std::vector<Value>,
    std::unordered_map<std::string, Value>
>;

enum class OpCode {
    OP_CONSTANT, OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE,
    OP_NEGATE, OP_NOT, OP_EQUAL, OP_GREATER, OP_LESS,
    OP_PRINT, OP_POP, OP_DEFINE_GLOBAL, OP_GET_GLOBAL,
    OP_SET_GLOBAL, OP_JUMP_IF_FALSE, OP_JUMP, OP_LOOP,
    OP_CALL, OP_RETURN, OP_HALT
};

class Chunk {
public:
    std::vector<uint8_t> code;
    std::vector<int> lines;
    std::vector<Value> constants;
    
    void write(uint8_t byte, int line);
    int add_constant(Value value);
};

class Lexer {
private:
    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;
    
    bool is_at_end();
    char advance();
    char peek();
    char peek_next();
    bool match(char expected);
    Token make_token(TokenType type);
    Token error_token(const std::string& message);
    void skip_whitespace();
    Token string_token();
    Token number_token();
    Token identifier_token();
    TokenType identifier_type();
    
public:
    Lexer(const std::string& source);
    Token scan_token();
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;
    Chunk* compiling_chunk = nullptr;
    bool had_error = false;
    bool panic_mode = false;
    
    Token previous();
    Token peek();
    bool check(TokenType type);
    bool match(TokenType type);
    Token advance();
    void consume(TokenType type, const std::string& message);
    void error_at(Token token, const std::string& message);
    void error(const std::string& message);
    void synchronize();
    
    void emit_byte(uint8_t byte);
    void emit_bytes(uint8_t byte1, uint8_t byte2);
    void emit_return();
    uint8_t make_constant(Value value);
    void emit_constant(Value value);
    
    void expression();
    void or_expression();
    void and_expression();
    void equality();
    void comparison();
    void term();
    void factor();
    void unary();
    void primary();
    void statement();
    void print_statement();
    void expression_statement();
    void declaration();
    void var_declaration();
    
public:
    bool compile(const std::vector<Token>& tokens, Chunk* chunk);
};

class VM {
private:
    Chunk* chunk = nullptr;
    uint8_t* ip = nullptr;
    std::vector<Value> stack;
    std::unordered_map<std::string, Value> globals;
    
    void reset_stack();
    void push(Value value);
    Value pop();
    Value peek(int distance = 0);
    bool is_falsey(Value value);
    bool values_equal(Value a, Value b);
    void runtime_error(const std::string& message);
    
public:
    enum class InterpretResult {
        OK, COMPILE_ERROR, RUNTIME_ERROR
    };
    
    VM();
    ~VM();
    InterpretResult interpret(const std::string& source);
    InterpretResult run();
};

class ReplitEngine {
public:
    VM vm;
    
    std::string run_code(const std::string& code);
    bool run_file(const std::string& filename);
    void start_repl();
};

}