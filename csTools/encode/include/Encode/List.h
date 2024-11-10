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

#include <list>

#include "Encode/Store.h"

namespace Encode {

  template<typename T>
  using FieldList = std::list<FieldPtr<T>>;

  template<typename T>
  inline T compose(const FieldList<T>& fields, const VariableStore<T>& store)
  {
    T value = 0;

    for(const FieldPtr<T>& field : fields) {
      if( !field ) {
        continue;
      }

      value |= field->value(getValue<T>(store, field));
    }

    return value;
  }

  template<typename T>
  inline size_t initialize(VariableStore<T>& store, const FieldList<T>& fields,
                           const T initValue = 0, const bool keep_values = false)
  {
    if( !keep_values ) {
      store.clear();
    }

    size_t cntField = 0;
    for(const FieldPtr<T>& field : fields) {
      if( !field ) {
        continue;
      }

      const std::string name = field->name();
      if( name.empty() ) {
        continue;
      }

      if( store.contains(name)  &&  keep_values ) {
        continue;
      }

      store[name] = initValue;

      cntField++;
    } // For each Field

    return cntField;
  }

} // namespace Encode
