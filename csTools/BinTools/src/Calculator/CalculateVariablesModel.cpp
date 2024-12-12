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

#include <QtGui/QFont>
#include <QtWidgets/QWidget>

#include <cs/Core/Container.h>
#include <cs/Core/QStringUtil.h>

#include "Calculator/CalculateVariablesModel.h"

////// public ////////////////////////////////////////////////////////////////

CalculateVariablesModel::CalculateVariablesModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  impl_clear();
}

CalculateVariablesModel::~CalculateVariablesModel()
{
}

int CalculateVariablesModel::columnCount(const QModelIndex& /*parent*/) const
{
  return NumColumns;
}

QVariant CalculateVariablesModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }

  const std::size_t row = static_cast<std::size_t>(index.row());
  const int      column = index.column();

  const Identifier& name = _names[row];

  if(        role == Qt::DisplayRole ) {
    if(        column == COL_Variable ) {
      return cs::toQString(cs::toUtf8StringView(name));
    } else if( column == COL_Value ) {
      const auto     value = _variables.at(name);
      const QString valStr = _base == Binary
          ? QStringLiteral("%1").arg(value, sizeof(value)*8, 2, cs::L1C('0'))
          : _base == Hexadecimal
            ? QStringLiteral("%1").arg(value, sizeof(value)*2, 16, cs::L1C('0'))
            : QString::number(value);

      return valStr;
    }
  } else if( role == Qt::FontRole ) {
    QWidget    *view = qobject_cast<QWidget*>(parent());
    const QFont font = view != nullptr
        ? view->font()
        : QFont();
    const int weight = !_result.empty()  &&  _result == name
        ? QFont::Bold
        : QFont::Normal;

    return QFont(QStringLiteral("Source Code Pro"), font.pointSize(), weight);
  } // Qt::ItemDataRole

  return QVariant();
}

QVariant CalculateVariablesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( role == Qt::DisplayRole ) {
    if(        orientation == Qt::Horizontal ) {
      if(        section == COL_Variable ) {
        return tr("Variable");
      } else if( section == COL_Value ) {
        const QString postfix = _base == Binary
            ? QStringLiteral(" (bin)")
            : _base == Hexadecimal
              ? QStringLiteral(" (hex)")
              : QString();

        return tr("Value") + postfix;
      }
    } else if( orientation == Qt::Vertical ) {
      return section + 1;
    } // Qt::Orientation
  } // Qt::ItemDataRole

  return QVariant();
}

int CalculateVariablesModel::rowCount(const QModelIndex& /*parent*/) const
{
  return static_cast<int>(_names.size());
}

CalculateVariablesModel::Base CalculateVariablesModel::base() const
{
  return _base;
}

void CalculateVariablesModel::setBase(const Base b)
{
  _base = b;

  emit headerDataChanged(Qt::Horizontal, COL_Value, COL_Value);
  if( rowCount() > 0 ) {
    emit dataChanged(index(0, COL_Value), index(rowCount() - 1, COL_Value));
  }
}

void CalculateVariablesModel::set(Variables variables, Identifier result)
{
  beginResetModel();

  impl_clear();

  _variables = std::move(variables);
  if( !result.empty()  &&  _variables.contains(result) ) {
    _result = std::move(result);
  }

  const auto lambda_ex = [](const Variables::value_type& value) -> Identifier {
    return value.first;
  };

  _names = cs::toSequence<std::vector>(_variables.begin(), _variables.end(), lambda_ex, true);

  endResetModel();
}

////// private ///////////////////////////////////////////////////////////////

void CalculateVariablesModel::impl_clear()
{
  _names.clear();
  _result.clear();
  _variables.clear();
}
