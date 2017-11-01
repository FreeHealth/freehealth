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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Tools::Internal::HprimIntegratorDialog
 */

#include "hprimintegratordialog.h"
#include "constants.h"
#include "ui_hprimintegratordialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>

#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/iformitemdata.h>
#include <formmanagerplugin/episodemanager.h>
#include <formmanagerplugin/constants_settings.h>

#include <patientbaseplugin/patientmodel.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/hprimparser.h>
#include <translationutils/constants.h>

#include <QStringListModel>
#include <QCryptographicHash>
#include <QPushButton>

#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Form::EpisodeManager &episodeManager() {return Form::FormCore::instance().episodeManager();}

namespace Tools {
namespace Internal {
class HprimIntegratorDialogPrivate
{
public:
    HprimIntegratorDialogPrivate(HprimIntegratorDialog *parent) :
        ui(0),
        _formItemsUidModel(0),
        q(parent)
    {
    }
    
    ~HprimIntegratorDialogPrivate()
    {
        delete ui;
    }

    // Return the SHA1 hash of the string
    QByteArray sha1(const QString &string)
    {
        QByteArray hashData = QCryptographicHash::hash(string.toUtf8(), QCryptographicHash::Sha1);
        return hashData.toHex();
    }

    Utils::HPRIM::HprimMessage &parseHprimContent(const QString &hprimContent)
    {
        return Utils::HPRIM::parseHprimRawSource(hprimContent);
    }

    // Find the FormItem to use for data importation
    void findFormItem()
    {
        // Initialize vars
        _formItems.clear();
        if (!_formItemsUidModel) {
            _formItemsUidModel = new QStringListModel(q);
        }

        // Get formUids from the settings
        QStringList itemUid = settings()->value(Constants::S_FORMITEM_UUIDS).toStringList(); //"GP::Basic::Consultation::Results::Textual";
        QStringList itemLabel;

        // Add EmptyRoot FormMain data of the central form
        QList<Form::FormMain*> emptyRoot = formManager().allEmptyRootForms();
        foreach(Form::FormMain *root, emptyRoot) {
            foreach(Form::FormItem *i, root->flattenedFormItemChildren()) {
                if (i->spec()->useForHprimImportation()
                        || itemUid.contains(i->uuid())) {
                    _formItems << i;
                    _itemUidForModel << i->uuid();
                    // Create the full label of the item
                    QString label = i->spec()->label();
                    Form::FormMain *parent = i->parentFormMain();
                    while (parent) {
                        if (!parent->spec()->label().isEmpty())
                            label.prepend(QString("%1 / ").arg(parent->spec()->label()));
                        parent = parent->parentFormMain();
                    }
                    itemLabel << label;
                }
            }
        }
        if (_formItems.isEmpty())
            LOG_ERROR_FOR(q, "No FormItem found");
        _formItemsUidModel->setStringList(itemLabel);
    }

    // Return the selected patient for the importation
    // Or a null QString if an error occured
    QString getPatientUid(const Utils::HPRIM::HprimHeader &hdr)
    {
        ui->patientSearch->setText(QString("%1 %2").arg(hdr.patientName()).arg(hdr.patientFirstName()));
        // Try to find the correct patient uuid
        Patients::PatientModel *model = new Patients::PatientModel(q);
        model->setFilter(hdr.patientName(), hdr.patientFirstName());
        if (model->rowCount() == 1) {
            ui->patientInfo->setText(QCoreApplication::translate("HprimIntegratorDialog", "Patient correctly detected: %1; %2")
                                        .arg(model->data(model->index(0, Core::IPatient::FullName)).toString())
                                     .arg(QLocale().toString(model->data(model->index(0, Core::IPatient::DateOfBirth)).toDate(), QLocale::LongFormat)));
            QModelIndex index = model->index(0, Core::IPatient::Uid);
            return model->data(index).toString();
        } else if (model->rowCount() == 0) {
            ui->patientInfo->setText(QCoreApplication::translate("HprimIntegratorDialog", "No patient found with the following names: %1; %2")
                                        .arg(hdr.patientName())
                                        .arg(hdr.patientFirstName()));
        } else {
            ui->patientInfo->setText(QCoreApplication::translate("HprimIntegratorDialog", "There are multiple patient with the same names: %1 %2. You will have to select the correct one.")
                                        .arg(hdr.patientName())
                                        .arg(hdr.patientFirstName()));
        }
        return QString::null;
    }

    // Create a new episode for the FormMain containing the FormItem
    // Import the HPRIM content to the FormItem
    // Save the newly created episode
    // Return the SHA1 of the FormItem content (as a control key)
    QString populateFormItem(Form::FormItem *item)
    {
        // Create an episode for the FormMain
        Form::EpisodeModel *model = new Form::EpisodeModel(item->parentFormMain(), q);
        model->initialize();
        model->setCurrentPatient(_patientUid);

        if (!model->insertRow(0)) {
            LOG_ERROR_FOR(q, "Unable to create an episode");
            return QString::null;
        }

        // Label the episode
        int row = model->rowCount() - 1;
        model->setData(model->index(row, Form::EpisodeModel::Label), "HPRIM");
        if (!item->itemData()) {
            LOG_ERROR_FOR(q, "No item data to fill");
            return QString::null;
        }

        // Add the content of the viewer to the FormItem
        QString html = QString("<pre>%1</pre>").arg(_hprimMessage.toRawSource().replace("<", "&lt;"));
        item->itemData()->setData(0, html);

        // getCurrentXmlEpisode()
        QHash<QString, QString> xmlData;
        foreach(Form::FormItem *it, item->parentFormMain()->flattenedFormItemChildren()) {
            if (it->itemData()) {
                xmlData.insert(it->uuid(), it->itemData()->storableData().toString());
            }
        }
        QString xml = Utils::createXml(Form::Constants::XML_FORM_GENERAL_TAG, xmlData, 2, false);
        model->setData(model->index(row, Form::EpisodeModel::XmlContent), xml);

        // Save the episode
        model->submit();
        QString data = item->itemData()->data(0).toString().replace("<pre ", "<p ").replace("</pre", "</p");
        return sha1(data.toUtf8());
    }

    bool checkFormItemContent(const QString &contentSha1, Form::FormItem *item)
    {
        Q_ASSERT(item);
        if (!item)
            return false;
        if (!item->itemData())
            return false;
        if (!item->parentFormMain())
            return false;
        item->parentFormMain()->clear();
        Form::EpisodeModel *model = new Form::EpisodeModel(item->parentFormMain(), q);
        model->initialize();
        model->setCurrentPatient(_patientUid);
        model->populateFormWithLatestValidEpisodeContent();
        QString data = item->itemData()->data(0).toString().replace("<pre ", "<p ").replace("</pre", "</p");

        if (sha1(data.toUtf8()) != contentSha1) {
            LOG_ERROR_FOR(q, "Wrong importation SHA1 validation");
            return false;
        } else {
            LOG_FOR(q, "Importation correctly checked");
        }
        return true;
    }

    // Return the selected form item to use for the data importation,
    // or zero if no form items are available.
    Form::FormItem *getSelectedFormItem()
    {
        Q_ASSERT(!_formItems.isEmpty());
        if (_formItems.isEmpty())
            return 0;
        if (!ui->formItemListView->selectionModel()->hasSelection())
            return _formItems.at(0);
        QString uid = _itemUidForModel.at(ui->formItemListView->selectionModel()->currentIndex().row());
        foreach(Form::FormItem *item, _formItems) {
            if (item->uuid() == uid)
                return item;
        }
        return _formItems.at(0);
    }

public:
    Ui::HprimIntegratorDialog *ui;
    QString _hprimContent, _patientUid;
    QList<Form::FormItem*> _formItems;
    Utils::HPRIM::HprimMessage _hprimMessage;
    QStringListModel *_formItemsUidModel;
    QStringList _itemUidForModel;

private:
    HprimIntegratorDialog *q;
};
} // namespace Internal
} // end namespace Tools


/*! Constructor of the Tools::Internal::HprimIntegratorDialog class */
HprimIntegratorDialog::HprimIntegratorDialog(QWidget *parent) :
    QDialog(parent),
    d(new HprimIntegratorDialogPrivate(this))
{
    d->ui = new Ui::HprimIntegratorDialog;
    d->ui->setupUi(this);
    d->findFormItem();
    d->ui->formItemListView->setModel(d->_formItemsUidModel);
    connect(d->ui->patientSearch, SIGNAL(patientSelected(QString,QString)), this, SLOT(onPatientSelected(QString,QString)));
}

/*! Destructor of the Tools::Internal::HprimIntegratorDialog class */
HprimIntegratorDialog::~HprimIntegratorDialog()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool HprimIntegratorDialog::initialize(const QString &hprimContent)
{
    // Parse HPRIM message
    d->_hprimContent = hprimContent;
    d->_hprimMessage = d->parseHprimContent(hprimContent);

    // Populate the viewer
    d->ui->hprimContent->clear();
    d->ui->hprimContent->appendHtml(d->_hprimMessage.toBasicHtml());
    QTextCursor cursor = d->ui->hprimContent->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    d->ui->hprimContent->setTextCursor(cursor);

    // Find the related patient
    d->_patientUid = d->getPatientUid(d->_hprimMessage.header());

    QPushButton *ok = d->ui->buttonBox->button(QDialogButtonBox::Ok);
    if (ok)
        ok->setEnabled(!d->_patientUid.isEmpty());
    return true;
}

void HprimIntegratorDialog::onPatientSelected(const QString &fullName, const QString &uid)
{
    d->_patientUid = uid;
    d->ui->patientInfo->setText(
                QCoreApplication::translate("HprimIntegratorDialog",
                                            "You selected the following patient: %1")
                .arg(fullName)
                );
    QPushButton *ok = d->ui->buttonBox->button(QDialogButtonBox::Ok);
    if (ok)
        ok->setEnabled(!d->_patientUid.isEmpty());
}

void HprimIntegratorDialog::done(int r)
{
    if (r == QDialog::Rejected) {
        QDialog::done(r);
        return;
    }

    // No valid patient uid?
    if (d->_patientUid.isEmpty()) {
        // ERROR
        QDialog::done(QDialog::Rejected);
        return;
    }

    // Get the form item
    Form::FormItem *item = d->getSelectedFormItem();
    if (!item) {
        // ERROR
        QDialog::done(QDialog::Rejected);
        return;
    }

    // Populate the item & check
    QString sha1 = d->populateFormItem(item);
    if (sha1.isEmpty()) {
        // ERROR
        QDialog::done(QDialog::Rejected);
        return;
    }

    // Du to Qt HTML interpretation, we can not be 100% sure of
    // the episode exact content matching. So if check fails, the dialog
    // is Accepted.
    if (!d->checkFormItemContent(sha1, item)) {
        // ERROR
        QDialog::done(QDialog::Accepted);
        return;
    }

    QDialog::done(QDialog::Accepted);
}
