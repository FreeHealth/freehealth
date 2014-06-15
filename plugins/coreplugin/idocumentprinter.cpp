/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "idocumentprinter.h"

/**
  \class Core::IDocumentPrinter
  \brief This class is a Document Printer's manager.
  The printerplugin will create one instance of it and leave it in the pluginmanager pool.
  Get it, and use it for your printing. But remember to \b never delete it.
  If in user settings, PDF cached is enabled, you can retrieve all printed docs in the Core::IDocumentPrinter::printedDocs().

  You can define the headers, watermark and footer to use in the Core::IDocumentPrinter::print() function.

  The tokens can be:
  - global (used for all documents during the print process (header, footer, watermark and document itself)
  - specific to one document: header, footer, watermark and document itself.
  - set this specificities in the Core::IDocumentPrinter::addTokens()

  The print process shows a dialog to the user. The user can modify some params of the print process:
  - select a printer
  - select a 2 n-up printing or not
  - print or not duplicate
  You should not care about these params.

  \code
  // Include files
  #include <coreplugin/idocumentprinter>
  #include <extensionsystem/pluginmanager.h>

  // Get the IDocumentPrinter object from objects pool
  static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

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
  \todo add a addUserTokens(), addPatientTokens() to automatically include the user && patient related tokens (extracted from Core::IUser, Core::IPatient).
*/

/** \fn virtual void Core::IDocumentPrinter::clearTokens() = 0;
  Clear all registered tokens.
 */

/** \fn virtual void Core::IDocumentPrinter::addTokens(const int tokenWhere, const QHash<QString, QVariant> &tokensAndValues) = 0;
  Register tokens \e tokensAndValues for a specific document, or a global usage with \e tokenWhere.
  Use the Core::IDocumentPrinter::TokensWhere enumerator to define the \e tokenWhere value.
  The tokens are represented in a hash of QString (key), QVariant (value).
  For more information about the token management and usage please refer to the web site:
  - French: http://www.freemedforms.com/fr/manuals/freemedforms/tokenmanager
  - English: http://www.freemedforms.com/en/manuals/freemedforms/tokenmanager
 */

/** \fn virtual bool Core::IDocumentPrinter::print(const QTextDocument &text, const int papers = Papers_Generic_User, bool printDuplicata = false) const = 0;
  Prints a QTextDocument \e text using a set of user papers (header, footer, watermark) \e papers. Use the enumerator Core::IDocumentPrinter::PapersToUse to define the /e papers.
  You can automatically print a duplicate.
 */

/** \fn virtual bool Core::IDocumentPrinter::print(QTextDocument *text, const int papers = Papers_Generic_User, bool printDuplicata = false) const = 0;
  Prints a QTextDocument \e text using a set of user papers (header, footer, watermark) \e papers. Use the enumerator Core::IDocumentPrinter::PapersToUse to define the /e papers.
  You can automatically print a duplicate.
 */

/** \fn virtual bool Core::IDocumentPrinter::print(const QString &html, const int papers = Papers_Generic_User, bool printDuplicata = false) const = 0;
  Prints an HTML \e text using a set of user papers (header, footer, watermark) \e papers. Use the enumerator Core::IDocumentPrinter::PapersToUse to define the /e papers.
  You can automatically print a duplicate.
 */

/** \fn virtual bool Core::IDocumentPrinter::printPreview(const QString &html, const int papers = Papers_Generic_User, bool printDuplicata = false) const = 0;
  Show a print preview. \sa print()
 */

/** \fn static void Core::IDocumentPrinter::addPrintedDoc(const QString &fileName, const QString &docName, const QDateTime &dt, const QString &userUid);
  You can use a PDF cache system. This cache system is automatically managed by the print process.
 */

/** \fn static Core::IDocumentPrinter::QList<PrintedDocumentTracer> printedDocs()
  You can use a PDF cache system. This cache system is automatically managed by the print process.
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
