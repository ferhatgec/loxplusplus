// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include "interpreter.hpp"

namespace loxplusplus {
class Resolver : public ExprVisitor, public StmtVisitor {
	enum class FunctionType { NONE,
														FUNCTION,
														INITIALIZER,
														METHOD };
	enum class ClassType { NONE,
												 CLASS,
												 SUBCLASS };

public:
	Resolver(Interpreter &interpreter);

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

	void resolve(const std::vector<std::shared_ptr<Stmt>> &statements);
	void resolve(std::shared_ptr<Stmt> stmt);
	void resolve(std::shared_ptr<Expr> expr);
	void resolve_function(std::shared_ptr<Function> function, FunctionType type);
	void begin_scope();
	void end_scope();
	void declare(const Token &name);
	void define(const Token &name);
	void resolve_local(std::shared_ptr<Expr> expr, const Token &name);

private:
	ClassType current_class{ClassType::NONE};
	FunctionType current_function{FunctionType::NONE};
	Interpreter &interpreter;
	std::vector<std::map<std::string, bool>> scopes;
};
}// namespace loxplusplus