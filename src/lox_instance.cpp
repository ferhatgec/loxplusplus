// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include "../include/lox_instance.hpp"
#include "../include/error.hpp"
#include "../include/lox_class.hpp"
#include "../include/lox_function.hpp"

namespace loxplusplus {
LoxInstance::LoxInstance()
    : klass{nullptr} {
}

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass)
    : klass{std::move(klass)} {
}

LoxInstance::~LoxInstance() {
}

[[nodiscard]] Object LoxInstance::get(const Token &name) {
  if (auto it = this->fields.find(name.lexeme); it != this->fields.end()) {
    return it->second;
  }
  std::shared_ptr<LoxFunction> method = this->klass->find_method(name.lexeme);
  if (method != nullptr)
    return method->bind(shared_from_this());
  throw RuntimeError(name, "undefined property '" + name.lexeme + "'.");
}

void LoxInstance::set(const Token &name, Object value) {
  this->fields[name.lexeme] = value;
}

[[nodiscard]] std::string LoxInstance::to_string() {
  return this->klass->name + " instance";
}
}// namespace loxplusplus