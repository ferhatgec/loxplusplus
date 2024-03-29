// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include "../include/scanner.hpp"

namespace loxplusplus {
Scanner::Scanner(std::string_view source) : source{source} {}

[[nodiscard]] std::vector<Token> Scanner::scan_tokens() {
	while (!this->is_at_end()) {
		this->start = this->current;
		this->scan_token();
	}
	this->tokens.emplace_back(TokenType::EOF_, "", nullptr, this->line);
	return this->tokens;
}

void Scanner::scan_token() {
	const char &c = this->advance();
	switch (c) {
	case '(':
	case ')':
	case '{':
	case '}':
	case ',':
	case '.':
	case '-':
	case '+':
	case ';':
	case '*': {
		this->add_token(static_cast<TokenType>(c));
		break;
	}
	case '!': {
		this->add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
		break;
	}
	case '=': {
		this->add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
		break;
	}
	case '<': {
		this->add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
		break;
	}
	case '>': {
		this->add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
		break;
	}
	case '/': {
		if (this->match('/')) {
			while (this->peek() != '\n' && !is_at_end())
				this->advance();
		} else
			this->add_token(TokenType::SLASH);
		break;
	}
	case ' ':
	case '\r':
	case '\t': {
		break;
	}
	case '\n': {
		++this->line;
		break;
	}
	case '"': {
		this->string();
		break;
	}
	default: {
		if (this->is_digit(c)) {
			this->number();
		} else if (this->is_alpha(c)) {
			this->identifier();
		} else {
			error(line, "unexpected character.");
		}
		break;
	}
	}
}

void Scanner::identifier() {
	while (this->is_alpha_numeric(this->peek()))
		this->advance();
	std::string text = std::string(source.substr(start, current - start));
	TokenType type;
	if (auto match = this->keywords.find(text); match == this->keywords.end()) {
		type = TokenType::IDENTIFIER;
	} else {
		type = match->second;
	}
	this->add_token(type);
}

void Scanner::number() {
	while (this->is_digit(this->peek()))
		this->advance();
	if (this->peek() == '.' && is_digit(peek_next())) {
		this->advance();
		while (this->is_digit(this->peek()))
			this->advance();
	}
	this->add_token(TokenType::NUMBER,
									std::stold(std::string(source.substr(start, current - start))));
}

void Scanner::string() {
	while (this->peek() != '"' && !this->is_at_end()) {
		if (this->peek() == '\n')
			++this->line;
		this->advance();
	}
	if (this->is_at_end()) {
		error(line, "unterminated string.");
		return;
	}
	this->advance();
	this->add_token(TokenType::STRING, std::string(this->source.substr(this->start + 1, this->current - 2 - this->start)));
}

[[nodiscard]] bool Scanner::match(const char &expected) {
	if (this->is_at_end())
		return false;
	if (this->source[this->current] != expected)
		return false;
	++this->current;
	return true;
}

[[nodiscard]] const char &Scanner::peek() {
	if (this->is_at_end())
		return Scanner::null_char;
	return this->source[this->current];
}

[[nodiscard]] const char &Scanner::peek_next() {
	if (this->current + 1 >= this->source.length())
		return Scanner::null_char;
	return this->source[this->current + 1];
}

const char &Scanner::advance() {
	return this->source[this->current++];
}

[[nodiscard]] bool Scanner::is_alpha(const char &c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

[[nodiscard]] bool Scanner::is_alpha_numeric(const char &c) {
	return this->is_alpha(c) || this->is_digit(c);
}

[[nodiscard]] bool Scanner::is_digit(const char &c) {
	return c >= '0' && c <= '9';
}

[[nodiscard]] bool Scanner::is_at_end() {
	return this->current >= this->source.length();
}

void Scanner::add_token(TokenType type) {
	this->add_token(type, nullptr);
}

void Scanner::add_token(TokenType type, Object literal) {
	this->tokens.emplace_back(type,
														std::string(this->source.substr(this->start, this->current - this->start)),
														literal,
														this->line);
}
}// namespace loxplusplus