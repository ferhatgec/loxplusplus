// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <map>

#include "lox_callable.hpp"

namespace loxplusplus {
class Interpreter;
class LoxFunction;

class LoxClass : public LoxCallable,
								 public std::enable_shared_from_this<LoxClass> {
	friend class LoxInstance;

public:
	LoxClass(std::string name, std::shared_ptr<LoxClass> superclass,
					 std::map<std::string, std::shared_ptr<LoxFunction>> methods);

	[[nodiscard]] std::shared_ptr<LoxFunction> find_method(const std::string &name);
	[[nodiscard]] std::string to_string() override;
	[[nodiscard]] Object call(Interpreter &interpreter, std::vector<Object> arguments) override;
	[[nodiscard]] int arity() override;

private:
	const std::string name;
	const std::shared_ptr<LoxClass> superclass;
	std::map<std::string, std::shared_ptr<LoxFunction>> methods;
};
}// namespace loxplusplus
