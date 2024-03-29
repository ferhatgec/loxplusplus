// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include "../include/lox_function.hpp"
#include "../include/environment.hpp"
#include "../include/interpreter.hpp"
#include "../include/lox_instance.hpp"
#include "../include/stmt.hpp"

namespace loxplusplus {
LoxFunction::LoxFunction(std::shared_ptr<Function> declaration,
												 std::shared_ptr<Environment> closure,
												 bool is_initializer)
		: is_initializer{is_initializer},
			closure{std::move(closure)},
			declaration{std::move(declaration)} {}

[[nodiscard]] int LoxFunction::arity() {
	return declaration->params.size();
}

[[nodiscard]] Object LoxFunction::call(Interpreter &interpreter,
																			 std::vector<Object> arguments) {
	auto environment = std::make_shared<Environment>(closure);
	for (std::size_t i = 0; i < this->declaration->params.size(); ++i) {
		environment->define(this->declaration->params[i].lexeme, arguments[i]);
	}
	try {
		interpreter.execute_block(declaration->body, environment);
	} catch (const LoxReturnException &return_value) {
		if (this->is_initializer)
			return this->closure->get_at(0, "this");
		return return_value.value;
	}
	if (this->is_initializer)
		return this->closure->get_at(0, "this");
	return nullptr;
}

[[nodiscard]] std::string LoxFunction::to_string() {
	return "<fn " + declaration->name.lexeme + ">";
}

[[nodiscard]] std::shared_ptr<LoxFunction> LoxFunction::bind(std::shared_ptr<LoxInstance> instance) {
	auto environment = std::make_shared<Environment>(this->closure);
	environment->define("this", instance);
	return std::make_shared<LoxFunction>(this->declaration, std::move(environment), this->is_initializer);
}
}// namespace loxplusplus