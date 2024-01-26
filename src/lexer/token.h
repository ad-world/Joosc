#include <string>

enum TokenType {
    IF,
    WHILE,
    FOR,
    EXTENDS,
    IMPLEMENTS,
    PUBLIC, 
    PROTECTED,
    PRIVATE,
    STATIC,
    ABSTRACT,
    THIS,
    VOID,
    FINAL,
    IMPORT,
    CLASS,
    // might need to look at this again
    IMPORTED_PACKAGE,
    COMPOUND_NAME,
    PACKAGE,
    INTERFACE,
    OPENING_BRACE,
    CLOSING_BRACE,
    OPENING_PAREN,
    CLOSING_PAREN,
    OPENING_BRACKET,
    CLOSING_BRACKET,
    SEMI_COLON,
    DOT,
    IDENTIFIER,
    NATIVE,
    ASSIGNMENT,
    RETURN,
    // types
    INT,
    BOOLEAN,
    CHAR, 
    BYTE,
    SHORT,
    ARRAY,
    TRUE,
    FALSE,
    STRING_LITERAL,
    INTEGER,
    NULL_TOKEN,
    CHAR_LITERAL,
    // comments
    MULTI_LINE_COMMENT,
    SINGLE_LINE_COMMENT,
    JAVADOC_COMMENT,
    // operators
    NEGATE,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL,
    BOOLEAN_EQUAL,
    NOT_EQUAL,
    AMPERSAND,
    PIPE
};

class Token {
    public:
        TokenType type;
        std::string value;
        int num_value;
        int line;
        int column;
        
        Token(TokenType type, std::string value, int line, int column) {
            this->type = type;
            this->value = value;
            this->line = line;
            this->column = column;
        }

        Token(TokenType type, int num_value, int line, int column) {
            this->type = type;
            this->num_value = num_value;
            this->line = line;
            this->column = column;
        }

        Token(TokenType type, std::string value) {
            this->type = type;
            this->value = value;
        }

        Token(TokenType type) {
            this->type = type;
        }

        std::string toString() {
            return this->type + "(" + this->value + ")";
        }
};

