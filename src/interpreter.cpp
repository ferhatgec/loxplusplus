// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include "../include/interpreter.hpp"

namespace loxplusplus {
Interpreter::Interpreter() {
	this->globals = std::make_shared<Environment>();
	this->environment = this->globals;
}

void Interpreter::interpret(
	const std::vector<std::shared_ptr<Stmt>> &statements) {
	try {
		for (const std::shared_ptr<Stmt> &statement : statements) {
			this->execute(statement);
		}
	} catch (const RuntimeError &error) {
		runtime_error(error);
	}
}

[[nodiscard]] Object Interpreter::evaluate(std::shared_ptr<Expr> expr) {
	return expr->accept(*this);
}

[[nodiscard]] Object Interpreter::look_up_variable(const Token &name, std::shared_ptr<Expr> expr) {
	if (auto it = this->locals.find(expr); it != this->locals.end())
		return this->environment->get_at(it->second, name.lexeme);
	return this->globals->get(name);
}

void Interpreter::execute(std::shared_ptr<Stmt> stmt) {
	stmt->accept(*this);
}

void Interpreter::resolve(std::shared_ptr<Expr> expr, int depth) {
	this->locals[expr] = depth;
}

void Interpreter::execute_block(const std::vector<std::shared_ptr<Stmt>> &statements,
																std::shared_ptr<Environment> environment) {
	std::shared_ptr<Environment> previous = this->environment;
	try {
		this->environment = environment;
		for (const std::shared_ptr<Stmt> &statement : statements)
			this->execute(statement);
	} catch (...) {
		this->environment = previous;
		throw;
	}
	this->environment = previous;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Block> stmt) {
	this->execute_block(stmt->statements, std::make_shared<Environment>(this->environment));
	return nullptr;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Class> stmt) {
	Object superclass;
	if (stmt->superclass != nullptr) {
		superclass = this->evaluate(stmt->superclass);
		if (superclass.index() != LoxClassIndex)
			throw RuntimeError(stmt->superclass->name, "superclass must be a class.");
	}
	this->environment->define(stmt->name.lexeme, nullptr);
	if (stmt->superclass != nullptr) {
		this->environment = std::make_shared<Environment>(this->environment);
		this->environment->define("super", superclass);
	}
	std::map<std::string, std::shared_ptr<LoxFunction>> methods;
	for (std::shared_ptr<Function> method : stmt->methods) {
		auto function = std::make_shared<LoxFunction>(method, this->environment, method->name.lexeme == "init");
		methods[method->name.lexeme] = std::move(function);
	}
	std::shared_ptr<LoxClass> superklass = nullptr;
	if (superclass.index() == LoxClassIndex)
		superklass = std::get<LoxClassIndex>(superclass);
	auto klass = std::make_shared<LoxClass>(stmt->name.lexeme, superklass, methods);
	if (superklass != nullptr)
		this->environment = this->environment->enclosing;
	this->environment->assign(stmt->name, std::move(klass));
	return nullptr;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Expression> stmt) {
	(void)this->evaluate(stmt->expression);
	return nullptr;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Function> stmt) {
	auto function = std::make_shared<LoxFunction>(stmt, environment, false);
	this->environment->define(stmt->name.lexeme, std::move(function));
	return nullptr;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<If> stmt) {
	if (this->is_truthy(evaluate(stmt->condition)))
		this->execute(stmt->then_branch);
	else if (stmt->else_branch != nullptr)
		this->execute(stmt->else_branch);
	return nullptr;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Print> stmt) {
	Object value = this->evaluate(stmt->expression);
	std::cout << this->stringify(value) << "\n";
	return nullptr;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Return> stmt) {
	Object value = nullptr;
	if (stmt->value != nullptr)
		value = this->evaluate(stmt->value);
	throw LoxReturnException(value);
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Var> stmt) {
	Object value = nullptr;
	if (stmt->initializer != nullptr)
		value = this->evaluate(stmt->initializer);
	this->environment->define(stmt->name.lexeme, value);
	return nullptr;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<While> stmt) {
	while (this->is_truthy(this->evaluate(stmt->condition)))
		this->execute(stmt->body);
	return nullptr;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Assign> expr) {
	Object value = this->evaluate(expr->value);
	if (auto it = this->locals.find(expr); it != this->locals.end())
		this->environment->assign_at(it->second, expr->name, value);
	else
		this->globals->assign(expr->name, value);
	return value;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Binary> expr) {
	Object left = this->evaluate(expr->left);
	Object right = this->evaluate(expr->right);
	switch (expr->op.type) {
	case TokenType::BANG_EQUAL: {
		return !this->is_equal(left, right);
	}
	case TokenType::EQUAL_EQUAL: {
		return this->is_equal(left, right);
	}
	case TokenType::GREATER: {
		this->check_number_operands(expr->op, left, right);
		return std::get<LongDoubleIndex>(left) > std::get<LongDoubleIndex>(right);
	}
	case TokenType::GREATER_EQUAL: {
		this->check_number_operands(expr->op, left, right);
		return std::get<LongDoubleIndex>(left) >= std::get<LongDoubleIndex>(right);
	}
	case TokenType::LESS: {
		this->check_number_operands(expr->op, left, right);
		return std::get<LongDoubleIndex>(left) < std::get<LongDoubleIndex>(right);
	}
	case TokenType::LESS_EQUAL: {
		this->check_number_operands(expr->op, left, right);
		return std::get<LongDoubleIndex>(left) <= std::get<LongDoubleIndex>(right);
	}
	case TokenType::MINUS: {
		this->check_number_operands(expr->op, left, right);
		return std::get<LongDoubleIndex>(left) - std::get<LongDoubleIndex>(right);
	}
	case TokenType::PLUS: {
		if (left.index() == LongDoubleIndex && right.index() == LongDoubleIndex) {
			return std::get<LongDoubleIndex>(left) + std::get<LongDoubleIndex>(right);
		}
		if (left.index() == StringIndex && right.index() == StringIndex) {
			return std::get<StringIndex>(left) + std::get<StringIndex>(right);
		}
		throw RuntimeError{expr->op, "operands must be two numbers or two strings."};
	}
	case TokenType::SLASH: {
		this->check_number_operands(expr->op, left, right);
		return std::get<LongDoubleIndex>(left) / std::get<LongDoubleIndex>(right);
	}
	case TokenType::STAR: {
		this->check_number_operands(expr->op, left, right);
		return std::get<LongDoubleIndex>(left) * std::get<LongDoubleIndex>(right);
	}
	}
	return nullptr;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Call> expr) {
	Object callee = this->evaluate(expr->callee);
	std::vector<Object> arguments;
	for (const std::shared_ptr<Expr> &argument : expr->arguments)
		arguments.push_back(this->evaluate(argument));
	std::shared_ptr<LoxCallable> function;
	if (callee.index() == LoxFunctionIndex) {
		function = std::get<LoxFunctionIndex>(callee);
	} else if (callee.index() == LoxClassIndex) {
		function = std::get<LoxClassIndex>(callee);
	} else {
		throw RuntimeError{expr->paren, "can only call functions and classes."};
	}
	if (arguments.size() != function->arity()) {
		throw RuntimeError{expr->paren, "expected " + std::to_string(function->arity()) + " arguments but got " + std::to_string(arguments.size()) + "."};
	}
	return function->call(*this, std::move(arguments));
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Get> expr) {
	Object object = this->evaluate(expr->object);
	if (object.index() == LoxInstanceIndex) {
		return std::get<LoxInstanceIndex>(object)->get(expr->name);
	}
	throw RuntimeError(expr->name, "only instances have properties.");
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Grouping> expr) {
	return this->evaluate(expr->expression);
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Literal> expr) {
	return expr->value;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Logical> expr) {
	if (Object left = this->evaluate(expr->left); expr->op.type == TokenType::OR) {
		if (this->is_truthy(left))
			return left;
	} else if (!this->is_truthy(left))
		return left;
	return this->evaluate(expr->right);
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Set> expr) {
	Object object = this->evaluate(expr->object);
	if (object.index() != LoxInstanceIndex)
		throw RuntimeError(expr->name, "only instances have fields.");
	Object value = this->evaluate(expr->value);
	std::get<std::shared_ptr<LoxInstance>>(object)->set(expr->name, value);
	return value;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Super> expr) {
	const int &distance = this->locals[expr];
	auto superclass = std::get<LoxClassIndex>(this->environment->get_at(distance, "super"));
	auto object = std::get<LoxInstanceIndex>(this->environment->get_at(distance - 1, "this"));
	std::shared_ptr<LoxFunction> method = superclass->find_method(expr->method.lexeme);
	if (method == nullptr)
		throw RuntimeError(expr->method, "undefined property '" + expr->method.lexeme + "'.");
	return method->bind(object);
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<This> expr) {
	return this->look_up_variable(expr->keyword, expr);
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Unary> expr) {
	Object right = this->evaluate(expr->right);
	switch (expr->op.type) {
	case TokenType::BANG: {
		return !this->is_truthy(right);
	}
	case MINUS: {
		this->check_number_operand(expr->op, right);
		return -std::get<LongDoubleIndex>(right);
	}
	}
	return nullptr;
}

[[nodiscard]] Object Interpreter::visit(std::shared_ptr<Variable> expr) {
	return this->look_up_variable(expr->name, expr);
}

void Interpreter::check_number_operand(const Token &op, const Object &operand) {
	if (operand.index() == LongDoubleIndex)
		return;
	throw RuntimeError(op, "operand must be a number.");
}

void Interpreter::check_number_operands(const Token &op, const Object &left,
																				const Object &right) {
	if (left.index() == LongDoubleIndex && right.index() == LongDoubleIndex)
		return;
	throw RuntimeError(op, "operands must be numbers.");
}

[[nodiscard]] bool Interpreter::is_truthy(const Object &object) {
	switch (object.index()) {
	case NullptrIndex: {
		return false;
	}
	case BoolIndex: {
		return std::get<BoolIndex>(object);
	}
	}
	return true;
}

[[nodiscard]] bool Interpreter::is_equal(const Object &a, const Object &b) {
	if (a.index() == NullptrIndex && b.index() == NullptrIndex)
		return true;
	if (a.index() == NullptrIndex)
		return false;
	if (a.index() == StringIndex && b.index() == StringIndex)
		return std::get<StringIndex>(a) == std::get<StringIndex>(b);
	if (a.index() == LongDoubleIndex && b.index() == LongDoubleIndex)
		return std::get<LongDoubleIndex>(a) == std::get<LongDoubleIndex>(b);
	if (a.index() == BoolIndex && b.index() == BoolIndex)
		return std::get<BoolIndex>(a) == std::get<BoolIndex>(b);
	return false;
}

std::string Interpreter::stringify(const Object &object) {
	if (object.index() == NullptrIndex)
		return "nil";
	if (object.index() == LongDoubleIndex) {
		std::string text = std::to_string(std::get<LongDoubleIndex>(object));
		if (text.ends_with(".0"))
			text = text.substr(0, text.length() - 2);
		return text;
	}
	switch (object.index()) {
	case StringIndex: {
		return std::get<StringIndex>(object);
	}
	case BoolIndex: {
		return std::get<BoolIndex>(object) ? "true" : "false";
	}
	case LoxFunctionIndex: {
		return std::get<LoxFunctionIndex>(object)->to_string();
	}
	case LoxClassIndex: {
		return std::get<LoxClassIndex>(object)->to_string();
	}
	case LoxInstanceIndex: {
		return std::get<LoxInstanceIndex>(object)->to_string();
	}
	}
	return "nil";
}
}// namespace loxplusplus