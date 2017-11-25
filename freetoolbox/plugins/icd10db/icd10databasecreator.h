/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ICD10DATABASECREATOR_H
#define ICD10DATABASECREATOR_H

#include <coreplugin/itoolpage.h>
#include <coreplugin/ftb_constants.h>

#include <QWidget>
#include <QProgressDialog>

namespace Utils {
class HttpDownloader;
}

namespace Icd10 {
namespace Internal {
class Icd10Step;

class Icd10DatabasePage : public Core::IToolPage
{
    Q_OBJECT

public:
    Icd10DatabasePage(QObject *parent = 0);
    ~Icd10DatabasePage();

    virtual QString id() const {return objectName();}
    virtual QString name() const;
    virtual QString category() const;
    virtual QIcon icon() const;

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);

private:
    Icd10::Internal::Icd10Step *_step;
};


namespace Ui {
    class Icd10DatabaseWidget;
}

class Icd10DatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Icd10DatabaseWidget(QWidget *parent = 0);
    ~Icd10DatabaseWidget();
    void setStep(Icd10::Internal::Icd10Step *step);

private Q_SLOTS:
    bool on_startCreation_clicked();
    bool onDownloadFinished();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Icd10DatabaseWidget *ui;
    Icd10::Internal::Icd10Step *_step;
};

}  //  End namespace Internal
}  //  End namespace Icd10

#endif // ICD10DATABASECREATOR_H
