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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugstemplateprinter.h"
#include "drugbasecore.h"
#include "drugsmodel.h"
#include "drugsio.h"
#include "prescriptionprinter.h"

#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/dialogs/simpletextdialog.h>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsIO &drugsIo() {return DrugsDB::DrugBaseCore::instance().drugsIo();}
static inline DrugsDB::PrescriptionPrinter &prescriptionPrinter() {return DrugsDB::DrugBaseCore::instance().prescriptionPrinter();}

QString DrugsTemplatePrinter::mimeType() const
{
    return DrugsIO::prescriptionMimeTypes().at(0);
}

bool DrugsTemplatePrinter::printTemplates(const QList<const Templates::ITemplate *> iTemplates) const
{
//    qWarning() << iTemplates;
    int n = iTemplates.count();
    if (!n)
        return true;
    DrugsDB::DrugsModel *model = new DrugsDB::DrugsModel;
    int r = 1;
    if (n > 1) {
        // Check interactions in the merged templates
        foreach(const Templates::ITemplate *t, iTemplates) {
            drugsIo().prescriptionFromXml(model, t->content(), DrugsIO::AppendPrescription);
        }
        bool interactions = model->prescriptionHasInteractions();
        bool allergy = model->prescriptionHasAllergies();
        if (interactions || allergy) {
            QString title = tr("Warning: ");
            if (interactions)
                title += tr("Interactions found. ");
            if (allergy)
                title += tr("Allergies found. ");
            int cancel;
            while (r!=0) {
                if (interactions) {
                    r = Utils::withButtonsMessageBox(title,
                                                     tr("What do you want to do?"), "",
                                                     QStringList() << tkTr(Trans::Constants::FILEPRINT_TEXT) << tkTr(Trans::Constants::VIEWINTERACTIONS_TEXT) << tkTr(Trans::Constants::CANCEL));
                    cancel = 2;
                } else {
                    r = Utils::withButtonsMessageBox(title,
                                                     tr("What do you want to do?"), "",
                                                     QStringList() << tkTr(Trans::Constants::FILEPRINT_TEXT) << tkTr(Trans::Constants::CANCEL));
                    cancel = 1;
                }

                if (r==cancel)
                    return true;
                if (interactions && r==1) {
                    Core::SimpleTextDialog dlg(tr("Synthetic interactions") + " - " + qApp->applicationName(),
                                               "",
                                               Core::ICore::instance()->mainWindow());
                    dlg.setHtml(model->index(0, DrugsDB::Constants::Interaction::FullSynthesis).data().toString());
                    dlg.setPrintDuplicata(true);
                    dlg.setUserPaper(Core::IDocumentPrinter::Papers_Prescription_User);
                    dlg.setHelpPageUrl(Constants::H_INTERACTION_SYNTHETISOR);
                    dlg.exec();
                }
            }
        }

        // Ask user : print separately or merge into an unique prescription
        r = Utils::withButtonsMessageBox(tr("Print separately or merge printing."),
                               tr("You have selected multiple templates, would you "
                                  "print them separately or merge templates for printing "
                                  "on a single order?"), "",
                               QStringList() << tr("Print separately") << tr("Merge and print") << tkTr(Trans::Constants::CANCEL));
        if (r==1) {
            bool ok = prescriptionPrinter().print(model);
            delete model;
            model = 0;
            return ok;
        } else if (r==-1 || r==2) {
            return false;
        }
    }

    model->clearDrugsList();
    foreach(const Templates::ITemplate *t, iTemplates) {
        drugsIo().prescriptionFromXml(model, t->content(), DrugsIO::ReplacePrescription);
        prescriptionPrinter().print(model);
    }
    delete model;
    model = 0;
    return true;
}
