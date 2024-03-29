// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <map>

#include "error.hpp"
#include "token.hpp"

namespace loxplusplus {
class Environment : public std::enable_shared_from_this<Environment> {
  friend class Interpreter;

public:
  Environment();
  Environment(std::shared_ptr<Environment> enclosing);

  [[nodiscard]] Object get(const Token &name);
  [[nodiscard]] Object get_at(int distance, const std::string &name);

  void assign(const Token &name, Object value);
  void define(const std::string &name, Object value);
  void assign_at(int distance, const Token &name, Object value);

  [[nodiscard]] std::shared_ptr<Environment> ancestor(int distance);

public:
  std::shared_ptr<Environment> enclosing;
  std::map<std::string, Object> values;
};
}// namespace loxplusplus