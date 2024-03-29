// MIT License
//
// Copyright (c) 2024 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include <fstream>

#include "../include/error.hpp"
#include "../include/interpreter.hpp"
#include "../include/parser.hpp"
#include "../include/resolver.hpp"
#include "../include/scanner.hpp"

using namespace loxplusplus;

[[nodiscard]] std::string read_file(std::string_view path) noexcept {
  std::ifstream file(path.data());
  if (!file)
    return "print \"failed to open file\";";
  std::string contents;
  for (std::string temp; std::getline(file, temp); contents.append(temp + "\n"))
    ;
  file.close();
  return std::move(contents);
}

Interpreter interpreter;

void run(std::string_view source) noexcept {
  Scanner scanner(std::move(source));
  std::vector<Token> tokens = scanner.scan_tokens();
  Parser parser(tokens);
  auto statements = parser.parse();
  if (had_error || had_runtime_error)
    return;
  Resolver resolver{interpreter};
  resolver.resolve(statements);
  if (had_error || had_runtime_error)
    return;
  interpreter.interpret(statements);
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    std::cout << "Usage: loxpp [script]\n";
  } else if (argc == 2) {
    run(read_file(argv[1]));
  } else {
    // TODO: implement REPL
  }
}