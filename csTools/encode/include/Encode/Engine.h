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

#include <Encode/List.h>
#include <Encode/Literal.h>
#include <Encode/Variable.h>

namespace Encode {

  template<typename>
  class Engine;

  template<typename T>
  using EnginePtr = std::unique_ptr<Engine<T>>;

  template<typename T>
  class Engine {
  private:
    struct ctor_tag {
      ctor_tag() noexcept = default;
    };

  public:
    using  size_type = size_t;
    using value_type = T;

    using Field = FieldPtr<value_type>;
    using List  = FieldList<value_type>;
    using Store = VariableStore<value_type>;

    using Literal  = LiteralField<value_type>;
    using Variable = VariableField<value_type>;

    Engine(const size_type numBits, const std::string_view& text,
           const ctor_tag& = ctor_tag()) noexcept
      : _numBits{numBits}
      , _text(text)
    {
    }

    ~Engine() noexcept
    {
    }

    bool isValid() const
    {
      if( !IField<value_type>::isRange(_numBits) ) {
        return false;
      }

      for(const Field& field : _fields) {
        if( !field->isValid(_numBits) ) {
          return false;
        }

        // TODO: check for non-overlapping composition
      }

      return true;
    }

    size_type numBits() const
    {
      return _numBits;
    }

    std::string text() const
    {
      return _text;
    }

    bool addLiteral(const value_type value,
                    const size_type from, const size_type to,
                    const size_type at = 0)
    {
      return addField(Literal::make(value, from, to, at));
    }

    bool addVariable(const std::string_view& name,
                     const size_type from, const size_type to,
                     const size_type at = 0)
    {
      return addField(Variable::make(name, from, to, at));
    }

    value_type compose(const Store& store) const
    {
      return ::Encode::compose(_fields, store);
    }

    size_type initialize(Store& store,
                         const value_type initValue = 0,
                         const bool keep_values = false) const
    {
      return ::Encode::initialize(store, _fields, initValue, keep_values);
    }

    static EnginePtr<value_type> make(const size_type numBits,
                                      const std::string_view& text)
    {
      EnginePtr<value_type> ptr = std::make_unique<Engine>(numBits, text);
      if( ptr  &&  !ptr->isValid() ) {
        ptr.reset();
      }
      return ptr;
    }

  private:
    bool addField(Field field)
    {
      if( !field  ||  !field->isValid(_numBits) ) {
        return false;
      }

      _fields.push_back(std::move(field));

      return true;
    }

    List        _fields;
    size_type   _numBits{0};
    std::string _text;
  };

} // namespace Encode
