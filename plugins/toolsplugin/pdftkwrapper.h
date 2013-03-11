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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_PDFTKWRAPPER_H
#define TOOLS_INTERNAL_PDFTKWRAPPER_H

#include <QProcess>
#include <QObject>

/**
 * \file pdftkwrapper.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 07 Feb 2013
*/

namespace Tools {
namespace Internal {
class ToolsPlugin;
class PdfTkWrapperPrivate;

class PdfTkWrapper : public QObject
{
    friend class Tools::Internal::ToolsPlugin;
    Q_OBJECT
    Q_PROPERTY(bool isAvailable READ isAvailable())

protected:
    explicit PdfTkWrapper(QObject *parent = 0);

public:
    static PdfTkWrapper *instance();
    ~PdfTkWrapper();
    
    bool initialize();
    
Q_SIGNALS:
    
public Q_SLOTS:
    // Checker
    bool isAvailable() const;

    // FDF creation helpers
    void beginFdfEncoding();
    void addFdfValue(const QString &fieldName, const QString &value, bool toUpper = true);
//    void addFdfValueFromFormItem(const QString &fieldName, const QString &itemUuid, bool toUpper = true);
    void endFdfEncoding(const QString &filename);
    QString getFdfContent();

    // PDF creation
    bool fillPdfWithFdf(const QString &absPdfFile, const QString &fdfContent, const QString &absFileNameOut, const QString &isoEncoding);

private Q_SLOTS:
    void onProcessError(QProcess::ProcessError);
    void onProcessFinished(int exitCode);

private:
    Internal::PdfTkWrapperPrivate *d;
    static PdfTkWrapper *_instance;
};

} // namespace Internal
} // namespace Tools

#endif // TOOLS_INTERNAL_PDFTKWRAPPER_H

