/****************************************************************************
** Copyright (c) 2024, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <print>

#include <cs/Logging/Logger.h>
#include <cs/Text/TextIO.h>

#include <Calculate/Parser.h>

template<typename T>
void print(const std::unordered_map<std::string,T>& variables)
{
  using value_type = typename std::unordered_map<std::string,T>::value_type;

  auto print = [](const value_type& var) -> void {
    std::println("{} = {} (0x{:X})", var.first, var.second, var.second);
  };

  std::for_each(variables.begin(), variables.end(), print);
}

int main(int /*argc*/, char ** /*argv*/)
{
  using Parser = Calculate::Parser<uint16_t>;

  Parser parser;

  std::print("input> ");

  const std::string input = cs::readStream(std::cin);

  parser.parse(Parser::fixInput(input), cs::Logger::make());

  print<Parser::value_type>(parser.variables);

  return EXIT_SUCCESS;
}
