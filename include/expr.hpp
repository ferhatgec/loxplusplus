// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <vector>

#include "token.hpp"

namespace loxplusplus {
class Assign;
class Binary;
class Call;
class Get;
class Grouping;
class Literal;
class Logical;
class Set;
class Super;
class This;
class Unary;
class Variable;

class ExprVisitor {
public:
  [[nodiscard]] virtual Object visit(std::shared_ptr<Assign> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<Binary> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<Call> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<Get> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<Grouping> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<Literal> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<Logical> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<Set> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<Super> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<This> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<Unary> expr) = 0;
  [[nodiscard]] virtual Object visit(std::shared_ptr<Variable> expr) = 0;
  virtual ~ExprVisitor() = default;
};

class Expr {
public:
  virtual ~Expr();
  virtual Object accept(ExprVisitor &visitor) = 0;
};

class Assign : public Expr, public std::enable_shared_from_this<Assign> {
public:
  Assign(Token name, std::shared_ptr<Expr> value);
  ~Assign();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const Token name;
  const std::shared_ptr<Expr> value;
};

class Binary : public Expr, public std::enable_shared_from_this<Binary> {
public:
  Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right);
  ~Binary();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;
};

class Call : public Expr, public std::enable_shared_from_this<Call> {
public:
  Call(std::shared_ptr<Expr> callee, Token paren,
       std::vector<std::shared_ptr<Expr>> arguments);
  ~Call();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const std::shared_ptr<Expr> callee;
  const Token paren;
  const std::vector<std::shared_ptr<Expr>> arguments;
};

class Get : public Expr, public std::enable_shared_from_this<Get> {
public:
  Get(std::shared_ptr<Expr> object, Token name);
  ~Get();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const std::shared_ptr<Expr> object;
  const Token name;
};

class Grouping : public Expr, public std::enable_shared_from_this<Grouping> {
public:
  Grouping(std::shared_ptr<Expr> expression);
  ~Grouping();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const std::shared_ptr<Expr> expression;
};

class Literal : public Expr, public std::enable_shared_from_this<Literal> {
public:
  Literal(Object value);
  ~Literal();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const Object value;
};

class Logical : public Expr, public std::enable_shared_from_this<Logical> {
public:
  Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right);
  ~Logical();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;
};

class Set : public Expr, public std::enable_shared_from_this<Set> {
public:
  Set(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value);
  ~Set();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const std::shared_ptr<Expr> object;
  const Token name;
  const std::shared_ptr<Expr> value;
};

class Super : public Expr, public std::enable_shared_from_this<Super> {
public:
  Super(Token keyword, Token method);
  ~Super();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const Token keyword;
  const Token method;
};

class This : public Expr, public std::enable_shared_from_this<This> {
public:
  This(Token keyword);
  ~This();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const Token keyword;
};

class Unary : public Expr, public std::enable_shared_from_this<Unary> {
public:
  Unary(Token op, std::shared_ptr<Expr> right);
  ~Unary();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const Token op;
  const std::shared_ptr<Expr> right;
};

class Variable : public Expr, public std::enable_shared_from_this<Variable> {
public:
  Variable(Token name);
  ~Variable();

  [[nodiscard]] Object accept(ExprVisitor &visitor) override;

public:
  const Token name;
};
}// namespace loxplusplus
