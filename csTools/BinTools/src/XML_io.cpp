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

#include <QtWidgets/QTabWidget>
#include <QtXml/QDomDocument>

#include "Calculator/WCalculatorPage.h"
#include "Encoder/WEncoderPage.h"
#include "XML_io.h"
#include "XML_tags.h"

namespace impl_xml {

  template<typename PageT>
  inline bool load(QTabWidget *tabWidget, const QDomElement& xml_page)
  {
    TabPagePtr page = PageT::make();

    if( !page  ||  !page->load(xml_page) ) {
      return false;
    }

    tabWidget->addTab(page.release(), PageT::label());

    return true;
  }

} // namespace impl_xml

bool xmlRead(QTabWidget *tabWidget, const QString& content)
{
  if( content.isEmpty() ) {
    return false;
  }

  QDomDocument doc;
  if( !doc.setContent(content) ) {
    return false;
  }

  const QDomElement xml_root = doc.firstChildElement(XML_BinTools);
  if( xml_root.isNull() ) {
    return false;
  }

  for(QDomElement xml_page = xml_root.firstChildElement(XML_Page);
      !xml_page.isNull();
      xml_page = xml_page.nextSiblingElement(XML_Page)) {

    if( impl_xml::load<WCalculatorPage>(tabWidget, xml_page) ) {
      continue;
    }

    if( impl_xml::load<WEncoderPage>(tabWidget, xml_page) ) {
      continue;
    }

  } // For Each Page in XML

  return true;
}

QString xmlWrite(const QTabWidget *tabWidget)
{
  QDomDocument doc;

  QDomProcessingInstruction pi = doc.createProcessingInstruction(XML_pitarget, XML_pidata);
  doc.appendChild(pi);

  QDomElement xml_root = doc.createElement(XML_BinTools);
  doc.appendChild(xml_root);

  for(int i = 0; i < tabWidget->count(); i++) {
    const WTabPageBase *page = dynamic_cast<const WTabPageBase*>(tabWidget->widget(i));
    if( page != nullptr ) {
      page->save(xml_root);
    }
  }

  return doc.toString(2);
}
