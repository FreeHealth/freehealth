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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Tools::Internal::HprimIntegrator
 * Central Core::IMode widget for the HPRIM integration process.
 */

#include "hprimintegrator.h"
#include "hprimfilemodel.h"
#include "constants.h"

#include "ui_hprimintegratorwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>

#include <patientbaseplugin/patientmodel.h>

#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/iformitemdata.h>
#include <formmanagerplugin/episodemanager.h>
#include <formmanagerplugin/constants_settings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/hprimparser.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>

#include <QTimer>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QStringListModel>
#include <QCryptographicHash>

#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}

namespace Tools {
namespace Internal {
class HprimIntegratorWidgetPrivate
{
public:
    HprimIntegratorWidgetPrivate(HprimIntegratorWidget *parent) :
        ui(0),
        _fileModel(0),
        _formItemsUidModel(0),
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~HprimIntegratorWidgetPrivate()
    {
        delete ui;
    }
    
    // Read the content of the currently selected file
    QString readFileContent()
    {
        QModelIndex index = ui->dirContentTableView->currentIndex();
        return Utils::readTextFile(_fileModel->fileInfo(index).absoluteFilePath());
    }

    // Return the SHA1 hash of the string
    QByteArray sha1(const QString &string)
    {
        QByteArray hashData = QCryptographicHash::hash(string.toUtf8(), QCryptographicHash::Sha1);
        return hashData.toHex();
    }

    // Find the FormItem to use for data importation and store it into the _formItemsUidModel
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

    // Create a new episode for the FormMain containing the FormItem
    // Import the HPRIM content to the FormItem
    // Save the newly created episode
    // Return the SHA1 of the FormItem content (as a control key)
    QString populateFormItem(Form::FormItem *item)
    {
        // Create an episode for the FormMain
        Form::EpisodeModel *model = new Form::EpisodeModel(item->parentFormMain(), q);
        model->initialize();
        model->setCurrentPatient(ui->dbPatientName->toolTip());

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
        QString html = QString("<pre>%1</pre>").arg(ui->contentViewer->toPlainText().replace("<", "&lt;"));
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
        model->setCurrentPatient(ui->dbPatientName->toolTip());
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
        if (ui->formItemSelector->currentIndex() == -1)
            return 0;
        QString uid = _itemUidForModel.at(ui->formItemSelector->currentIndex());
        foreach(Form::FormItem *item, _formItems) {
            if (item->uuid() == uid)
                return item;
        }
        return 0;
    }

public:
    Ui::HprimIntegratorWidget *ui;
    HprimFileModel *_fileModel;
    QList<Form::FormItem*> _formItems;
    QStringListModel *_formItemsUidModel;
    QStringList _itemUidForModel;

private:
    HprimIntegratorWidget *q;
};
} // namespace Internal
} // namespace Tools


/*! Constructor of the Tools::Internal::HprimIntegrator class */
HprimIntegratorWidget::HprimIntegratorWidget(QWidget *parent) :
    QWidget(parent),
    d(new HprimIntegratorWidgetPrivate(this))
{
    d->ui = new Ui::HprimIntegratorWidget;
    d->ui->setupUi(this);
    d->ui->integrate->setEnabled(false);

    // Set courrier as font of the viewer
    QFont font = d->ui->contentViewer->font();
    font.setFamily("Courrier");
    font.setPointSize(font.pointSize() + 2);
    d->ui->contentViewer->setFont(font);

    // Create the HPRIM model & populate the view
    d->_fileModel = new HprimFileModel(this);
    d->ui->dirContentTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->ui->dirContentTableView->setModel(d->_fileModel);
    d->ui->dirContentTableView->setRootIndex(d->_fileModel->fileRootPath());
    d->ui->dirContentTableView->horizontalHeader()->setSectionResizeMode(HprimFileModel::PatientName, QHeaderView::Stretch);
    d->ui->dirContentTableView->horizontalHeader()->setSectionResizeMode(HprimFileModel::PatientDateOfBirth, QHeaderView::ResizeToContents);
    d->ui->dirContentTableView->horizontalHeader()->setSectionResizeMode(HprimFileModel::FileName, QHeaderView::ResizeToContents);
    d->ui->dirContentTableView->horizontalHeader()->setSectionResizeMode(HprimFileModel::FileDate, QHeaderView::ResizeToContents);
    // Populate the form item selector combo
    d->findFormItem();
    d->ui->formItemSelector->setModel(d->_formItemsUidModel);

    // Connect some signals
    connect(d->ui->dirContentTableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(onFileSelected(QModelIndex, QModelIndex)));
    connect(d->ui->patientSearch, SIGNAL(patientSelected(QString,QString)), this, SLOT(onPatientSelected(QString,QString)));
    connect(d->ui->integrate, SIGNAL(clicked()), this, SLOT(onDataIntegrationRequested()));
}

/*! Destructor of the Tools::Internal::HprimIntegrator class */
HprimIntegratorWidget::~HprimIntegratorWidget()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool HprimIntegratorWidget::initialize()
{
    return true;
}

/**
 * When a file is selected, read its content and show it in the viewer.
 * Tries also to find the related patient.
 */
void HprimIntegratorWidget::onFileSelected(const QModelIndex &current, const QModelIndex &)
{
    // Read the content of the file
    QString content = d->_fileModel->fileContent(current);
    // Try to read HPRIM header
    Utils::HPRIM::HprimMessage msg = Utils::HPRIM::parseHprimRawSource(content);

    // Clear view
    d->ui->hprimPatientName->clear();
    d->ui->hprimPatientDob->clear();
    d->ui->hprimPatientNss->clear();
    d->ui->hprimDateOfExam->clear();
    d->ui->patientSearch->clear();
    d->ui->dbPatientName->setText(tr("Select a patient"));
    d->ui->dbDob->clear();
    d->ui->contentViewer->clear();
    d->ui->integrate->setEnabled(false);

    if (msg.isValid() && !msg.isNull()) {
        // Populate HPRIM group
        d->ui->hprimPatientName->setText(QString("%1 %2").arg(msg.header().patientName()).arg(msg.header().patientFirstName()));
        d->ui->hprimPatientDob->setText(QLocale().toString(msg.header().patientDateOfBirth()));
        d->ui->hprimPatientNss->setText(msg.header().data(Utils::HPRIM::HprimHeader::PatientSocialNumber));
        d->ui->hprimDateOfExam->setText(QLocale().toString(msg.header().dateOfExamination()));

        // Populate the viewer with the HPRIM content
        d->ui->contentViewer->appendHtml(msg.toBasicHtml());
        QTextCursor cursor = d->ui->contentViewer->textCursor();
        cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
        d->ui->contentViewer->setTextCursor(cursor);

        // Try to find the correct patient uuid
        Patients::PatientModel *model = new Patients::PatientModel(this);
        model->setFilter(msg.header().patientName(), msg.header().patientFirstName());
        if (model->rowCount() == 1) {
            d->ui->patientSearch->setText(model->data(model->index(0, Core::IPatient::FullName)).toString());
            d->ui->dbPatientName->setToolTip(model->data(model->index(0, Core::IPatient::Uid)).toString());
            d->ui->dbPatientName->setText(d->ui->patientSearch->text());
            d->ui->dbDob->setText(QLocale().toString(model->data(model->index(0, Core::IPatient::DateOfBirth)).toDate(), QLocale::LongFormat));
            d->ui->integrate->setEnabled(true);
        } else {
            if (model->rowCount() == 0) {
                d->ui->dbPatientName->setToolTip(tr("No patient found with the following names: %1; %2")
                                                 .arg(msg.header().patientName())
                                                 .arg(msg.header().patientFirstName()));
            } else {
                d->ui->dbPatientName->setToolTip(tr("There are multiple patient with the same names: %1 %2. You will have to select the correct one.")
                                                 .arg(msg.header().patientName())
                                                 .arg(msg.header().patientFirstName()));
            }
        }
    } else {
        // Populate the viewer with the raw file content
        d->ui->contentViewer->setPlainText(content);
        d->ui->dbPatientName->setToolTip(tr("No correct HPRIM header found: %1; %2")
                                         .arg(msg.header().patientName())
                                         .arg(msg.header().patientFirstName()));
    }
}

/** Slot called when a patient is selected in the patient search box. */
void HprimIntegratorWidget::onPatientSelected(const QString &fullName, const QString &uid)
{
    if (!fullName.isEmpty() && !uid.isEmpty()) {
        Patients::PatientModel *model = new Patients::PatientModel(this);
        model->setFilter("", "", uid, Patients::PatientModel::FilterOnUuid);
        if (model->rowCount() == 1) {
            d->ui->dbPatientName->setText(model->data(model->index(0, Core::IPatient::FullName)).toString());
            d->ui->dbPatientName->setToolTip(uid);
            d->ui->dbDob->setText(QLocale().toString(model->data(model->index(0, Core::IPatient::DateOfBirth)).toDate(), QLocale::LongFormat));
        }
        d->ui->integrate->setEnabled(true);
    } else {
        d->ui->dbPatientName->setText(tr("Select a patient"));
        d->ui->dbPatientName->setToolTip(tr("No patient selected"));
        d->ui->integrate->setEnabled(false);
    }
}

/** Integrate an HPRIM message into a patient file */
void HprimIntegratorWidget::onDataIntegrationRequested()
{
    // Get the form item
    Form::FormItem *item = d->getSelectedFormItem();
    if (!item) {
        // ERROR
        Utils::warningMessageBox(tr("Unable to find the form item for importation"),
                                 tr("Unable to find the form item for importation"));
        return;
    }

    // Populate the item & check
    QString sha1 = d->populateFormItem(item);
    if (sha1.isEmpty()) {
        // ERROR
        Utils::warningMessageBox(tr("Error during importation"),
                                 tr("Error during importation (content is empty)"));
        return;
    }

    // Du to Qt HTML interpretation, we can not be 100% sure of
    // the episode exact content matching. So if check fails, the dialog
    // is Accepted.
    if (!d->checkFormItemContent(sha1, item)) {
        // ERROR
        Utils::warningMessageBox(tr("Error during importation"),
                                 tr("Error during importation (wrong checking)"));
        return;
    }

    // Manage the imported hprim file
    QModelIndex index = d->ui->dirContentTableView->currentIndex();
    QString sourceFileName = d->_fileModel->fileInfo(index).absoluteFilePath();
    QString fileMsg;
    switch (settings()->value(Constants::S_FILE_MANAGEMENT).toInt()) {
    case Constants::RemoveFileDefinitively:
        if (!QFile(sourceFileName).remove())
            fileMsg = tkTr(Trans::Constants::FILE_1_CANNOT_BE_REMOVED).arg(sourceFileName);
        else
            fileMsg = tkTr(Trans::Constants::FILE_1_CORRECTLY_REMOVED).arg(sourceFileName);
        break;
    case Constants::RemoveFileOneMonthAfterIntegration:
        // TODO: manage this
        break;
    case Constants::StoreFileInPath:
    {
        QFileInfo info(sourceFileName);
        QString newFileName = QString("%1/%2").arg(settings()->value(Constants::S_FILE_MANAGEMENT_STORING_PATH).toString()).arg(info.fileName());

        // Ensure that output dir exists
        QDir dir(QFileInfo(newFileName).absolutePath());
        if (!dir.exists()) {
            if (!QDir().mkpath(dir.absolutePath())) {
                LOG_ERROR(tkTr(Trans::Constants::PATH_1_CANNOT_BE_CREATED).arg(dir.absolutePath()));
                break;
            }
        }

        // Move source file
        if (!QFile(sourceFileName).copy(newFileName)) {
            fileMsg += tkTr(Trans::Constants::FILE_1_CAN_NOT_BE_COPIED)
                    .arg(sourceFileName);
        } else if (!QFile(sourceFileName).remove()) {
            fileMsg += tkTr(Trans::Constants::FILE_1_CANNOT_BE_REMOVED).arg(sourceFileName);
        } else {
            fileMsg += tkTr(Trans::Constants::FILE_1_MOVE_TO_2)
                    .arg(sourceFileName)
                    .arg(newFileName);
        }
        break;
    }
    }

    // Show a widget with the importation success message
    Utils::informativeMessageBox(tr("Importation was successful"),
                                 tr("Importation was successful. \n%1")
                                 .arg(fileMsg));
}

/** Update view & models when settings are modified */
void HprimIntegratorWidget::refreshSettings()
{
    Q_ASSERT(d->_fileModel);
    if (!d->_fileModel)
        return;
    d->_fileModel->setRootPath(settings()->value(Constants::S_PATH_TO_SCAN).toString());
    d->ui->dirContentTableView->setRootIndex(d->_fileModel->fileRootPath());
}


/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////     HprimIntegratorMode     ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
HprimIntegratorMode::HprimIntegratorMode(QObject *parent) :
    Core::IMode(parent),
    _widget(0)
{
    setEnabled(true);
    setDisplayName(tr("HPRIM"));
    setIcon(theme()->icon(Core::Constants::ICONHPRIM));
    setPriority(Core::Constants::P_MODE_TOOLS);
    setId("HprimIntegratorMode");
    setType("HprimIntegratorMode");
    setPatientBarVisibility(false);
    setWidget(_widget = new HprimIntegratorWidget);
}

HprimIntegratorMode::~HprimIntegratorMode()
{
    // Has did not parent the widget -> delete it by hand
    delete widget();
    setWidget(0);
}

void HprimIntegratorMode::refreshSettings()
{
    _widget->refreshSettings();
}
