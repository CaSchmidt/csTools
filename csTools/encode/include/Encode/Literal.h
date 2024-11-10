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

#include "Encode/Field.h"

namespace Encode {

  template<typename T>
  class LiteralField : public IField<T> {
  private:
    struct ctor_tag {
      ctor_tag() noexcept = default;
    };

  public:
    using value_type = typename IField<T>::value_type;

    LiteralField(const value_type value,
                 const size_t from, const size_t to,
                 const size_t at,
                 const ctor_tag& = ctor_tag()) noexcept
      : IField<value_type>(from, to, at)
      , _value{value}
    {
    }

    ~LiteralField() noexcept
    {
    }

    value_type value(const value_type) const
    {
      return IField<T>::value(_value);
    }

    static FieldPtr<T> make(const T value,
                            const size_t from, const size_t to,
                            const size_t at = 0)
    {
      FieldPtr<T> ptr = std::make_unique<LiteralField<T>>(value, from, to, at);
      if( ptr  &&  !ptr->isValid() ) {
        ptr.reset();
      }
      return ptr;
    }

  private:
    LiteralField() noexcept = delete;

    value_type _value{};
  };

} // namespace Encode
