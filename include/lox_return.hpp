// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#pragma once

#include <exception>

#include "token.hpp"

namespace loxplusplus {
class LoxReturnException : std::exception {
public:
  LoxReturnException(Object value) : value{value} {}
  const Object value;
};
}// namespace loxplusplus