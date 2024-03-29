// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include "environment.hpp"
#include "error.hpp"
#include "expr.hpp"
#include "lox_callable.hpp"
#include "lox_class.hpp"
#include "lox_function.hpp"
#include "lox_instance.hpp"
#include "lox_return.hpp"
#include "runtime_error.hpp"
#include "stmt.hpp"

namespace loxplusplus {
class Interpreter : public ExprVisitor, public StmtVisitor {
  friend class LoxFunction;
  friend class Resolver;

public:
  Interpreter();

  void interpret(const std::vector<std::shared_ptr<Stmt>> &statements);

private:
  [[nodiscard]] Object evaluate(std::shared_ptr<Expr> expr);
  [[nodiscard]] Object look_up_variable(const Token &name, std::shared_ptr<Expr> expr);

  void execute(std::shared_ptr<Stmt> stmt);
  void resolve(std::shared_ptr<Expr> expr, int depth);
  void execute_block(const std::vector<std::shared_ptr<Stmt>> &statements,
                     std::shared_ptr<Environment> environment);
  void check_number_operand(const Token &op, const Object &operand);
  void check_number_operands(const Token &op,
                             const Object &left,
                             const Object &right);

  [[nodiscard]] bool is_truthy(const Object &object);
  [[nodiscard]] bool is_equal(const Object &a, const Object &b);

  [[nodiscard]] std::string stringify(const Object &object);

  [[nodiscard]] Object visit(std::shared_ptr<Block> stmt) override;
  [[nodiscard]] Object visit(std::shared_ptr<Class> stmt) override;
  [[nodiscard]] Object visit(std::shared_ptr<Expression> stmt) override;
  [[nodiscard]] Object visit(std::shared_ptr<Function> stmt) override;
  [[nodiscard]] Object visit(std::shared_ptr<If> stmt) override;
  [[nodiscard]] Object visit(std::shared_ptr<Print> stmt) override;
  [[nodiscard]] Object visit(std::shared_ptr<Return> stmt) override;
  [[nodiscard]] Object visit(std::shared_ptr<Var> stmt) override;
  [[nodiscard]] Object visit(std::shared_ptr<While> stmt) override;

  [[nodiscard]] Object visit(std::shared_ptr<Assign> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<Binary> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<Call> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<Get> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<Grouping> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<Literal> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<Logical> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<Set> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<Super> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<This> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<Unary> expr) override;
  [[nodiscard]] Object visit(std::shared_ptr<Variable> expr) override;

private:
  std::shared_ptr<Environment> globals;
  std::shared_ptr<Environment> environment;
  std::map<std::shared_ptr<Expr>, int> locals;
};
}// namespace loxplusplus