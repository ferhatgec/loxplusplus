// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include "../include/lox_class.hpp"
#include "../include/lox_function.hpp"
#include "../include/lox_instance.hpp"

namespace loxplusplus {
LoxClass::LoxClass(std::string name, std::shared_ptr<LoxClass> superclass,
                   std::map<std::string, std::shared_ptr<LoxFunction>> methods)
    : superclass{superclass}, name{std::move(name)},
      methods{std::move(methods)} {}

[[nodiscard]] std::shared_ptr<LoxFunction> LoxClass::find_method(const std::string &name) {
  auto elem = methods.find(name);
  if (elem != methods.end()) {
    return elem->second;
  }
  if (this->superclass != nullptr) {
    return this->superclass->find_method(name);
  }
  return nullptr;
}

[[nodiscard]] int LoxClass::arity() {
  std::shared_ptr<LoxFunction> initializer = this->find_method("init");
  if (initializer == nullptr)
    return 0;
  return initializer->arity();
}

[[nodiscard]] Object LoxClass::call(Interpreter &interpreter, std::vector<Object> arguments) {
  auto instance = std::make_shared<LoxInstance>(shared_from_this());
  std::shared_ptr<LoxFunction> initializer = find_method("init");
  if (initializer != nullptr) {
    initializer->bind(instance)->call(interpreter, std::move(arguments));
  }
  return instance;
}

[[nodiscard]] std::string LoxClass::to_string() { return name; }
}// namespace loxplusplus