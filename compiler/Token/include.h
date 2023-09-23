#pragma once

#include <string>
#include <map>

class Token
{
public:
  enum class Type
  {
    NOT_FOUND,
    END_OF_FILE,

    /* ------------- KEYWORDS --------------- */
    KEYWORD_RETURN,
    KEYWORD_IF,
    KEYWORD_WHILE,
    KEYWORD_DO,
    KEYWORD_ELSE,
    KEYWORD_FUNCTION,
    KEYWORD_CLASS,
    KEYWORD_CONSTRUCTOR,
    KEYWORD_DESSTRUCTOR,
    KEYWORD_OF,
    KEYWORD_IN,
    KEYWORD_UFLOAT,
    KEYWORD_SFLOAT32,
    KEYWORD_SFLOAT64,
    KEYWORD_UINT8,
    KEYWORD_SINT8,
    KEYWORD_UINT16,
    KEYWORD_SINT1,
    KEYWORD_SINT16,
    KEYWORD_UINT32,
    KEYWORD_SINT32,
    KEYWORD_UINT64,
    KEYWORD_SINT64,
    KEYWORD_EXTERN,

    /* ------------- IDENTIFIER --------------- */
    IDENTIFIER,

    /* ------------- LITERALS --------------- */
    LITERAL_INT,
    LITERAL_FLOAT,
    LITERAL_FALSE,
    LITERAL_TRUE,
    LITERAL_CHAR,
    LITERAL_STRING,

    /* -------------------- SEPERATORS --------------------- */
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    LEFT_SQUARE_BRACKET,
    RIGHT_SQUARE_BRACKET,
    SEMICOLON,
    COLON,
    COMMA,
    WHITE_SPACE,
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT,
    ELLIPSIS,
    PERIOD,
    LINE_BREAK,
    TAB,

    /* -------------------- OPERATORS --------------------- */
    LEFT_ANGULAR_BRACKET_EQUALS,
    RIGHT_ANGULAR_BRACKET_EQUALS,
    EXCLAMATION,
    AMPERSAND,
    VBAR,
    EQUALS,
    PLUS,
    HYPHEN,
    BACKSLASH,
    ASTERISK,
    CIRCUMFLEX,
    PERCENT,
    DOUBLE_AMPERSAND,
    DOUBLE_VBAR,
    DOUBLE_EQUALS,
    DOUBLE_PLUS,
    DOUBLE_HYPHEN,
    DOUBLE_ASTERISK,
    DOUBLE_BACKSLASH,
    EXCLAMATION_EQUALS,
    PLUS_EQUALS,
    HYPHEN_EQUALS,
    ASTERISK_EQUALS,
    BACKSLASH_EQUALS,
    CIRCUMFLEX_EQUALS,
    PERCENT_EQUALS,

    /* ------------- SEPERATORS OR OPERATORS --------------- */
    LEFT_ANGULAR_BRACKET,
    RIGHT_ANGULAR_BRACKET,
  };

private:
  unsigned long long start_line_position;
  unsigned long long end_line_position;
  unsigned long long start_column_position;
  unsigned long long end_column_position;

public:
  std::string value;
  Type type;

  Token(std::string value = "\0",
        Type type = Type::NOT_FOUND,
        unsigned long long start_line_position = 1,
        unsigned long long end_line_position = 1,
        unsigned long long start_column_position = 0,
        unsigned long long end_column_position = 0) : value(value),
                                                      type(type),
                                                      start_line_position(start_line_position),
                                                      end_line_position(end_line_position),
                                                      start_column_position(start_column_position),
                                                      end_column_position(end_column_position){};

  ~Token() = default;

  int get_binary_operator_precedence()
  {
    switch (type)
    {
    case Type::DOUBLE_VBAR:
    case Type::DOUBLE_AMPERSAND:
      return 2;
    case Type::DOUBLE_EQUALS:
    case Type::EXCLAMATION_EQUALS:
    case Type::LEFT_ANGULAR_BRACKET_EQUALS:
    case Type::LEFT_ANGULAR_BRACKET:
    case Type::RIGHT_ANGULAR_BRACKET_EQUALS:
    case Type::RIGHT_ANGULAR_BRACKET:
      return 3;
    case Type::PLUS:
    case Type::HYPHEN:
      return 4;
    case Type::BACKSLASH:
    case Type::ASTERISK:
    case Type::PERCENT:
      return 5;
    case Type::CIRCUMFLEX:
    case Type::DOUBLE_ASTERISK:
    case Type::DOUBLE_BACKSLASH:
      return 6;
    default:
      return -1;
    }
  }

  int get_operator_associative()
  {
    switch (type)
    {
    case Type::PLUS:
    case Type::HYPHEN:
    case Type::BACKSLASH:
    case Type::ASTERISK:
    case Type::PERCENT:
    case Type::DOUBLE_BACKSLASH:
    case Type::DOUBLE_VBAR:
    case Type::DOUBLE_AMPERSAND:
    case Type::DOUBLE_EQUALS:
    case Type::EXCLAMATION_EQUALS:
    case Type::LEFT_ANGULAR_BRACKET_EQUALS:
    case Type::LEFT_ANGULAR_BRACKET:
    case Type::RIGHT_ANGULAR_BRACKET_EQUALS:
    case Type::RIGHT_ANGULAR_BRACKET:
      return 1;
    case Type::CIRCUMFLEX:
    case Type::DOUBLE_ASTERISK:
      return 2;
    default:
      return -1;
    }
  }

  bool is_unary_operator()
  {
    switch (type)
    {
    case Type::DOUBLE_EQUALS:
    case Type::EXCLAMATION_EQUALS:
    case Type::LEFT_ANGULAR_BRACKET_EQUALS:
    case Type::LEFT_ANGULAR_BRACKET:
    case Type::RIGHT_ANGULAR_BRACKET_EQUALS:
    case Type::RIGHT_ANGULAR_BRACKET:
      return true;
    default:
      return false;
    }
  }

  bool is_binary_operator_once()
  {
    switch (type)
    {
    case Type::DOUBLE_EQUALS:
    case Type::EXCLAMATION_EQUALS:
    case Type::LEFT_ANGULAR_BRACKET_EQUALS:
    case Type::LEFT_ANGULAR_BRACKET:
    case Type::RIGHT_ANGULAR_BRACKET_EQUALS:
    case Type::RIGHT_ANGULAR_BRACKET:
      return true;
    default:
      return false;
    }
  }

  std::string at()
  {
    return std::to_string(start_line_position) + ':' + std::to_string(start_column_position);
  }

  void set_start_position(unsigned long long line_position, unsigned long long column_position)
  {
    start_line_position = line_position;
    start_column_position = column_position;
  }

  void set_end_position(unsigned long long line_position, unsigned long long column_position)
  {
    end_line_position = line_position;
    end_column_position = column_position;
  }

  inline bool operator==(Token &token)
  {
    return value == token.value;
  }

  inline bool operator==(std::string &str)
  {
    return value == str;
  }
};

std::map<std::string, Token::Type> keywords = {
    {"return", Token::Type::KEYWORD_RETURN},
    {"if", Token::Type::KEYWORD_IF},
    {"while", Token::Type::KEYWORD_WHILE},
    {"do", Token::Type::KEYWORD_DO},
    {"else", Token::Type::KEYWORD_ELSE},
    {"of", Token::Type::KEYWORD_OF},
    {"sint1", Token::Type::KEYWORD_SINT1},
    {"sint8", Token::Type::KEYWORD_SINT8},
    {"sint16", Token::Type::KEYWORD_SINT16},
    {"sint32", Token::Type::KEYWORD_SINT32},
    {"sint64", Token::Type::KEYWORD_SINT64},
    {"uint8", Token::Type::KEYWORD_UINT8},
    {"uint16", Token::Type::KEYWORD_UINT16},
    {"uint32", Token::Type::KEYWORD_UINT32},
    {"uint64", Token::Type::KEYWORD_UINT64},
    {"sfloat32", Token::Type::KEYWORD_SFLOAT32},
    {"sfloat64", Token::Type::KEYWORD_SFLOAT64},
    {"ufloat", Token::Type::KEYWORD_UFLOAT},
    {"extern", Token::Type::KEYWORD_EXTERN},
    {"function", Token::Type::KEYWORD_FUNCTION},
    {"class", Token::Type::KEYWORD_CLASS},
    {"constructor", Token::Type::KEYWORD_CONSTRUCTOR},
    {"destructor", Token::Type::KEYWORD_DESSTRUCTOR},
};

std::map<char, Token::Type> one_character_tokens = {
    {'\n', Token::Type::LINE_BREAK},
    {-1, Token::Type::END_OF_FILE},
    {'\t', Token::Type::TAB},
    {' ', Token::Type::WHITE_SPACE},
    {';', Token::Type::SEMICOLON},
    {':', Token::Type::COLON},
    {',', Token::Type::COMMA},
    {'[', Token::Type::LEFT_SQUARE_BRACKET},
    {']', Token::Type::RIGHT_SQUARE_BRACKET},
    {'{', Token::Type::LEFT_CURLY_BRACKET},
    {'}', Token::Type::RIGHT_CURLY_BRACKET},
    {'(', Token::Type::LEFT_PARENTHESIS},
    {')', Token::Type::RIGHT_PARENTHESIS},
};

// struct CharacterToken
// {
//   char match;
//   Type type;
// };

// struct TwoCharacterToken
// {
//   CharacterToken first;
//   CharacterToken deep[];
// };

// TwoCharacterToken two_character_tokens[] = {
//     {'+', PLUS, {{'=', PLUS_EQUALS}, {'+', DOUBLE_PLUS}}},
//     {'-', HYPHEN, {{'=', HYPHEN_EQUALS}, {'-', DOUBLE_HYPHEN}}},
//     {'=', EQUALS, {{'=', DOUBLE_EQUALS}}},
//     {'^', CIRCUMFLEX, {{'=', CIRCUMFLEX_EQUALS}}},
//     {'/', BACKSLASH, {{'=', BACKSLASH_EQUALS}}},
//     {'*', ASTERISK, {{'=', ASTERISK_EQUALS}}},
//     {'%', PERCENT, {{'=', PERCENT_EQUALS}}},
//     {'<', LEFT_ANGULAR_BRACKET, {{'=', LEFT_ANGULAR_BRACKET_EQUALS}}},
//     {'>', RIGHT_ANGULAR_BRACKET, {{'=', RIGHT_ANGULAR_BRACKET_EQUALS}}},
// };