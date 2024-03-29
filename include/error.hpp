// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <iostream>
#include <string_view>

#include "runtime_error.hpp"
#include "token.hpp"

namespace loxplusplus {
static inline bool had_error{false};
static inline bool had_runtime_error{false};

static void report(int line, std::string_view where, std::string_view message) {
  std::cerr << "[line " << line << "]: " << where << ": " << message << '\n';
  had_error = true;
}

static void error(const Token &token, std::string_view message) {
  if (token.type == TokenType::EOF_) {
    report(token.line, " at end", message);
    return;
  }
  report(token.line, " at '" + token.lexeme + "'", message);
}

static void error(int line, std::string_view message) {
  report(line, "", message);
}

static void runtime_error(const RuntimeError &error) {
  std::cerr << "[line " << error.token.line << "]: " << error.what() << '\n';
  had_runtime_error = true;
}
}// namespace loxplusplus
