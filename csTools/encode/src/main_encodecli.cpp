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
#include <charconv>
#include <iostream>
#include <print>

#include <cs/Logging/Logger.h>

#include "Encode/Parser.h"

template<typename T>
inline T stringToUInt(const char *first, const char *last,
                      const int base = 10, const T defValue = 0)
{
  T value = defValue;
  const std::from_chars_result result =
      std::from_chars(first, last, value, base);

  return result.ec == std::errc()
      ? value
      : defValue;
}

template<typename T>
void inputVariables(Encode::VariableStore<T>& store)
{
  using citer_t = typename Encode::VariableStore<T>::const_iterator;

  for(citer_t it = store.cbegin(); it != store.cend(); ++it) {
    std::print("{} (== 0x{:X}) := ", it->first, it->second);

    std::string input;
    std::cin >> input;
    if( input.empty() ) {
      continue;
    }
    cs::lower(input);

    const T value = cs::startsWith(input, "0x", true)
        ? stringToUInt<T>(input.data() + 2, input.data() + input.size(), 16)
        : stringToUInt<T>(input.data(), input.data() + input.size());

    store[it->first] = value;
  }
}

void test_engine(const Encode::EnginePtr<uint32_t>& engine)
{
  using Engine = cs::element_of_ptr_t<decltype(engine)>;
  using Store  = Engine::Store;

  using  size_type = Engine::size_type;
  using value_type = Engine::value_type;

  if( !engine  ||  !engine->isValid() ) {
    return;
  }

  Store store;
  const size_type count = engine->initialize(store);
  std::println("Number of Variables = {}", count);

  inputVariables(store);

  const value_type value = engine->compose(store);
  std::println("{0}: 0x{1:0{2}X}", engine->text(), value, sizeof(value_type)*2);

  std::println();
}

void test_encode32()
{
  using EnginePtr = Encode::EnginePtr<uint32_t>;
  using Engine    = EnginePtr::element_type;

  EnginePtr engine = Engine::make(32, "test_encode32");
  engine->addLiteral(0x91, 0, 7, 24);
  engine->addVariable("a", 12, 15,  0);
  engine->addVariable("a",  8, 11,  8);
  engine->addVariable("a",  4,  7, 16);
  engine->addVariable("a",  0,  3, 20);
  engine->addVariable("b",  6,  9,  4);

  test_engine(engine);
}

void test_parser()
{
  using Parser = Encode::Parser<uint32_t>;

  Parser parser;

  parser.parse("Encode(32,\"test_parser\") = { "
               "0x91[7:0]@24,"
               "a[15:12]@0,"
               "a[11:8]@8,"
               "a[7:4]@16,"
               "a[3:0]@20,"
               "b[9:6]@4"
               "}\n", cs::Logger::make());

  std::for_each(parser.result.cbegin(), parser.result.cend(), test_engine);
}

void display_note()
{
  std::println("Enter a == 0xABCD and b == 0x3C0.");
  std::println("");
}

////// Main //////////////////////////////////////////////////////////////////

int main(int /*argc*/, char ** /*argv*/)
{
  display_note();
  test_encode32();
  test_parser();

  return EXIT_SUCCESS;
}
