/****************************************************************************
** Copyright (c) 2020, Carsten Schmidt. All rights reserved.
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

#include <QtWidgets/QAbstractItemView>

#include <cs/Qt/ItemModel.h>
#include <QtCreator/HighlightingItemDelegate.h>

#include "ResultsProxyDelegate.h"

////// Constants /////////////////////////////////////////////////////////////

constexpr int kIndexDepth = 1;
constexpr int kTabWidth = 8;

////// public ////////////////////////////////////////////////////////////////

ResultsProxyDelegate::ResultsProxyDelegate(QAbstractItemView *view)
  : QAbstractItemDelegate(view)
{
  _delegate  = view->itemDelegate();
  _highlight = new QtCreator::HighlightingItemDelegate(kTabWidth, view);

  view->setItemDelegate(this);
}

ResultsProxyDelegate::~ResultsProxyDelegate()
{
}

void ResultsProxyDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if( cs::indexDepth(index) == kIndexDepth ) {
    _highlight->paint(painter, option, index);
  } else {
    _delegate->paint(painter, option, index);
  }
}

QSize ResultsProxyDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if( cs::indexDepth(index) == kIndexDepth ) {
    return _highlight->sizeHint(option, index);
  }
  return _delegate->sizeHint(option, index);
}

void ResultsProxyDelegate::setTabWidth(const int width)
{
  _highlight->setTabWidth(width);
}
