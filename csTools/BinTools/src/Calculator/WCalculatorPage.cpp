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

#include <QtExamples/CodeEditor.h>

#include "Calculator/WCalculatorPage.h"
#include "ui_WCalculatorPage.h"

////// Private ///////////////////////////////////////////////////////////////

namespace impl_priv {

  void setFont(QWidget *w, const int pointSize)
  {
    const QFontDatabase db;
    const QFont font = db.font(QStringLiteral("Source Code Pro"),
                               QStringLiteral("Regular"),
                               pointSize);
    w->setFont(font);
  }

} // namespace impl_priv

////// public ////////////////////////////////////////////////////////////////

WCalculatorPage::WCalculatorPage(QWidget *parent, const Qt::WindowFlags flags,
                                 const ctor_tag&)
  : WTabPageBase(parent, flags)
  , ui(std::make_unique<Ui::WCalculatorPage>())
{
  ui->setupUi(this);

  // Font ////////////////////////////////////////////////////////////////////

  impl_priv::setFont(ui->expressionEdit, 12);
  impl_priv::setFont(ui->historyEdit, 12);
}

WCalculatorPage::~WCalculatorPage()
{
}

QString WCalculatorPage::label()
{
  return tr("Calculator");
}

TabPagePtr WCalculatorPage::make(QWidget *parent, const Qt::WindowFlags flags)
{
  return std::make_unique<WCalculatorPage>(parent, flags);
}
