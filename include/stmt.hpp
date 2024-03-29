// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include "expr.hpp"
#include "token.hpp"

namespace loxplusplus {
class Block;
class Class;
class Expression;
class Function;
class If;
class Print;
class Return;
class Var;
class While;

class StmtVisitor {
public:
	[[nodiscard]] virtual Object visit(std::shared_ptr<Block> stmt) = 0;
	[[nodiscard]] virtual Object visit(std::shared_ptr<Class> stmt) = 0;
	[[nodiscard]] virtual Object visit(std::shared_ptr<Expression> stmt) = 0;
	[[nodiscard]] virtual Object visit(std::shared_ptr<Function> stmt) = 0;
	[[nodiscard]] virtual Object visit(std::shared_ptr<If> stmt) = 0;
	[[nodiscard]] virtual Object visit(std::shared_ptr<Print> stmt) = 0;
	[[nodiscard]] virtual Object visit(std::shared_ptr<Return> stmt) = 0;
	[[nodiscard]] virtual Object visit(std::shared_ptr<Var> stmt) = 0;
	[[nodiscard]] virtual Object visit(std::shared_ptr<While> stmt) = 0;
	virtual ~StmtVisitor() = default;
};

class Stmt {
public:
	virtual ~Stmt();
	virtual Object accept(StmtVisitor &visitor) = 0;
};

class Block : public Stmt, public std::enable_shared_from_this<Block> {
public:
	Block(std::vector<std::shared_ptr<Stmt>> statements);
	~Block();

	[[nodiscard]] Object accept(StmtVisitor &visitor) override;

public:
	const std::vector<std::shared_ptr<Stmt>> statements;
};

class Class : public Stmt, public std::enable_shared_from_this<Class> {
public:
	Class(Token name, std::shared_ptr<Variable> superclass,
				std::vector<std::shared_ptr<Function>> methods);
	~Class();

	[[nodiscard]] Object accept(StmtVisitor &visitor) override;

public:
	const Token name;
	const std::shared_ptr<Variable> superclass;
	const std::vector<std::shared_ptr<Function>> methods;
};

class Expression : public Stmt,
									 public std::enable_shared_from_this<Expression> {
public:
	Expression(std::shared_ptr<Expr> expression);
	~Expression();

	[[nodiscard]] Object accept(StmtVisitor &visitor) override;

public:
	const std::shared_ptr<Expr> expression;
};

class Function : public Stmt, public std::enable_shared_from_this<Function> {
public:
	Function(Token name, std::vector<Token> params,
					 std::vector<std::shared_ptr<Stmt>> body);
	~Function();

	[[nodiscard]] Object accept(StmtVisitor &visitor) override;

public:
	const Token name;
	const std::vector<Token> params;
	const std::vector<std::shared_ptr<Stmt>> body;
};

class If : public Stmt, public std::enable_shared_from_this<If> {
public:
	If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> then_branch,
		 std::shared_ptr<Stmt> else_branch);
	~If();

	[[nodiscard]] Object accept(StmtVisitor &visitor) override;

public:
	const std::shared_ptr<Expr> condition;
	const std::shared_ptr<Stmt> then_branch;
	const std::shared_ptr<Stmt> else_branch;
};

class Print : public Stmt, public std::enable_shared_from_this<Print> {
public:
	Print(std::shared_ptr<Expr> expression);
	~Print();

	[[nodiscard]] Object accept(StmtVisitor &visitor) override;

public:
	const std::shared_ptr<Expr> expression;
};

class Return : public Stmt, public std::enable_shared_from_this<Return> {
public:
	Return(Token keyword, std::shared_ptr<Expr> value);
	~Return();
	[[nodiscard]] Object accept(StmtVisitor &visitor) override;

public:
	const Token keyword;
	const std::shared_ptr<Expr> value;
};

class Var : public Stmt, public std::enable_shared_from_this<Var> {
public:
	Var(Token name, std::shared_ptr<Expr> initializer);
	~Var();

	[[nodiscard]] Object accept(StmtVisitor &visitor) override;

public:
	const Token name;
	const std::shared_ptr<Expr> initializer;
};

class While : public Stmt, public std::enable_shared_from_this<While> {
public:
	While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body);
	~While();

	[[nodiscard]] Object accept(StmtVisitor &visitor) override;

public:
	const std::shared_ptr<Expr> condition;
	const std::shared_ptr<Stmt> body;
};
}// namespace loxplusplus