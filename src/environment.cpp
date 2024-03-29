// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include "../include/environment.hpp"

namespace loxplusplus {
Environment::Environment()
		: enclosing{nullptr} {
}

Environment::Environment(std::shared_ptr<Environment> enclosing)
		: enclosing{std::move(enclosing)} {
}

[[nodiscard]] Object Environment::get(const Token &name) {
	if (auto it = this->values.find(name.lexeme); it != this->values.end())
		return it->second;
	if (this->enclosing != nullptr)
		return this->enclosing->get(name);
	throw RuntimeError(name, "undefined variable '" + name.lexeme + "'.");
}

[[nodiscard]] Object Environment::get_at(int distance, const std::string &name) {
	return this->ancestor(distance)->values[name];
}

void Environment::assign(const Token &name, Object value) {
	if (auto it = this->values.find(name.lexeme); it != this->values.end()) {
		it->second = value;
		return;
	}
	if (this->enclosing != nullptr) {
		this->enclosing->assign(name, std::move(value));
		return;
	}
	throw RuntimeError(name, "undefined variable '" + name.lexeme + "'.");
}

void Environment::define(const std::string &name, Object value) {
	this->values[name] = value;
}

void Environment::assign_at(int distance, const Token &name, Object value) {
	this->ancestor(distance)->values[name.lexeme] = value;
}

[[nodiscard]] std::shared_ptr<Environment> Environment::ancestor(int distance) {
	std::shared_ptr<Environment> environment = shared_from_this();
	for (int i = 0; i < distance; ++i)
		environment = environment->enclosing;
	return environment;
}
}// namespace loxplusplus