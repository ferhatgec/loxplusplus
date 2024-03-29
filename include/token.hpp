// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <variant>

#include "token_type.hpp"

#define StringIndex 0
#define LongDoubleIndex 1
#define BoolIndex 2
#define NullptrIndex 3
#define LoxFunctionIndex 4
#define LoxClassIndex 5
#define LoxInstanceIndex 6

namespace loxplusplus {
class LoxFunction;
class LoxClass;
class LoxInstance;

using Object =
	std::variant<std::string, long double, bool, std::nullptr_t,
							 std::shared_ptr<LoxFunction>, std::shared_ptr<LoxClass>,
							 std::shared_ptr<LoxInstance>>;

class Token {
public:
	Token(TokenType type, std::string lexeme, Object literal, int line) noexcept;
	~Token() noexcept;
	[[nodiscard]] std::string to_string() const noexcept;

public:
	const TokenType type;
	const std::string lexeme;
	const Object literal;
	const int line;
};
}// namespace loxplusplus