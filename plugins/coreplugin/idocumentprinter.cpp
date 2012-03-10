/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "idocumentprinter.h"

/**
  \class Core::IDocumentPrinter
  \brief This class is a Document Printer's manager.
  The printerplugin will create one instance of it and leave it in the pluginmanager pool.
  Get it, and use it for your printing. But remember to never delete it.
  If in user settings, PDF cached is enabled, you can retrieve all printed docs in the QList<Doc>.

  \code
  // Get the IDocumentPrinter object from objects pool
  inline static Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

  // Your print function
  void print(const QString &html)
  {
      Core::IDocumentPrinter *p = printer();
      if (!p) {
          Utils::Log::addError(this, "No IDocumentPrinter found", __FILE__, __LINE__);
          return;
      }

      p->clearTokens();
      QHash<QString, QVariant> tokens;
      tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, this->windowTitle());
      p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);
      p->print(html, m_Papers, m_Duplicata);
  }
  \endcode
  \todo create a queue that manages printing jobs
*/

using namespace Core;

QList<PrintedDocumentTracer> Core::IDocumentPrinter::m_Docs;

void IDocumentPrinter::addPrintedDoc(const QString &fileName, const QString &docName, const QDateTime &dt, const QString &userUid)
{
    PrintedDocumentTracer d;
    d.m_docName = docName;
    d.m_fileName = fileName;
    d.m_userUid = userUid;
    d.m_dateTime = dt;
    m_Docs.append(d);
}
