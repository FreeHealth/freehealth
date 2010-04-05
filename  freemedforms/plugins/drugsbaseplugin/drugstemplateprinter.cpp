/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "drugstemplateprinter.h"
#include "drugsmodel.h"
#include "drugsio.h"

#include <utils/global.h>

using namespace DrugsDB;
using namespace DrugsDB::Internal;

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
    if (n > 1) {
        // Ask user : print separately or merge into an unique prescription
        int r = Utils::withButtonsMessageBox(tr("Print separately or merge printing."),
                               tr("You have selected multiple templates, would you "
                                  "print them separately or merge templates for printing "
                                  "on a single order ?"), "",
                               QStringList() << tr("Print separately") << tr("Merge and print"));
        if (r==1) {
            DrugsDB::DrugsModel *model = new DrugsDB::DrugsModel;
            foreach(const Templates::ITemplate *t, iTemplates) {
                DrugsIO::prescriptionFromXml(model, t->content(), DrugsIO::AppendPrescription);
            }
            bool r = DrugsIO::printPrescription(model);
            delete model;
            model = 0;
            return r;
        } else if (r==-1) {
            return false;
        }
    }

    DrugsDB::DrugsModel *model = new DrugsDB::DrugsModel;
    foreach(const Templates::ITemplate *t, iTemplates) {
        DrugsIO::prescriptionFromXml(model, t->content(), DrugsIO::ReplacePrescription);
        DrugsIO::printPrescription(model);
    }
    delete model;
    model = 0;
    return true;
}
