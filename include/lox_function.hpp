// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include "lox_callable.hpp"

namespace loxplusplus {
class Environment;
class Function;
class LoxInstance;

class LoxFunction : public LoxCallable {
public:
	LoxFunction(std::shared_ptr<Function> declaration,
							std::shared_ptr<Environment> closure, bool is_initializer);
	[[nodiscard]] int arity() override;
	[[nodiscard]] Object call(Interpreter &interpreter, std::vector<Object> arguments) override;
	[[nodiscard]] std::string to_string() override;
	[[nodiscard]] std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> instance);

private:
	std::shared_ptr<Function> declaration;
	std::shared_ptr<Environment> closure;
	bool is_initializer;
};
}// namespace loxplusplus