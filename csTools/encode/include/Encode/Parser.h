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

#pragma once

#include <cs/Lexer/Context.h>
#include <cs/Lexer/AbstractParser.h>

#include <Encode/Engine.h>

namespace Encode {

  enum EncodeTokens : cs::tokenid_t {
    TOK_Identifier = cs::Token::make_userid(1),
    TOK_Integral,
    TOK_String
  };

  class EncodeTokenNames : public cs::TokenNamesBase {
  private:
    struct ctor_tag {
      ctor_tag() noexcept = default;
    };

  public:
    EncodeTokenNames(const ctor_tag& = ctor_tag()) noexcept;

    ~EncodeTokenNames() noexcept;

    const char *name(const cs::tokenid_t id) const;

    static cs::TokenNamesPtr make();
  };

  template<typename T>
  class Parser : public cs::AbstractParser<char> {
  public:
    using EnginePtr = Encode::EnginePtr<T>;
    using Engine    = typename EnginePtr::element_type;
    using Result    = std::list<EnginePtr>;

    using  size_type = typename Engine::size_type;
    using value_type = typename Engine::value_type;

    Parser() noexcept = default;

    ~Parser() noexcept = default;

    Result result;

  protected:
    bool initialize()
    {
      using ctx = cs::LexerContext<char_type>;

      _lexer.addScanner(ctx::CharLiteralScanner::make("(),={}[]:@"));
      _lexer.addScanner(ctx::CIdentifierScanner::make(TOK_Identifier));
      _lexer.addScanner(ctx::CIntegralScanner<value_type>::make(TOK_Integral, true));
      _lexer.addScanner(ctx::CStringScanner::make(TOK_String));

      _lexer.setFlags(cs::LexerFlag::ScanLF);

      _names = EncodeTokenNames::make();

      result.clear();

      return true;
    }

    void start()
    {
      while( isFirstAssignment()  ||  isLookAhead('\n') ) {
        if( isFirstAssignment() ) {
          parseAssignment();
        }

        check('\n');
      }

      check(cs::TOK_EndOfInput);
    }

    bool isFirstAssignment() const
    {
      return isFirstEncode();
    }

    /*
     * Grammar:
     *
     * Assignment = Encode '=' FieldList .
     */
    void parseAssignment()
    {
      parseEncode();

      check('=');

      parseFieldList();
    }

    bool isFirstEncode() const
    {
      return isLookAheadValue<std::string>(TOK_Identifier, "Encode");
    }

    /*
     * Grammar:
     *
     * Encode = identifier<'Encode'> '(' integral ',' string ')' .
     */
    void parseEncode()
    {
      checkValue<std::string>(TOK_Identifier, "Encode");

      check('(');

      check(TOK_Integral);
      const size_type numBits = currentValue<value_type>();

      check(',');

      check(TOK_String);
      const std::string text = currentValue<std::string>();

      check(')');

      EnginePtr engine = Engine::make(numBits, text);
      if( !engine ) {
        throwErrorMessage(_currentToken->line(), "invalid encode declaration");
      }

      result.push_back(std::move(engine));
    }

    /*
     * Grammar:
     *
     * Field = ( identifier | integral ) '[' integral ':' integral ']' '@' integral .
     */
    void parseField()
    {
      cs::TokenPtr fieldType;
      if( isLookAhead(TOK_Identifier)  ||  isLookAhead(TOK_Integral) ) {
        scan();
        fieldType = std::move(_currentToken);
      } else {
        throwUnexpectedToken(_lookAheadToken);
      }

      check('[');

      check(TOK_Integral);
      const size_type to = currentValue<value_type>();

      check(':');

      check(TOK_Integral);
      const size_type from = currentValue<value_type>();

      check(']');

      check('@');

      check(TOK_Integral);
      const size_type at = currentValue<value_type>();

      const bool ok = fieldType->id() == TOK_Identifier
          ? result.back()->addVariable(cs::Token::to_value<std::string>(fieldType),
                                       from, to, at)
          : result.back()->addLiteral(cs::Token::to_value<value_type>(fieldType),
                                      from, to, at);
      if( !ok ) {
        throwErrorMessage(_currentToken->line(), "invalid field definition");
      }
    } // parseField()

    /*
     * Grammar:
     *
     * FieldList = '{' Field { ',' Field } '}' .
     */
    void parseFieldList()
    {
      check('{');

      parseField();

      while( isLookAhead(',') ) {
        scan();

        parseField();
      }

      check('}');
    }
  };

} // namespace Encode
