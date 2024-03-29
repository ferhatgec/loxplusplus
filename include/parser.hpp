// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include "error.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "token_type.hpp"

namespace loxplusplus {
class Parser {
	using ParseError = std::runtime_error;

public:
	Parser(const std::vector<Token> &tokens);

	[[nodiscard]] std::vector<std::shared_ptr<Stmt>> parse();

private:
	[[nodiscard]] std::shared_ptr<Stmt> declaration();
	[[nodiscard]] std::shared_ptr<Stmt> class_declaration();
	[[nodiscard]] std::shared_ptr<Stmt> statement();
	[[nodiscard]] std::shared_ptr<Stmt> for_statement();
	[[nodiscard]] std::shared_ptr<Stmt> if_statement();
	[[nodiscard]] std::shared_ptr<Stmt> print_statement();
	[[nodiscard]] std::shared_ptr<Stmt> return_statement();
	[[nodiscard]] std::shared_ptr<Stmt> var_declaration();
	[[nodiscard]] std::shared_ptr<Stmt> while_statement();
	[[nodiscard]] std::shared_ptr<Stmt> expression_statement();

	[[nodiscard]] std::shared_ptr<Function> function(std::string kind);

	[[nodiscard]] std::vector<std::shared_ptr<Stmt>> block();

	[[nodiscard]] std::shared_ptr<Expr> expression();
	[[nodiscard]] std::shared_ptr<Expr> assignment();
	[[nodiscard]] std::shared_ptr<Expr> or_();
	[[nodiscard]] std::shared_ptr<Expr> and_();
	[[nodiscard]] std::shared_ptr<Expr> equality();
	[[nodiscard]] std::shared_ptr<Expr> comparison();
	[[nodiscard]] std::shared_ptr<Expr> term();
	[[nodiscard]] std::shared_ptr<Expr> factor();
	[[nodiscard]] std::shared_ptr<Expr> unary();
	[[nodiscard]] std::shared_ptr<Expr> finish_call(std::shared_ptr<Expr> callee);
	[[nodiscard]] std::shared_ptr<Expr> call();
	[[nodiscard]] std::shared_ptr<Expr> primary();

	[[nodiscard]] bool match(std::initializer_list<TokenType> &&type);
	[[nodiscard]] bool check(TokenType type);
	[[nodiscard]] bool is_at_end();

	Token consume(TokenType type, std::string_view message);
	Token advance();
	[[nodiscard]] Token peek();
	[[nodiscard]] Token previous();

	ParseError parse_error(const Token &token, std::string_view message);

	void synchronize();

private:
	const std::vector<Token> &tokens;
	int current{0};
};
}// namespace loxplusplus
