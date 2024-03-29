// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include "../include/parser.hpp"

namespace loxplusplus {
Parser::Parser(const std::vector<Token> &tokens)
    : tokens{tokens} {
}

[[nodiscard]] std::vector<std::shared_ptr<Stmt>> Parser::parse() {
  std::vector<std::shared_ptr<Stmt>> statements;
  while (!this->is_at_end()) {
    statements.push_back(std::move(this->declaration()));
  }
  return statements;
}

[[nodiscard]] std::shared_ptr<Stmt> Parser::declaration() {
  try {
    if (this->match({TokenType::CLASS}))
      return std::move(this->class_declaration());
    if (this->match({TokenType::FUN}))
      return std::move(this->function("function"));
    if (this->match({TokenType::VAR}))
      return std::move(this->var_declaration());
    return std::move(this->statement());
  } catch (const ParseError &error) {
    this->synchronize();
    return nullptr;
  }
}

[[nodiscard]] std::shared_ptr<Stmt> Parser::class_declaration() {
  Token name = this->consume(TokenType::IDENTIFIER, "expect class name.");
  std::shared_ptr<Variable> superclass = nullptr;
  if (this->match({TokenType::LESS})) {
    this->consume(TokenType::IDENTIFIER, "expect superclass name.");
    superclass = std::make_shared<Variable>(this->previous());
  }
  this->consume(TokenType::LEFT_BRACE, "expect '{' before class body.");
  std::vector<std::shared_ptr<Function>> methods;
  while (!this->check(TokenType::RIGHT_BRACE) && !this->is_at_end()) {
    methods.push_back(this->function("method"));
  }
  this->consume(TokenType::RIGHT_BRACE, "expect '}' after class body");
  return std::make_shared<Class>(std::move(name), std::move(superclass), std::move(methods));
}

[[nodiscard]] std::shared_ptr<Stmt> Parser::statement() {
  if (this->match({TokenType::FOR}))
    return std::move(this->for_statement());
  if (this->match({TokenType::IF}))
    return std::move(this->if_statement());
  if (this->match({TokenType::PRINT}))
    return std::move(this->print_statement());
  if (this->match({TokenType::RETURN}))
    return std::move(this->return_statement());
  if (this->match({TokenType::WHILE}))
    return std::move(this->while_statement());
  if (this->match({TokenType::LEFT_BRACE}))
    return std::make_shared<Block>(std::move(this->block()));
  return std::move(this->expression_statement());
}

[[nodiscard]] std::shared_ptr<Stmt> Parser::for_statement() {
  this->consume(TokenType::LEFT_PAREN, "expect '(' after 'for'.");
  std::shared_ptr<Stmt> initializer;
  if (this->match({TokenType::SEMICOLON})) {
    initializer = nullptr;
  } else if (this->match({TokenType::VAR})) {
    initializer = std::move(this->var_declaration());
  } else {
    initializer = std::move(this->expression_statement());
  }
  std::shared_ptr<Expr> condition = nullptr;
  if (!this->check(TokenType::SEMICOLON)) {
    condition = std::move(this->expression());
  }
  this->consume(TokenType::SEMICOLON, "expect ';' after loop condition.");
  std::shared_ptr<Expr> increment = nullptr;
  if (!this->check(TokenType::RIGHT_PAREN)) {
    increment = std::move(this->expression());
  }
  this->consume(TokenType::RIGHT_PAREN, "expect ')' after for clauses.");
  std::shared_ptr<Stmt> body = std::move(this->statement());
  if (increment != nullptr) {
    body = std::make_shared<Block>(std::vector<std::shared_ptr<Stmt>>{
      body, std::make_shared<Expression>(std::move(increment))});
  }
  if (condition == nullptr) {
    condition = std::make_shared<Literal>(true);
  }
  body = std::make_shared<While>(condition, body);
  if (initializer != nullptr) {
    body = std::make_shared<Block>(std::vector<std::shared_ptr<Stmt>>{
      std::move(initializer), body});
  }
  return std::move(body);
}

[[nodiscard]] std::shared_ptr<Stmt> Parser::if_statement() {
  this->consume(TokenType::LEFT_PAREN, "expect '(' after 'if'.");
  std::shared_ptr<Expr> condition = std::move(this->expression());
  this->consume(TokenType::RIGHT_PAREN, "expect ')' after if condition.");
  std::shared_ptr<Stmt> then_branch = std::move(this->statement());
  std::shared_ptr<Stmt> else_branch = nullptr;
  if (this->match({TokenType::ELSE})) {
    else_branch = std::move(this->statement());
  }
  return std::make_shared<If>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

[[nodiscard]] std::shared_ptr<Stmt> Parser::print_statement() {
  std::shared_ptr<Expr> value = std::move(this->expression());
  this->consume(TokenType::SEMICOLON, "expect ';' after value.");
  return std::make_shared<Print>(std::move(value));
}

[[nodiscard]] std::shared_ptr<Stmt> Parser::return_statement() {
  Token keyword = this->previous();
  std::shared_ptr<Expr> value = nullptr;
  if (!this->check(TokenType::SEMICOLON))
    value = std::move(this->expression());
  this->consume(TokenType::SEMICOLON, "expect ';' after return value.");
  return std::make_shared<Return>(keyword, std::move(value));
}

[[nodiscard]] std::shared_ptr<Stmt> Parser::var_declaration() {
  Token name = this->consume(TokenType::IDENTIFIER, "expect variable name.");
  std::shared_ptr<Expr> initializer = nullptr;
  if (this->match({TokenType::EQUAL})) {
    initializer = std::move(this->expression());
  }
  this->consume(TokenType::SEMICOLON, "expect ';' after variable declaration.");
  return std::make_shared<Var>(std::move(name), std::move(initializer));
}

[[nodiscard]] std::shared_ptr<Stmt> Parser::while_statement() {
  this->consume(TokenType::LEFT_PAREN, "expect '(' after 'while'.");
  std::shared_ptr<Expr> condition = std::move(this->expression());
  this->consume(TokenType::RIGHT_PAREN, "expect ')' after condition.");
  std::shared_ptr<Stmt> body = std::move(this->statement());
  return std::make_shared<While>(std::move(condition), std::move(body));
}

[[nodiscard]] std::shared_ptr<Stmt> Parser::expression_statement() {
  std::shared_ptr<Expr> expr = std::move(this->expression());
  this->consume(TokenType::SEMICOLON, "expect ';' after expression.");
  return std::make_shared<Expression>(std::move(expr));
}

[[nodiscard]] std::shared_ptr<Function> Parser::function(std::string kind) {
  Token name = this->consume(TokenType::IDENTIFIER, "expect " + kind + " name.");
  this->consume(TokenType::LEFT_PAREN, "expect '(' after " + kind + " name.");
  std::vector<Token> parameters;
  if (!this->check(TokenType::RIGHT_PAREN)) {
    do {
      if (parameters.size() >= 255) {
        error(this->peek(), "can't have more than 255 parameters.");
      }
      parameters.push_back(this->consume(TokenType::IDENTIFIER, "expect parameter name."));
    } while (this->match({TokenType::COMMA}));
  }
  this->consume(TokenType::RIGHT_PAREN, "expect ')' after parameters.");
  this->consume(TokenType::LEFT_BRACE, "expect '{' before " + kind + " body.");
  std::vector<std::shared_ptr<Stmt>> body = this->block();
  return std::make_shared<Function>(std::move(name), std::move(parameters), std::move(body));
}

[[nodiscard]] std::vector<std::shared_ptr<Stmt>> Parser::block() {
  std::vector<std::shared_ptr<Stmt>> statements;
  while (!this->check(TokenType::RIGHT_BRACE) && !this->is_at_end())
    statements.push_back(std::move(this->declaration()));
  this->consume(TokenType::RIGHT_BRACE, "expect '}' after block.");
  return statements;
}

[[nodiscard]] std::shared_ptr<Expr> Parser::expression() {
  return std::move(this->assignment());
}

[[nodiscard]] std::shared_ptr<Expr> Parser::assignment() {
  std::shared_ptr<Expr> expr = std::move(this->or_());
  if (this->match({TokenType::EQUAL})) {
    Token equals = this->previous();
    std::shared_ptr<Expr> value = std::move(this->assignment());
    if (const std::shared_ptr<Variable> &variable = std::dynamic_pointer_cast<Variable>(expr);
        expr != nullptr)
      return std::make_shared<Assign>(variable->name, std::move(value));
    else if (const std::shared_ptr<Get> get = std::dynamic_pointer_cast<Get>(expr);
             get != nullptr)
      return std::make_shared<Set>(get->object, get->name, std::move(value));
    error(std::move(equals), "invalid assignment target.");
  }
  return std::move(expr);
}

[[nodiscard]] std::shared_ptr<Expr> Parser::or_() {
  std::shared_ptr<Expr> expr = std::move(this->and_());
  while (this->match({TokenType::OR})) {
    Token op = this->previous();
    expr = std::make_shared<Logical>(std::move(expr), std::move(op), std::move(this->and_()));
  }
  return std::move(expr);
}

[[nodiscard]] std::shared_ptr<Expr> Parser::and_() {
  std::shared_ptr<Expr> expr = std::move(this->equality());
  while (this->match({TokenType::AND})) {
    Token op = this->previous();
    expr = std::make_shared<Logical>(expr, std::move(op), std::move(this->equality()));
  }
  return std::move(expr);
}

[[nodiscard]] std::shared_ptr<Expr> Parser::equality() {
  std::shared_ptr<Expr> expr = std::move(this->comparison());
  while (this->match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
    Token op = this->previous();
    expr = std::make_shared<Binary>(expr, std::move(op), std::move(this->comparison()));
  }
  return std::move(expr);
}

[[nodiscard]] std::shared_ptr<Expr> Parser::comparison() {
  std::shared_ptr<Expr> expr = std::move(this->term());
  while (this->match({TokenType::GREATER, TokenType::GREATER_EQUAL,
                      TokenType::LESS, TokenType::LESS_EQUAL})) {
    Token op = this->previous();
    expr = std::make_shared<Binary>(expr, std::move(op), std::move(this->term()));
  }
  return std::move(expr);
}

[[nodiscard]] std::shared_ptr<Expr> Parser::term() {
  std::shared_ptr<Expr> expr = std::move(this->factor());
  while (this->match({TokenType::MINUS, TokenType::PLUS})) {
    Token op = this->previous();
    expr = std::make_shared<Binary>(expr, std::move(op), std::move(this->factor()));
  }
  return std::move(expr);
}

[[nodiscard]] std::shared_ptr<Expr> Parser::factor() {
  std::shared_ptr<Expr> expr = std::move(this->unary());
  while (this->match({TokenType::SLASH, TokenType::STAR})) {
    Token op = this->previous();
    expr = std::make_shared<Binary>(expr, std::move(op), std::move(this->unary()));
  }
  return std::move(expr);
}

[[nodiscard]] std::shared_ptr<Expr> Parser::unary() {
  if (this->match({TokenType::BANG, TokenType::MINUS})) {
    Token op = this->previous();
    return std::make_shared<Unary>(std::move(op), std::move(this->unary()));
  }
  return std::move(this->call());
}

[[nodiscard]] std::shared_ptr<Expr> Parser::finish_call(std::shared_ptr<Expr> callee) {
  std::vector<std::shared_ptr<Expr>> arguments;
  if (!this->check(TokenType::RIGHT_PAREN)) {
    do {
      if (arguments.size() >= 255) {
        error(this->peek(), "Can't have more than 255 arguments.");
      }
      arguments.push_back(this->expression());
    } while (this->match({TokenType::COMMA}));
  }
  Token paren = this->consume(TokenType::RIGHT_PAREN, "expect ')' after arguments.");
  return std::make_shared<Call>(callee, std::move(paren), std::move(arguments));
}

[[nodiscard]] std::shared_ptr<Expr> Parser::call() {
  std::shared_ptr<Expr> expr = this->primary();
  while (true) {
    if (this->match({TokenType::LEFT_PAREN})) {
      expr = this->finish_call(expr);
    } else if (this->match({TokenType::DOT})) {
      Token name = this->consume(TokenType::IDENTIFIER, "expect property name after '.'.");
      expr = std::make_shared<Get>(expr, std::move(name));
    } else {
      break;
    }
  }
  return std::move(expr);
}

[[nodiscard]] std::shared_ptr<Expr> Parser::primary() {
  if (this->match({TokenType::FALSE}))
    return std::make_shared<Literal>(false);
  if (this->match({TokenType::TRUE}))
    return std::make_shared<Literal>(true);
  if (this->match({TokenType::NIL}))
    return std::make_shared<Literal>(nullptr);
  if (this->match({TokenType::NUMBER, TokenType::STRING}))
    return std::make_shared<Literal>(this->previous().literal);
  if (this->match({TokenType::SUPER})) {
    Token keyword = this->previous();
    this->consume(TokenType::DOT, "expect '.' after 'super'.");
    Token method = this->consume(TokenType::IDENTIFIER, "expect superclass method name.");
    return std::make_shared<Super>(std::move(keyword), std::move(method));
  }
  if (this->match({TokenType::THIS}))
    return std::make_shared<This>(std::move(this->previous()));
  if (this->match({TokenType::IDENTIFIER}))
    return std::make_shared<Variable>(std::move(this->previous()));
  if (this->match({TokenType::LEFT_PAREN})) {
    std::shared_ptr<Expr> expr = std::move(this->expression());
    this->consume(TokenType::RIGHT_PAREN, "expect ')' after expression.");
    return std::make_shared<Grouping>(std::move(expr));
  }
  throw parse_error(this->peek(), "expect expression.");
}

[[nodiscard]] bool Parser::match(std::initializer_list<TokenType> &&type) {
  for (auto &typ : std::move(type)) {
    if (this->check(typ)) {
      this->advance();
      return true;
    }
  }
  return false;
}

[[nodiscard]] bool Parser::check(TokenType type) {
  if (this->is_at_end())
    return false;
  return this->peek().type == type;
}

[[nodiscard]] bool Parser::is_at_end() {
  return this->peek().type == EOF_;
}

Token Parser::consume(TokenType type, std::string_view message) {
  if (this->check(type))
    return std::move(this->advance());
  throw parse_error(this->peek(), std::move(message));
}

Token Parser::advance() {
  if (!this->is_at_end())
    ++this->current;
  return this->previous();
}

[[nodiscard]] Token Parser::peek() {
  return this->tokens.at(this->current);
}

[[nodiscard]] Token Parser::previous() {
  return this->tokens.at(this->current - 1);
}

Parser::ParseError Parser::parse_error(const Token &token, std::string_view message) {
  error(token, message);
  return ParseError("");
}

void Parser::synchronize() {
  this->advance();
  while (!this->is_at_end()) {
    if (this->previous().type == TokenType::SEMICOLON)
      return;
    switch (this->peek().type) {
    case TokenType::CLASS:
    case TokenType::FUN:
    case TokenType::VAR:
    case TokenType::FOR:
    case TokenType::IF:
    case TokenType::WHILE:
    case TokenType::PRINT:
    case TokenType::RETURN:
      return;
    }
    this->advance();
  }
}
}// namespace loxplusplus