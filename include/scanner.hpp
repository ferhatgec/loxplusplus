// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <map>
#include <string_view>
#include <vector>

#include "error.hpp"
#include "token.hpp"

namespace loxplusplus {
class Scanner {
public:
	Scanner(std::string_view source);

	[[nodiscard]] std::vector<Token> scan_tokens();

private:
	void scan_token();
	void identifier();
	void number();
	void string();

	[[nodiscard]] const char &peek();
	[[nodiscard]] const char &peek_next();
	const char &advance();

	[[nodiscard]] bool match(const char &expected);
	[[nodiscard]] bool is_alpha(const char &c);
	[[nodiscard]] bool is_alpha_numeric(const char &c);
	[[nodiscard]] bool is_digit(const char &c);
	[[nodiscard]] bool is_at_end();

	void add_token(TokenType type);
	void add_token(TokenType type, Object literal);

private:
	std::string_view source;
	std::vector<Token> tokens;

	int start{0},
		current{0},
		line{1};

	static inline const char null_char = '\0';
	static inline const std::map<std::string, TokenType> keywords{
		{"and", TokenType::AND},
		{"class", TokenType::CLASS},
		{"else", TokenType::ELSE},
		{"false", TokenType::FALSE},
		{"for", TokenType::FOR},
		{"fun", TokenType::FUN},
		{"if", TokenType::IF},
		{"nil", TokenType::NIL},
		{"or", TokenType::OR},
		{"print", TokenType::PRINT},
		{"return", TokenType::RETURN},
		{"super", TokenType::SUPER},
		{"this", TokenType::THIS},
		{"true", TokenType::TRUE},
		{"var", TokenType::VAR},
		{"while", TokenType::WHILE},
	};
};
}// namespace loxplusplus