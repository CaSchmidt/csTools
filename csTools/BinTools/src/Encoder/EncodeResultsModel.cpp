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

#include <iterator>

#include <cs/Convert/BufferUtil.h>
#include <cs/Convert/Serialize.h>
#include <cs/Core/QStringUtil.h>

#include "Encoder/EncodeResultsModel.h"

////// public ////////////////////////////////////////////////////////////////

EncodeResultsModel::EncodeResultsModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

EncodeResultsModel::~EncodeResultsModel()
{
}

int EncodeResultsModel::columnCount(const QModelIndex& /*parent*/) const
{
  return NumColumns;
}

QVariant EncodeResultsModel::data(const QModelIndex& index,
                                  const int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }

  const int     column = index.column();
  const Result& result = *std::next(_results.cbegin(), index.row());

  if( role == Qt::DisplayRole ) {
    if(        column == COL_Engine ) {
      return result.first;
    } else if( column == COL_Result ) {
      return result.second;
    }
  } // Qt::ItemDataRole

  return QVariant();
}

Qt::ItemFlags EncodeResultsModel::flags(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  Qt::ItemFlags flags = QAbstractTableModel::flags(index);
  flags |= Qt::ItemIsSelectable;

  return flags;
}

QVariant EncodeResultsModel::headerData(const int section,
                                        const Qt::Orientation orientation,
                                        const int role) const
{
  if( role == Qt::DisplayRole ) {
    if(        orientation == Qt::Horizontal ) {
      if(        section == COL_Engine ) {
        return tr("Engine");
      } else if( section == COL_Result ) {
        return tr("Result");
      }
    } else if( orientation == Qt::Vertical ) {
      return section + 1;
    }
  } // Qt::ItemDataRole

  return QVariant();
}

int EncodeResultsModel::rowCount(const QModelIndex& /*parent*/) const
{
  return static_cast<int>(_results.size());
}

void EncodeResultsModel::clear()
{
  beginResetModel();
  _results.clear();
  endResetModel();
}

bool EncodeResultsModel::addResult(const EnginePtr& engine, const Store& store, const bool is_msb)
{
  if( !engine  ||  !engine->isValid() ) {
    return false;
  }

  const value_type value = engine->compose(store);
  const cs::Buffer valBuffer = is_msb
      ? cs::toBytesBE(value, engine->numBits()/8)
      : cs::toBytesLE(value, engine->numBits()/8);
  const std::string valString = cs::toString(valBuffer, ' ', true);

  const QString  first = cs::toQString(cs::toUtf8String(engine->text()));
  const QString second = cs::toQString(cs::toUtf8String(valString));

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  _results.emplace_back(first, second);
  endInsertRows();

  return true;
}
