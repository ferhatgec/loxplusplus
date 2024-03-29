// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <map>
#include "token.hpp"

namespace loxplusplus {
class LoxClass;
class Token;

class LoxInstance : public std::enable_shared_from_this<LoxInstance> {
public:
	LoxInstance();
	LoxInstance(std::shared_ptr<LoxClass> klass);
	~LoxInstance();

	[[nodiscard]] Object get(const Token &name);
	void set(const Token &name, Object value);
	[[nodiscard]] std::string to_string();

private:
	std::shared_ptr<LoxClass> klass;
	std::map<std::string, Object> fields;
};
}// namespace loxplusplus