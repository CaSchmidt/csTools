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

#include <cs/Core/QStringUtil.h>

#include "Encoder/EncodeVariablesModel.h"

////// public ////////////////////////////////////////////////////////////////

EncodeVariablesModel::EncodeVariablesModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

EncodeVariablesModel::~EncodeVariablesModel()
{
}

int EncodeVariablesModel::columnCount(const QModelIndex& /*parent*/) const
{
  return NumColumns;
}

QVariant EncodeVariablesModel::data(const QModelIndex& index, const int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }

  const int    column = index.column();
  const Variable& var = _variables[index.row()];

  if( role == Qt::DisplayRole ) {
    if(        column == COL_Variable ) {
      return var.first;
    } else if( column == COL_Value ) {
      const QString str = cs::toQString(var.second, 16).toUpper();
      return QStringLiteral("0x%1").arg(str);
    }
  }

  return QVariant();
}

Qt::ItemFlags EncodeVariablesModel::flags(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  Qt::ItemFlags flags = QAbstractTableModel::flags(index);
  flags |= Qt::ItemIsSelectable;

  if( index.column() == COL_Value ) {
    flags |= Qt::ItemIsEditable;
  }

  return flags;
}

QVariant EncodeVariablesModel::headerData(const int section, const Qt::Orientation orientation,
                                          const int role) const
{
  if( role == Qt::DisplayRole ) {
    if(        orientation == Qt::Horizontal ) {
      if(        section == COL_Variable ) {
        return tr("Variable");
      } else if( section == COL_Value ) {
        return tr("Value");
      }
    } else if( orientation == Qt::Vertical ) {
      return section + 1;
    }
  } // Qt::ItemDataRole

  return QVariant();
}

int EncodeVariablesModel::rowCount(const QModelIndex& /*parent*/) const
{
  return static_cast<int>(_variables.size());
}

bool EncodeVariablesModel::setData(const QModelIndex& index, const QVariant& value,
                                   const int role)
{
  if( !index.isValid() ) {
    return false;
  }

  const int column = index.column();
  Variable&    var = _variables[index.row()];

  if( role == Qt::EditRole ) {
    if( column == COL_Value ) {
      bool ok = false;
      const value_type val = cs::toValue<value_type>(value.toString(), &ok, 0);
      if( ok ) {
        var.second = val;

        emit dataChanged(index, index);

        return true;
      }
    }
  } // Qt::ItemDataRole

  return false;
}

void EncodeVariablesModel::clear()
{
  beginResetModel();
  _variables.clear();
  endResetModel();
}

bool EncodeVariablesModel::addVariables(const EnginePtr& engine)
{
  if( !engine ) {
    return false;
  }

  Store store = EncodeVariablesModel::store();
  const std::size_t count = engine->initialize(store, 0, true);
  if( count < 1 ) {
    return false;
  }

  set(store);

  return true;
}

void EncodeVariablesModel::set(const Store& store)
{
  constexpr auto lambda_cmp = [](const Variable& lhs, const Variable& rhs) -> bool {
    return lhs.first < rhs.first;
  };

  beginResetModel();
  _variables.clear();

  if( !store.empty() ) {
    _variables.reserve(store.size());

    for(const Store::value_type& var : store) {
      _variables.emplace_back(cs::toQString(cs::toUtf8String(var.first)), var.second);
    }

    std::sort(_variables.begin(), _variables.end(), lambda_cmp);
  } // !store.empty()
  endResetModel();
}

EncodeVariablesModel::Store EncodeVariablesModel::store() const
{
  Store store;

  for(const Variable& var : _variables) {
    const std::string name = cs::toString(cs::toUtf8String(var.first));
    if( name.empty() ) {
      continue;
    }

    store[name] = var.second;
  }

  return store;
}
