// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include "../include/resolver.hpp"

namespace loxplusplus {
Resolver::Resolver(Interpreter &interpreter) : interpreter{interpreter} {}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Block> stmt) {
  this->begin_scope();
  this->resolve(stmt->statements);
  this->end_scope();
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Class> stmt) {
  ClassType enclosing_class = this->current_class;
  this->current_class = ClassType::CLASS;
  this->declare(stmt->name);
  this->define(stmt->name);
  if (stmt->superclass != nullptr && stmt->name.lexeme == stmt->superclass->name.lexeme) {
    error(stmt->superclass->name, "a class can't inherit from itself.");
  }
  if (stmt->superclass != nullptr) {
    this->current_class = ClassType::SUBCLASS;
    this->resolve(stmt->superclass);
  }
  if (stmt->superclass != nullptr) {
    this->begin_scope();
    this->scopes.back()["super"] = true;
  }
  this->begin_scope();
  this->scopes.back()["this"] = true;
  for (std::shared_ptr<Function> method : stmt->methods) {
    FunctionType declaration = FunctionType::METHOD;
    if (method->name.lexeme == "init")
      declaration = FunctionType::INITIALIZER;
    this->resolve_function(method, declaration);
  }
  this->end_scope();
  if (stmt->superclass != nullptr)
    this->end_scope();
  this->current_class = enclosing_class;
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Expression> stmt) {
  this->resolve(stmt->expression);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Function> stmt) {
  this->declare(stmt->name);
  this->define(stmt->name);
  this->resolve_function(stmt, FunctionType::FUNCTION);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<If> stmt) {
  this->resolve(stmt->condition);
  this->resolve(stmt->then_branch);
  if (stmt->else_branch != nullptr)
    this->resolve(stmt->else_branch);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Print> stmt) {
  this->resolve(stmt->expression);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Return> stmt) {
  if (current_function == FunctionType::NONE) {
    error(stmt->keyword, "can't return from top-level code.");
  }
  if (stmt->value != nullptr) {
    if (current_function == FunctionType::INITIALIZER) {
      error(stmt->keyword, "can't return a value from an initializer.");
    }
    this->resolve(stmt->value);
  }
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Var> stmt) {
  this->declare(stmt->name);
  if (stmt->initializer != nullptr) {
    this->resolve(stmt->initializer);
  }
  this->define(stmt->name);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<While> stmt) {
  this->resolve(stmt->condition);
  this->resolve(stmt->body);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Assign> expr) {
  this->resolve(expr->value);
  this->resolve_local(expr, expr->name);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Binary> expr) {
  this->resolve(expr->left);
  this->resolve(expr->right);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Call> expr) {
  this->resolve(expr->callee);
  for (const std::shared_ptr<Expr> &argument : expr->arguments)
    this->resolve(argument);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Get> expr) {
  this->resolve(expr->object);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Grouping> expr) {
  this->resolve(expr->expression);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Literal> expr) {
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Logical> expr) {
  this->resolve(expr->left);
  this->resolve(expr->right);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Set> expr) {
  this->resolve(expr->value);
  this->resolve(expr->object);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Super> expr) {
  if (this->current_class == ClassType::NONE) {
    error(expr->keyword, "can't user 'super' outside of a class.");
  } else if (this->current_class != ClassType::SUBCLASS) {
    error(expr->keyword, "can't user 'super' in a class with no superclass.");
  }
  this->resolve_local(expr, expr->keyword);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<This> expr) {
  if (this->current_class == ClassType::NONE) {
    error(expr->keyword, "can't use 'this' outside of a class.");
    return nullptr;
  }
  this->resolve_local(expr, expr->keyword);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Unary> expr) {
  this->resolve(expr->right);
  return nullptr;
}

[[nodiscard]] Object Resolver::visit(std::shared_ptr<Variable> expr) {
  if (!this->scopes.empty()) {
    auto &scope = this->scopes.back();
    if (auto it = scope.find(expr->name.lexeme); it != scope.end() && it->second == false) {
      error(expr->name, "can't read local variable in its own initializer.");
    }
  }
  this->resolve_local(expr, expr->name);
  return nullptr;
}

void Resolver::resolve(const std::vector<std::shared_ptr<Stmt>> &statements) {
  for (const std::shared_ptr<Stmt> &statement : statements)
    this->resolve(statement);
}

void Resolver::resolve(std::shared_ptr<Stmt> stmt) {
  stmt->accept(*this);
}

void Resolver::resolve(std::shared_ptr<Expr> expr) {
  expr->accept(*this);
}

void Resolver::resolve_function(std::shared_ptr<Function> function,
                                FunctionType type) {
  FunctionType enclosingFunction = current_function;
  current_function = type;
  this->begin_scope();
  for (const Token &param : function->params) {
    this->declare(param);
    this->define(param);
  }
  this->resolve(function->body);
  this->end_scope();
  current_function = enclosingFunction;
}

void Resolver::begin_scope() {
  this->scopes.emplace_back(std::map<std::string, bool>());
}

void Resolver::end_scope() {
  this->scopes.pop_back();
}

void Resolver::declare(const Token &name) {
  if (this->scopes.empty())
    return;
  std::map<std::string, bool> &scope = this->scopes.back();
  if (scope.find(name.lexeme) != scope.end()) {
    error(name, "already variable with this name in this scope.");
  }
  scope[name.lexeme] = false;
}

void Resolver::define(const Token &name) {
  if (this->scopes.empty())
    return;
  this->scopes.back()[name.lexeme] = true;
}

void Resolver::resolve_local(std::shared_ptr<Expr> expr, const Token &name) {
  for (int i = scopes.size() - 1; i >= 0; --i) {
    if (this->scopes[i].find(name.lexeme) != scopes[i].end()) {
      this->interpreter.resolve(expr, scopes.size() - 1 - i);
      return;
    }
  }
}
}// namespace loxplusplus