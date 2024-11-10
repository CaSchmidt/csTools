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

#include <utility>
#include <vector>

#include <Encode/Engine.h>

#include <QtCore/QAbstractTableModel>

class EncodeVariablesModel : public QAbstractTableModel {
  Q_OBJECT
public:
  using EnginePtr = Encode::EnginePtr<uint64_t>;
  using     Store = cs::element_of_ptr_t<EnginePtr>::Store;

  using value_type = cs::element_of_ptr_t<EnginePtr>::value_type;

  enum Columns : int {
    COL_Variable = 0,
    COL_Value,
    NumColumns
  };

  EncodeVariablesModel(QObject *parent = nullptr);
  ~EncodeVariablesModel();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, const int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex& index = QModelIndex()) const;
  QVariant headerData(const int section, const Qt::Orientation orientation,
                      const int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;

  bool setData(const QModelIndex& index, const QVariant& value,
               const int role = Qt::EditRole);

  void clear();
  bool addVariables(const EnginePtr& engine);
  void set(const Store& store);
  Store store() const;

private:
  using Variable  = std::pair<QString,value_type>;
  using Variables = std::vector<Variable>;

  Variables _variables;
};
