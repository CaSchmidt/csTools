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
#include <cs/Qt/XML.h>

#include <Encode/Parser.h>

#include <QtGui/QFontDatabase>
#include <QtXml/QDomDocument>

#include "Encoder/WEncoderPage.h"
#include "ui_WEncoderPage.h"

#include "Encoder/EncodeResultsModel.h"
#include "Encoder/EncodeVariablesModel.h"
#include "global.h"
#include "XML_tags.h"

////// Private ///////////////////////////////////////////////////////////////

namespace impl_encoder {

  struct EncoderPage {
    using Parser = Encode::Parser<EncoderConfig::value_type>;

    EncoderPage() noexcept = default;

    Parser                   parser;
    EncodeResultsModel     *results{nullptr};
    EncodeVariablesModel *variables{nullptr};
  };

  static_assert( std::is_same_v<EncoderPage::Parser::value_type,EncodeResultsModel::value_type> );

  static_assert( std::is_same_v<EncoderPage::Parser::value_type,EncodeVariablesModel::value_type> );

} // namespace impl_encoder

////// public ////////////////////////////////////////////////////////////////

WEncoderPage::WEncoderPage(QWidget *parent, const Qt::WindowFlags flags, const ctor_tag&)
  : WTabPageBase(parent, flags)
  , ui(std::make_unique<Ui::WEncoderPage>())
  , d(std::make_unique<impl_encoder::EncoderPage>())
{
  ui->setupUi(this);

  // Font ////////////////////////////////////////////////////////////////////

  const QFontDatabase db;
  const QFont font = db.font(QStringLiteral("Source Code Pro"),
                             QStringLiteral("Regular"),
                             12);
  ui->editorWidget->setFont(font);

  // Data Models /////////////////////////////////////////////////////////////

  d->results = new EncodeResultsModel(ui->resultsView);
  ui->resultsView->setModel(d->results);

  d->variables = new EncodeVariablesModel(ui->variablesView);
  ui->variablesView->setModel(d->variables);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->clearButton, &QPushButton::clicked,
          this, &WEncoderPage::clearVariables);
  connect(ui->encodeButton, &QPushButton::clicked,
          this, &WEncoderPage::encode);
}

WEncoderPage::~WEncoderPage()
{
}

bool WEncoderPage::load(const QDomNode& node)
{
  const QDomElement xml_page = node.toElement();
  if( xml_page.isNull()  ||
      xml_page.tagName() != XML_Page  ||
      xml_page.attribute(XML_type) != XML_Encoder ) {
    return false;
  }

  ui->editorWidget->setPlainText(cs::xmlToValue<QString>(xml_page.firstChildElement(XML_Editor),
                                                         XML_Editor));

  ui->msbFirstCheck->setChecked(cs::xmlToValue<bool>(xml_page.firstChildElement(XML_MSBfirst),
                                                     XML_MSBfirst));

  EncodeVariablesModel::Store store;
  {
    const QDomElement xml_variables = xml_page.firstChildElement(XML_Variables);

    for(QDomElement xml_variable = xml_variables.firstChildElement(XML_Variable);
        !xml_variable.isNull();
        xml_variable = xml_variable.nextSiblingElement(XML_Variable)) {
      using value_type = impl_encoder::EncoderPage::Parser::value_type;

      const auto  name = cs::xmlToValue<QString>(xml_variable.firstChildElement(XML_Name),
                                                 XML_Name);
      const auto value = cs::xmlToValue<value_type>(xml_variable.firstChildElement(XML_Value),
                                                    XML_Value);

      if( name.isEmpty() ) {
        continue;
      }

      store.emplace(cs::toString(cs::toUtf8String(name)), value);
    } // For each Variable
  }
  d->variables->set(store);

  return true;
}

void WEncoderPage::save(QDomNode& parent) const
{
  if( parent.isNull() ) {
    return;
  }

  QDomDocument doc = parent.ownerDocument();

  QDomNode xml_page = cs::xmlAppend(parent, XML_Page, {XML_type, XML_Encoder});

  cs::xmlAppend(xml_page, XML_Editor, ui->editorWidget->toPlainText());

  QDomElement xml_variables = doc.createElement(XML_Variables);
  xml_page.appendChild(xml_variables);

  const auto store = d->variables->store();
  for(const auto& variable : store) {
    QDomElement xml_variable = doc.createElement(XML_Variable);
    xml_variables.appendChild(xml_variable);

    const QString name = cs::toQString(cs::toUtf8String(variable.first));
    cs::xmlAppend(xml_variable, XML_Name, name);
    cs::xmlAppend(xml_variable, XML_Value,
                  QStringLiteral("0x%1").arg(cs::toQString(variable.second, 16).toUpper()));
  }

  cs::xmlAppend(xml_page, XML_MSBfirst, ui->msbFirstCheck->isChecked());
}

////// private slots /////////////////////////////////////////////////////////

void WEncoderPage::clearVariables()
{
  d->variables->clear();
}

void WEncoderPage::encode()
{
  using EnginePtr = impl_encoder::EncoderPage::Parser::EnginePtr;
  using     Store = impl_encoder::EncoderPage::Parser::Engine::Store;

  d->results->clear();

  const std::string input = cs::toString(cs::toUtf8String(ui->editorWidget->toPlainText()));

  const bool ok = d->parser.parse(input, global::logger);
  if( !ok ) {
    return;
  }

  for(const EnginePtr& engine : d->parser.result) {
    d->variables->addVariables(engine);
  }

  const Store store = d->variables->store();

  for(const EnginePtr& engine : d->parser.result) {
    d->results->addResult(engine, store, ui->msbFirstCheck->isChecked());
  }
}

////// public static /////////////////////////////////////////////////////////

QString WEncoderPage::label()
{
  return tr("Encoder");
}

TabPagePtr WEncoderPage::make(QWidget *parent, const Qt::WindowFlags flags)
{
  return std::make_unique<WEncoderPage>(parent, flags);
}
