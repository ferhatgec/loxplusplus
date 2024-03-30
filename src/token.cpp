// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include "../include/token.hpp"

namespace loxplusplus {
Token::Token(TokenType type, std::string lexeme, Object literal, int line) noexcept
    : type{type},
      lexeme{std::move(lexeme)},
      literal{std::move(literal)},
      line{line} {
}

Token::~Token() noexcept {
}

[[nodiscard]] std::string Token::to_string() const noexcept {
  std::string literal_text;
  switch (this->type) {
  case TokenType::IDENTIFIER: {
    literal_text = this->lexeme;
    break;
  }
  case TokenType::STRING: {
    literal_text = std::get<StringIndex>(this->literal);
    break;
  }
  case TokenType::NUMBER: {
    literal_text = std::to_string(std::get<LongDoubleIndex>(this->literal));
    break;
  }
  case TokenType::TRUE: {
    literal_text = "true";
    break;
  }
  case TokenType::FALSE: {
    literal_text = "false";
    break;
  }
  default: {
    literal_text = "nil";
    break;
  }
  }
  return loxplusplus::to_string(this->type) + " " + this->lexeme + " " + literal_text;
}
}// namespace loxplusplus