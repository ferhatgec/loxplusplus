// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <vector>

#include "token.hpp"

namespace loxplusplus {
class Interpreter;

class LoxCallable {
public:
	[[nodiscard]] virtual int arity() = 0;
	[[nodiscard]] virtual Object call(Interpreter &interpreter,
																		std::vector<Object> arguments) = 0;
	[[nodiscard]] virtual std::string to_string() = 0;
	virtual ~LoxCallable() = default;
};
}// namespace loxplusplus