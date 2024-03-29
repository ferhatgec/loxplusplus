// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <string>

namespace loxplusplus {
enum TokenType {
  // Single-character tokens.
  LEFT_PAREN = '(',
  RIGHT_PAREN = ')',
  LEFT_BRACE = '{',
  RIGHT_BRACE = '}',
  COMMA = ',',
  DOT = '.',
  MINUS = '-',
  PLUS = '+',
  SEMICOLON = ';',
  SLASH = '/',
  STAR = '*',

  // One or two character tokens.
  BANG = -127,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals.
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords.
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  EOF_
};

static std::string to_string(TokenType type) {
  static const std::string strings[] = {
    "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE", "COMMA",
    "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH",
    "STAR", "BANG", "BANG_EQUAL", "EQUAL", "EQUAL_EQUAL",
    "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL", "IDENTIFIER",
    "STRING", "NUMBER", "AND", "CLASS", "ELSE",
    "FALSE", "FUN", "FOR", "IF", "NIL",
    "OR", "PRINT", "RETURN", "SUPER", "THIS",
    "TRUE", "VAR", "WHILE", "EOF"};

  return strings[static_cast<int>(type)];
}
}// namespace loxplusplus