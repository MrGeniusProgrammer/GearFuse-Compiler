#pragma once

#include <vector>
#include "../Token/include.h"

class Lexer
{
private:
  char current_char;
  char lookahead_char;
  unsigned long long line_position;
  unsigned long long column_position;
  long length;
  FILE *file;

  Token read_token();
  void next(int amount = 1);

public:
  Lexer();
  Lexer(char *filename);
  void init(char *filename);
  Token get_next_token();
  bool has_next();
  std::vector<Token> read_tokens();
  ~Lexer();
};

Lexer::Lexer()
{
  current_char = '\0';
  lookahead_char = '\0';
  file = NULL;
  line_position = 1;
  column_position = 0;
  length = 0;
}

Lexer::Lexer(char *filename)
{
  init(filename);
}

bool Lexer::has_next()
{
  return ftell(file) < length;
}

void Lexer::init(char *filename)
{
  file = fopen(filename, "r");
  if (file == NULL)
  {
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  length = ftell(file);
  fseek(file, 0, SEEK_SET);

  line_position = 1;
  column_position = 0;
  current_char = fgetc(file);
  lookahead_char = fgetc(file);
}

Lexer::~Lexer()
{
  fclose(file);
}

void Lexer::next(int amount)
{
  char ch;
  for (int i = 0; i < amount; i++)
  {
    ch = fgetc(file);
    if (ch == '\n')
    {
      line_position++;
      column_position = 0;
      continue;
    }

    column_position++;
  }

  current_char = lookahead_char;
  lookahead_char = ch;
}

Token Lexer::get_next_token()
{
  Token token = read_token();
  token.set_end_position(line_position, column_position);
  return token;
}

std::vector<Token> Lexer::read_tokens()
{
  std::vector<Token> tokens;

  Token token = get_next_token();
  while (token.type != Token::Type::END_OF_FILE)
  {
    tokens.push_back(token);
    token = get_next_token();
  }

  return tokens;
}

Token Lexer::read_token()
{
  Token token;
  token.set_start_position(line_position, column_position);

  if (std::isdigit(current_char))
  {
    token.value.push_back(current_char);
    next();

    bool found_period = false;
    while (std::isdigit(current_char) || current_char == '.')
    {
      if (current_char == '.')
      {
        if (!found_period)
        {
          found_period = true;
        }
        else
        {
          break;
        }
      }

      token.value.push_back(current_char);
      next();
    }

    if (!found_period)
    {
      token.type = Token::Type::LITERAL_INT;
    }
    else
    {
      token.type = Token::Type::LITERAL_FLOAT;
    }
    return token;
  }

  if (std::isalpha(current_char) || current_char == '_')
  {
    bool is_underscore = current_char == '_';
    token.value.push_back(current_char);
    next();
    while (std::isalpha(current_char) || std::isdigit(current_char) || current_char == '_')
    {
      token.value.push_back(current_char);
      next();
    }

    if (!is_underscore && keywords.count(token.value))
    {
      token.type = keywords[token.value];
      return token;
    }

    token.type = Token::Type::IDENTIFIER;
    return token;
  }

  if (one_character_tokens.count(current_char))
  {
    token.value.push_back(current_char);
    token.type = one_character_tokens[current_char];
    next();
    return token;
  }

  if (current_char == '\\')
  {
    next();
    if (current_char == '\\')
    {
      next();
      while (current_char != '\n')
      {
        token.value.push_back(current_char);
        next();
      }

      token.type = Token::Type::LITERAL_STRING;
      return token;
    }
    else if (current_char == '*')
    {
      next();
      while (current_char != '*' && lookahead_char != '/')
      {
        token.value.push_back(current_char);
        next();
      }

      token.type = Token::Type::MULTI_LINE_COMMENT;
      next(2);
      return token;
    }
  }

  if (current_char == '"')
  {
    next();
    while (current_char != '"')
    {
      token.value.push_back(current_char);
      next();
    }

    token.type = Token::Type::LITERAL_STRING;
    next();
    return token;
  }

  if (current_char == 39)
  {
    next();
    while (current_char != 39)
    {
      token.value.push_back(current_char);
      next();
    }

    token.type = Token::Type::LITERAL_CHAR;
    next();
    return token;
  }

  if (current_char == '!')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '=')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::EXCLAMATION_EQUALS;
      next();
      return token;
    }

    token.type = Token::Type::EXCLAMATION;
    return token;
  }

  if (current_char == '<')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '=')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::LEFT_ANGULAR_BRACKET_EQUALS;
      next();
      return token;
    }

    token.type = Token::Type::LEFT_ANGULAR_BRACKET;
    return token;
  }

  if (current_char == '>')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '=')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::RIGHT_ANGULAR_BRACKET_EQUALS;
      next();
      return token;
    }

    token.type = Token::Type::RIGHT_ANGULAR_BRACKET;
    return token;
  }

  if (current_char == '=')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '=')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::DOUBLE_EQUALS;
      next();
      return token;
    }

    token.type = Token::Type::EQUALS;
    return token;
  }

  if (current_char == '&')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '&')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::DOUBLE_AMPERSAND;
      next();
      return token;
    }

    token.type = Token::Type::AMPERSAND;
    return token;
  }

  if (current_char == '|')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '|')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::DOUBLE_VBAR;
      next();
      return token;
    }

    token.type = Token::Type::VBAR;
    return token;
  }

  if (current_char == '+')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '=')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::PLUS_EQUALS;
      next();
      return token;
    }

    if (current_char == '+')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::DOUBLE_PLUS;
      next();
      return token;
    }

    token.type = Token::Type::PLUS;
    return token;
  }

  if (current_char == '-')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '=')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::HYPHEN_EQUALS;
      next();
      return token;
    }

    if (current_char == '-')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::DOUBLE_HYPHEN;
      next();
      return token;
    }

    token.type = Token::Type::HYPHEN;
    return token;
  }

  if (current_char == '*')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '=')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::ASTERISK_EQUALS;
      next();
      return token;
    }

    if (current_char == '*')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::DOUBLE_ASTERISK;
      next();
      return token;
    }

    token.type = Token::Type::ASTERISK;
    return token;
  }

  if (current_char == '/')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '=')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::BACKSLASH_EQUALS;
      next();
      return token;
    }

    if (current_char == '/')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::DOUBLE_BACKSLASH;
      next();
      return token;
    }

    token.type = Token::Type::BACKSLASH;
    return token;
  }

  if (current_char == '%')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '=')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::PERCENT_EQUALS;
      next();
      return token;
    }

    token.type = Token::Type::PERCENT;
    return token;
  }

  if (current_char == '^')
  {
    token.value.push_back(current_char);
    next();
    if (current_char == '=')
    {
      token.value.push_back(current_char);
      token.type = Token::Type::CIRCUMFLEX_EQUALS;
      next();
      return token;
    }

    token.type = Token::Type::CIRCUMFLEX;
    return token;
  }

  token.value.push_back(current_char);
  token.type = Token::Type::NOT_FOUND;
  return token;
}
