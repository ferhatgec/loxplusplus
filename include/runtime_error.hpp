// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <stdexcept>

#include "token.hpp"

namespace loxplusplus {
class RuntimeError : public std::runtime_error {
public:
	RuntimeError(const Token &token, std::string_view message)
			: std::runtime_error{message.data()}, token{token} {}

public:
	const Token &token;
};
}// namespace loxplusplus