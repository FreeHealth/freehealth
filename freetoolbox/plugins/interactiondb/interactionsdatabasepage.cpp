/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@free.fr>               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "interactionsdatabasepage.h"
#include "afssapsintegrator.h"
#include "interactionstep.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpdownloader.h>
#include <translationutils/constanttranslations.h>
#include <translationutils/googletranslator.h>

#include <extensionsystem/pluginmanager.h>

#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <QFile>
#include <QApplication>
#include <QDir>
#include <QProgressDialog>

#include "ui_interactiondatabasebuilder.h"
#include "ui_interactiondatabasecreator.h"
#include "ui_interactiondatabasechecker.h"

using namespace IAMDb;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()         {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/Interactions/") + QDir::separator();}
static inline QString iamDatabaseAbsPath()  {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::IAM_DATABASE_FILENAME);}
static inline QString iamDatabaseSqlSchema() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::FILE_IAM_DATABASE_SCHEME);}

static inline QString translationsCorrectionsFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::INTERACTIONS_ENGLISHCORRECTIONS_FILENAME);}
static inline QString afssapsIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString atcCsvFile()          {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::ATC_FILENAME);}


static QString typeToString(const QString &type)
{
    QStringList tmp;
    if (type.contains("C")) {
        tmp << tkTr(Trans::Constants::CONTRAINDICATION);
    }
    if (type.contains("D")) {
        tmp << tkTr(Trans::Constants::DISCOURAGED);
    }
    if (type.contains("T")) {
        tmp << tkTr(Trans::Constants::TAKE_INTO_ACCOUNT);
    }
    if (type.contains("P")) {
        tmp << tkTr(Trans::Constants::PRECAUTION_FOR_USE);
    }
    if (type.contains("I")) {
        tmp << tkTr(Trans::Constants::INFORMATION);
    }

    return tmp.join( ", " );
}

static void levelToListWidget(QListWidget *listWidget, const QString &type)
{
    listWidget->clear();
    QListWidgetItem *item;
    item = new QListWidgetItem(tkTr(Trans::Constants::CONTRAINDICATION), listWidget);
    if (type.contains("C")) {
        item->setCheckState(Qt::Checked);
    } else {
        item->setCheckState(Qt::Unchecked);
    }
    item = new QListWidgetItem(tkTr(Trans::Constants::DISCOURAGED), listWidget);
    if (type.contains("D")) {
        item->setCheckState(Qt::Checked);
    } else {
        item->setCheckState(Qt::Unchecked);
    }
    item = new QListWidgetItem(tkTr(Trans::Constants::TAKE_INTO_ACCOUNT), listWidget);
    if (type.contains("T")) {
        item->setCheckState(Qt::Checked);
    } else {
        item->setCheckState(Qt::Unchecked);
    }
    item = new QListWidgetItem(tkTr(Trans::Constants::PRECAUTION_FOR_USE), listWidget);
    if (type.contains("P")) {
        item->setCheckState(Qt::Checked);
    } else {
        item->setCheckState(Qt::Unchecked);
    }
    item = new QListWidgetItem(tkTr(Trans::Constants::INFORMATION), listWidget);
    if (type.contains("I")) {
        item->setCheckState(Qt::Checked);
    } else {
        item->setCheckState(Qt::Unchecked);
    }
}

static QIcon typeToIcon(const QString &type)
{
    if (type.contains("C"))
        return theme()->icon("critical.png");
    if (type.contains("D"))
        return theme()->icon("deconseille.png");
    if (type.contains("T"))
        return theme()->icon("warning.png");
    if (type.contains("P"))
        return theme()->icon("info.png");
    if (type.contains("I"))
        return theme()->icon("info.png");
    return QIcon();
}


QWidget *InteractionsDatabasePage::createPage(QWidget *parent)
{
    return new InteractionsDatabaseBuilder(parent);
}

QWidget *InteractionsDatabaseCreatorPage::createPage(QWidget *parent)
{
    return new InteractionDatabaseCreator(parent);
}

namespace IAMDb {

class InteractionsDatabaseBuilderPrivate
{
public:
    QPersistentModelIndex m_EditingIndex;
    bool m_ReviewModified;
};

} // namespace IAMDb

InteractionsDatabaseBuilder::InteractionsDatabaseBuilder(QWidget *parent) :
        QWidget(parent), ui(new Ui::InteractionDatabaseBuilder), d(new InteractionsDatabaseBuilderPrivate)
{
    ui->setupUi(this);
    ui->save->setIcon(theme()->icon(Core::Constants::ICONSAVE));
    ui->modify->setIcon(theme()->icon(Core::Constants::ICONEDIT));
    ui->makeCorrections->setEnabled(false);
//    ui->translate->setEnabled(false);

    InteractionModel *model = InteractionModel::instance();

    ui->treeView->setModel(model);
    ui->treeView->setWordWrap(true);
    ui->treeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeView->header()->setResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeView->header()->setResizeMode(2, QHeaderView::ResizeToContents);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactionActivated(QModelIndex)));
}

InteractionsDatabaseBuilder::~InteractionsDatabaseBuilder()
{
    delete ui; ui=0;
    delete d; d=0;
}

void InteractionsDatabaseBuilder::on_modify_clicked()
{
    ui->groupBox->setEnabled(!ui->groupBox->isEnabled());
    ui->groupBox_2->setEnabled(!ui->groupBox_2->isEnabled());
    ui->groupBox_4->setEnabled(!ui->groupBox_4->isEnabled());
    ui->groupReview->setEnabled(!ui->groupReview->isEnabled());
}

void InteractionsDatabaseBuilder::interactionActivated(const QModelIndex &index)
{
    if (d->m_EditingIndex==index)
        return;

    // save ?
    InteractionModel *model = InteractionModel::instance();
    /** \todo Save only if suficient rights */
    if (d->m_EditingIndex.isValid() &&
        (ui->risk->document()->isModified() ||
         ui->management->document()->isModified() ||
         ui->risk_en->document()->isModified() ||
         ui->management_en->document()->isModified()
         || (ui->checkBox->isChecked() != model->getReviewState(d->m_EditingIndex))
        )) {
        if (Utils::yesNoMessageBox(tr("Data changed but not saved."), tr("Do you want to save changes to the file ?")))
            on_save_clicked();
    }

    ui->groupBox->setEnabled(false);
    ui->groupBox_2->setEnabled(false);
    ui->groupBox_4->setEnabled(false);
    ui->groupReview->setEnabled(false);

    // clear
    ui->risk->clear();
    ui->management->clear();
    ui->risk_en->clear();
    ui->management_en->clear();

    if (model->tagName(index) != "Interactor")
        return;

    // set datas
    ui->risk->setPlainText(model->getRisk(index, "fr").replace("<br />", "\n"));
    ui->management->setPlainText(model->getManagement(index, "fr").replace("<br />", "\n"));
    ui->risk_en->setPlainText(model->getRisk(index, "en").replace("<br />", "\n"));
    ui->management_en->setPlainText(model->getManagement(index, "en").replace("<br />", "\n"));
    levelToListWidget(ui->listWidget, model->getLevel(index, "fr"));
    int reviewer = ui->comboBox->findText(model->getReviewer(index));
    if (reviewer==-1)
        ++reviewer;
    ui->comboBox->setCurrentIndex(reviewer);
    ui->checkBox->setChecked(model->getReviewState(index));
    d->m_EditingIndex = index;
}

void InteractionsDatabaseBuilder::on_save_clicked()
{
    InteractionModel *model = InteractionModel::instance();
    if (d->m_EditingIndex.isValid()) {
        if (ui->risk->document()->isModified())
            model->setRisk(d->m_EditingIndex, "fr", ui->risk->toPlainText().replace("\n", "<br />"));
        if (ui->management->document()->isModified())
            model->setManagement(d->m_EditingIndex, "fr", ui->management->toPlainText().replace("\n", "<br />"));
        if (ui->risk_en->document()->isModified())
            model->setRisk(d->m_EditingIndex, "en", ui->risk_en->toPlainText().replace("\n", "<br />"));
        if (ui->management_en->document()->isModified())
            model->setManagement(d->m_EditingIndex, "en", ui->management_en->toPlainText().replace("\n", "<br />"));
//        if (ui->level->isModified())
//            model->setLevel(d->m_EditingIndex, "fr", ui->level->text());
        model->setReviewer(d->m_EditingIndex, ui->comboBox->itemText(ui->comboBox->currentIndex()));
        model->setReviewState(d->m_EditingIndex, ui->checkBox->isChecked());
    }
    model->saveModel();

    ui->groupBox->setEnabled(false);
    ui->groupBox_2->setEnabled(false);
    ui->groupBox_4->setEnabled(false);
}

void InteractionsDatabaseBuilder::on_translate_clicked()
{
    InteractionModel *model = InteractionModel::instance();
    model->startTranslations();
}

void InteractionsDatabaseBuilder::on_makeCorrections_clicked()
{
    InteractionModel *model = InteractionModel::instance();
    model->correctTranslations();
}


namespace IAMDb {

class InteractionDatabaseCreatorPrivate
{
public:
    InteractionStep *m_Step;
};


} // namespace IAMDb


InteractionDatabaseCreator::InteractionDatabaseCreator(QWidget *parent) :
        QWidget(parent), ui(new Ui::InteractionDatabaseCreator), d(new InteractionDatabaseCreatorPrivate)
{
    ui->setupUi(this);
    d->m_Step = new InteractionStep(this);
    pluginManager()->addObject(d->m_Step);
    d->m_Step->createDir();
}

InteractionDatabaseCreator::~InteractionDatabaseCreator()
{
    pluginManager()->removeObject(d->m_Step);
    delete ui; ui=0;
    delete d->m_Step; d->m_Step=0;
    delete d; d=0;
}

void InteractionDatabaseCreator::on_createAndSave_clicked()
{
    d->m_Step->process();
    if (ui->biblioCheck->isChecked())
        d->m_Step->postProcessDownload();
}

void InteractionDatabaseCreator::on_checkDatas_clicked()
{
    // clear ui
    ui->treeWidget->clear();
    QTreeWidgetItem *item;

    // check afssaps molecules linking
    // check afssaps molecules links reviews
    AfssapsLinkerModel *molLinkModel = AfssapsLinkerModel::instance();
    while (molLinkModel->canFetchMore(QModelIndex()))
        molLinkModel->fetchMore(QModelIndex());
    item = new QTreeWidgetItem(ui->treeWidget);
    QTreeWidgetItem *unreviewed = new QTreeWidgetItem(item, QStringList() << tr("Unreviewed"));
    QTreeWidgetItem *unlinked = new QTreeWidgetItem(item, QStringList() << tr("Unlinked"));
    int unr = 0;
    int unl = 0;
    int nb = molLinkModel->rowCount();
    for(int i = 0; i < nb; ++i) {
        const QString &reviewed = molLinkModel->index(i, AfssapsLinkerModel::Review).data().toString();
        if (reviewed!="true") {
            new QTreeWidgetItem(unreviewed, QStringList() << tr("Unreviewed: %1").arg(molLinkModel->index(i, AfssapsLinkerModel::AfssapsName).data().toString()));
            ++unr;
        }
        const QString &links = molLinkModel->index(i, AfssapsLinkerModel::AtcCodes).data().toString();
        if (links.isEmpty()) {
            new QTreeWidgetItem(unlinked, QStringList() << tr("Unlinked: %1").arg(molLinkModel->index(i, AfssapsLinkerModel::AfssapsName).data().toString()));
            ++unl;
        }
    }
    item->setText(0, tr("AFSSAPS Molecules link (%1%) and review (%2%)")
                  .arg((double)(nb-unl)/(double)nb*100.0)
                  .arg((double)(nb-unr)/(double)nb*100.0));

    // check afssaps tree reviews
    AfssapsClassTreeModel *afssapsTreeModel = AfssapsClassTreeModel::instance();
    while (afssapsTreeModel->canFetchMore(QModelIndex()))
        afssapsTreeModel->fetchMore(QModelIndex());
    item = new QTreeWidgetItem(ui->treeWidget);
    unr = 0;
    nb = afssapsTreeModel->rowCount();
    for(int i = 0; i < afssapsTreeModel->rowCount(); ++i) {
        const QString &reviewed = afssapsTreeModel->index(i, AfssapsClassTreeModel::Review).data().toString();
        if (reviewed!="true") {
            new QTreeWidgetItem(item, QStringList() << tr("Unreviewed: %1").arg(afssapsTreeModel->index(i, AfssapsClassTreeModel::Name).data().toString()));
            ++unr;
        }
    }
    item->setText(0, tr("AFSSAPS Tree review (%1%)")
                  .arg((double)(nb-unr)/(double)nb*100.0));

    // check afssaps interactions reviews
    InteractionModel *interactionModel = InteractionModel::instance();
    while (interactionModel->canFetchMore(QModelIndex()))
        interactionModel->fetchMore(QModelIndex());
    item = new QTreeWidgetItem(ui->treeWidget);
    unr = 0;
    nb = interactionModel->rowCount();
    for(int i = 0; i < nb; ++i) {
        const QString &reviewed = interactionModel->index(i, InteractionModel::Review).data().toString();
        if (reviewed!="true") {
            new QTreeWidgetItem(item, QStringList() << tr("Unreviewed: %1").arg(interactionModel->index(i, InteractionModel::Name).data().toString()));
            ++unr;
        }
    }
    item->setText(0, tr("AFSSAPS Interaction review (%1%)")
                  .arg((double)(nb-unr)/(double)nb*100.0));

}


namespace IAMDb {

class InteractionDatabaseCheckerPrivate
{
public:
};

}  // End namespace IAMDb


InteractionDatabaseChecker::InteractionDatabaseChecker(QWidget *parent) :
        QWidget(parent), ui(new Ui::InteractionDatabaseChecker), d(new InteractionDatabaseCheckerPrivate)
{
    ui->setupUi(this);
}

InteractionDatabaseChecker::~InteractionDatabaseChecker()
{
    delete ui; ui=0;
    delete d; d=0;
}

void InteractionDatabaseChecker::on_check_clicked()
{
    QFont bold;
    bold.setBold(true);
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
    item->setFont(0, bold);
    if (!Core::Tools::connectDatabase(Core::Constants::IAM_DATABASE_NAME, iamDatabaseAbsPath())) {
        item->setText(0, tr("Database can not be connected"));
        return;
    } else {
        item->setText(0, tr("Database connected"));
    }
    /** \todo code here */
    QSqlDatabase iam = QSqlDatabase::database(Core::Constants::IAM_DATABASE_NAME);
}




/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////    MODEL    //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
struct ToTranslate {
    QString interactor, risk_fr, management_fr;
    QString risk_en, management_en;
};

class DomItem
{
public:
    DomItem(QDomNode &node, int row, DomItem *parent = 0)
    {
        domNode = node;
        // Record the item's location within its parent.
        rowNumber = row;
        parentItem = parent;
    }

    ~DomItem()
    {
        QHash<int,DomItem*>::iterator it;
        for (it = childItems.begin(); it != childItems.end(); ++it)
            delete it.value();
    }

    QDomNode node() const
    {
        return domNode;
    }

    DomItem *parent()
    {
        return parentItem;
    }

    void refreshChildren()
    {
        childItems.clear();
    }

    DomItem *child(int i)
    {
        if (childItems.contains(i))
            return childItems[i];

        if (i >= 0 && i < domNode.childNodes().count()) {
            QDomNode childNode = domNode.childNodes().item(i);
            DomItem *childItem = new DomItem(childNode, i, this);
            childItems[i] = childItem;
            return childItem;
        }
        return 0;
    }

    int row()
    {
        return rowNumber;
    }

private:
    QDomNode domNode;
    QHash<int,DomItem*> childItems;
    DomItem *parentItem;
    int rowNumber;
};


namespace IAMDb {

class InteractionModelPrivate
{
public:
    InteractionModelPrivate(InteractionModel *parent) :
            m_RootItem(0), m_FetchedRows(0), m_NumberOfTranslationsNeeded(0), q(parent)
    {
        QFile file(afssapsIamXmlFile());
        if (file.open(QIODevice::ReadOnly)) {
            QString error;
            int line, col;
            if (!domDocument.setContent(&file, &error,&line,&col)) {
                Utils::Log::addError(q, QApplication::translate("InteractionModel","Can not read XML file content %1").arg(file.fileName()), __FILE__, __LINE__);
                Utils::Log::addError(q, QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error), __FILE__, __LINE__);
            } else {
                Utils::Log::addMessage(q, QApplication::translate("InteractionModel","Reading file: %1").arg(file.fileName()));
            }
            file.close();
        } else {
            Utils::Log::addError(q, QApplication::translate("InteractionModel","Can not open XML file %1").arg(file.fileName()), __FILE__, __LINE__);
        }

        m_RootNode = domDocument.firstChildElement("InteractionModel");

        m_RootItem = new DomItem(m_RootNode, 0);
    }

    ~InteractionModelPrivate()
    {
        qDeleteAll(m_Main_ToTranslate);
        m_Main_ToTranslate.clear();
    }

    void getUntranslated()
    {
        // Get all untranslated risks
        QDomElement main = m_RootNode.firstChildElement("MainInteractor");
        int nb = 0;
        while (!main.isNull()) {
            QDomElement interactor = main.firstChildElement("Interactor");
            while (!interactor.isNull()) {
                ToTranslate *t = new ToTranslate;
                t->interactor = interactor.attribute("name");

                // get risk
                QDomElement risk = interactor.firstChildElement("Risk");
                QDomElement toTranslate;
                bool languageFound = false;
                while (!risk.isNull()) {
                    if (risk.attribute("lang")=="fr" && !risk.attribute("text").isEmpty()) {
                        // to be translated
                        toTranslate = risk;
                    }
                    if (risk.attribute("lang")=="en") {
                        languageFound = true;
                        if (risk.attribute("text").isEmpty() && !toTranslate.isNull()) {
                            // TRANSLATE toTranslate.attribute("text")
                            t->risk_fr = toTranslate.attribute("text");
                        }
                    }
                    risk = risk.nextSiblingElement("Risk");
                }
                if (!languageFound && !toTranslate.isNull()) {
                    // TRANSLATE toTranslate.attribute("text")
                    t->risk_fr = toTranslate.attribute("text");
                }

                // get management
                QDomElement management = interactor.firstChildElement("Management");
                toTranslate = QDomElement();
                languageFound = false;
                while (!management.isNull()) {
                    if (management.attribute("lang")=="fr" && !management.attribute("text").isEmpty()) {
                        // to be translated
                        toTranslate = management;
                    }
                    if (management.attribute("lang")=="en") {
                        languageFound = true;
                        if (management.attribute("text").isEmpty() && !toTranslate.isNull()) {
                            // TRANSLATE toTranslate.attribute("text")
                            t->management_fr = toTranslate.attribute("text");
                        }
                    }
                    management = management.nextSiblingElement("Management");
                }
                if (!languageFound && !toTranslate.isNull()) {
                    // TRANSLATE toTranslate.attribute("text")
                    t->management_fr = toTranslate.attribute("text");
                }

                // Add to untr datas ?
                if (!t->risk_fr.isEmpty() || !t->management_fr.isEmpty()) {
                    m_Main_ToTranslate.insertMulti(main.attribute("name"), t);
                    ++nb;
                } else {
                    delete t;
                }
                interactor = interactor.nextSiblingElement("Interactor");
            }
            main = main.nextSiblingElement("MainInteractor");
        }
        m_NumberOfTranslationsNeeded = nb;
        Utils::Log::addMessage("InteractionModel", QString("Found %1 translations undone").arg(nb));
    }

public:
    QDomDocument domDocument;
    QDomNode m_RootNode;
    DomItem *m_RootItem;
    QString reviewer, actualDbUid;
    int m_FetchedRows;
    // for google translations
    QString m_From, m_ToLang;
    Utils::GoogleTranslator *google;
    QMultiHash<QString, ToTranslate *> m_Main_ToTranslate;
    int m_NumberOfTranslationsNeeded;
    int m_TranslatingDataId;


private:
    InteractionModel *q;
};

}  // End namespace IAMDb



InteractionModel *InteractionModel::m_Instance = 0;
InteractionModel *InteractionModel::instance(QObject *parent)
{
    if (!m_Instance)
        m_Instance = new InteractionModel(parent);
    return m_Instance;
}

InteractionModel::InteractionModel(QObject *parent) :
        QAbstractTableModel(parent), d(0)
{
    setObjectName("InteractionModel");
    d = new InteractionModelPrivate(this);
}

InteractionModel::~InteractionModel()
{
    if (d)
        delete d;
    d = 0;
}

bool InteractionModel::saveModel()
{
    QFile file(afssapsIamXmlFile());
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        file.write(d->domDocument.toString(2).toUtf8());
        file.close();
        return true;
    }
    return false;
}

void InteractionModel::setActualReviewer(const QString &name)
{
    d->reviewer = name;
}

//bool InteractionModel::canFetchMore(const QModelIndex &parent) const
//{
//    int nbItemRows = 0;
//    DomItem *parentItem = 0;
//    if (!parent.isValid())
//        parentItem = d->m_RootItem;
//    else
//        parentItem = static_cast<DomItem*>(parent.internalPointer());

//    if (parentItem)
//        nbItemRows = parentItem->node().childNodes().count();

//    //    qWarning() << "canFetchMore" << (d->m_FetchedRows < nbItemRows) << parent << nbItemRows << d->m_FetchedRows;

//    return (d->m_FetchedRows < nbItemRows);
//}

//void InteractionModel::fetchMore(const QModelIndex &parent)
//{
//    int nbItemRows = 0;
//    DomItem *parentItem = 0;
//    if (!parent.isValid())
//        parentItem = d->m_RootItem;
//    else
//        parentItem = static_cast<DomItem*>(parent.internalPointer());

//    if (parentItem)
//        nbItemRows = parentItem->node().childNodes().count();

//    int remainder = nbItemRows - d->m_FetchedRows;
//    int itemsToFetch = qMin(MODEL_PREFETCH, remainder);

//    //    qWarning() << "InteractionModel::fetchMore" << parent << d->m_FetchedRows << itemsToFetch;

//    beginInsertRows(parent, d->m_FetchedRows, d->m_FetchedRows + itemsToFetch);
//    d->m_FetchedRows += itemsToFetch;
//    endInsertRows();
//}

QModelIndex InteractionModel::index(int row, int column, const QModelIndex &parent) const
{
//    qWarning() << "index" << row << column;
     if (!hasIndex(row, column, parent))
        return QModelIndex();

    DomItem *parentItem;

    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    DomItem *childItem = parentItem->child(row);
    if (childItem) {
        const QString &tag = childItem->node().toElement().tagName();
        if (tag == "MainInteractor" || tag == "MainInformation" || tag == "Interactor") {
            return createIndex(row, column, childItem);
        }
    }
    return QModelIndex();
}

QModelIndex InteractionModel::parent(const QModelIndex &child) const
{
//    qWarning() << "parent" << child;
    if (!child.isValid())
        return QModelIndex();

    DomItem *childItem = static_cast<DomItem*>(child.internalPointer());
    DomItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == d->m_RootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int InteractionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    DomItem *parentItem = 0;
    int nb = 0;
    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    if (parentItem)
        nb = parentItem->node().childNodes().count();

    return nb;
}

int InteractionModel::columnCount(const QModelIndex &index) const
{
//    DomItem *item = static_cast<DomItem*>(index.internalPointer());
//    if (!item)
//        return 3;
//    QDomNode node = item->node();
//    if (node.toElement().tagName()=="Interaction")
//        return 2;
//    else if (node.toElement().tagName()=="Risk" || node.toElement().tagName()=="Management")
//        return 3;
    return ColumnCount;
}

QVariant InteractionModel::data(const QModelIndex &index, int role) const
{
    //    <MainInteractor name="ACIDE ACETOHYDROXAMIQUE">
    //      <MainInformation lang="fr"></MainInformation>
    //      <Interactor name="FER" level="10"/>
    //        <Risk lang="fr" text="Diminution de l'absorption digestive de ces deux médicaments par chélation du fer."/>
    //        <Management lang="fr" text=""/>
    //        <Reviews>
    //          <Reviewer name="" uid="" dateofreview="" comment="" completed=""/>
    //          <Reviewer name="" uid="" dateofreview="" comment="" completed=""/>
    //        </Reviews>
    //        <References>
    //          <Reference comitter="" reviewer="" source="" web="" journal=""/>
    //          <Reference comitter="" reviewer="" source="" web="" journal=""/>
    //        </References>
    //      </Interactor>
    //    </MainInteractor>

    if (!index.isValid())
        return QVariant();

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    QDomNamedNodeMap attributeMap = node.attributes();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QString tag = node.toElement().tagName();
        if (tag == "MainInteractor" && index.column() == 0) {
            return attributeMap.namedItem("name").nodeValue();
        } else if (tag == "MainInformation" && index.column() == 0) {
            const QString &text = node.toElement().text();
            if (text.isEmpty())
                return tr("No information");
            return text;
        } else if (tag == "Interactor") {
            if (index.column()==0)
                return attributeMap.namedItem("name").nodeValue();
            else if (index.column()==1)
                return typeToString(attributeMap.namedItem("level").nodeValue());
        }
        return QVariant();
    } else if (role == Qt::CheckStateRole) {
        if (index.column()==Review) {
            if (attributeMap.namedItem("review").nodeValue() == "true")
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
    } else if (role == Qt::ToolTipRole) {
        QString tmp;
        //        tmp += attributeMap.namedItem("name").nodeValue();
        //        QStringList codes = attributeMap.namedItem("AtcCode").nodeValue().split(",");
        //        tmp += "\n  " + codes.join("\n  ");
        //        tmp += "\n  " + AtcModel::instance()->getAtcLabel(codes).join("\n  ");
        return tmp;
    } else if (role==Qt::ForegroundRole) {
        if (!attributeMap.namedItem("autoFound").nodeValue().isEmpty()) {
            return QColor("lightgray");
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column()==Level)
            return typeToIcon(attributeMap.namedItem("level").nodeValue());
        if (index.column()==Name) {
            if (node.toElement().tagName()=="Interactor") {
                if (attributeMap.namedItem("review").nodeValue()=="true") {
                    return theme()->icon(Core::Constants::ICONOK);
                }
            } else if (node.toElement().tagName()=="MainInteractor") {
                // check all children review state
                QDomElement e = node.firstChildElement("Interactor");
                while (!e.isNull()) {
                    if (e.attribute("review") != "true") {
                        return QVariant();
                    }
                    e = e.nextSiblingElement("Interactor");
                }
                return theme()->icon(Core::Constants::ICONOK);
            }

        }
    } else if (role == Qt::FontRole) {
        if (!index.parent().isValid()) {
            QFont bold;
            bold.setBold(true);
            return bold;
        }
    }
    return QVariant();
}

bool InteractionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    QDomNamedNodeMap attributeMap = node.attributes();

    if (role==Qt::EditRole) {
        switch (index.column()) {
        case References:
            attributeMap.namedItem("references").setNodeValue(value.toString());
            break;
        case Comments:
            attributeMap.namedItem("comment").setNodeValue(value.toString());
            break;
        case Review:
            attributeMap.namedItem("review").setNodeValue(value.toString());
            break;
        default:
            return false;
        }
        if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
            attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
        attributeMap.namedItem("dateofreview").setNodeValue(QDate::currentDate().toString(Qt::ISODate));
        Q_EMIT dataChanged(index, this->index(index.row(), columnCount()));
        return true;
    } else if (role==Qt::CheckStateRole && index.column()==Review) {
        if (value.toInt() == Qt::Checked) {
            attributeMap.namedItem("review").setNodeValue("true");
        } else {
            attributeMap.namedItem("review").setNodeValue("false");
        }
        if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
            attributeMap.namedItem("reviewer").setNodeValue(d->reviewer);
        attributeMap.namedItem("dateofreview").setNodeValue(QDate::currentDate().toString(Qt::ISODate));
        Q_EMIT dataChanged(index, this->index(index.row(), columnCount()));
        return true;
    }

    return false;
}

Qt::ItemFlags InteractionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

//    if (index.column()==AfssapsName)
//        f |= Qt::ItemIsDragEnabled  | Qt::ItemIsDropEnabled;

//    if (index.column() == AfssapsName || index.column() == AffapsCategory || index.column() == Date || index.column() == FancyButton)
//        return f;

    if (index.column() == Review)
        f |= Qt::ItemIsUserCheckable;

    f |= Qt::ItemIsEditable;

    return f;
}

QVariant InteractionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Review:
            return tr("Review state");
        case Reviewer:
            return tr("Reviewer");
        case References:
            return tr("References");
        case Comments:
            return tr("Comments");
        case Date:
            return tr("Date of review");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QString InteractionModel::tagName(const QModelIndex &index) const
{
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    return node.toElement().tagName();
}

QString InteractionModel::getRisk(const QModelIndex &index, const QString &lang)
{
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    if (node.toElement().tagName() == "Interactor") {
        QDomElement el = node.firstChildElement("Risk");
        while (!el.isNull()) {
            if (el.attribute("lang")==lang)
                return el.attribute("text");
            el = el.nextSiblingElement("Risk");
        }
    }
    return QString();
}

QString InteractionModel::getManagement(const QModelIndex &index, const QString &lang)
{
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    if (node.toElement().tagName() == "Interactor") {
        QDomElement el = node.firstChildElement("Management");
        while (!el.isNull()) {
            if (el.attribute("lang")==lang)
                return el.attribute("text");
            el = el.nextSiblingElement("Management");
        }
    }
    return QString();
}

QString InteractionModel::getLevel(const QModelIndex &index, const QString &lang)
{
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    if (node.toElement().tagName() == "Interactor") {
        return node.toElement().attribute("level");
    }
    return QString();
}

QString InteractionModel::getReviewer(const QModelIndex &index)
{
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    if (node.toElement().tagName() == "Interactor") {
        return node.toElement().attribute("reviewer");
    }
    return QString();
}

bool InteractionModel::getReviewState(const QModelIndex &index)
{
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    if (node.toElement().tagName() == "Interactor") {
        return (node.toElement().attribute("review", "false")=="true");
    }
    return false;
}

bool InteractionModel::setRisk(const QModelIndex &index, const QString &lang, const QString &value)
{
    if (!index.isValid())
        return false;
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    if (node.toElement().tagName() == "Interactor") {
        QDomElement el = node.firstChildElement("Risk");
        while (!el.isNull()) {
            if (el.attribute("lang")==lang) {
                el.setAttribute("text", value);
            }
            el = el.nextSiblingElement("Risk");
        }
        return true;
    }
    return true;
}

bool InteractionModel::setManagement(const QModelIndex &index, const QString &lang, const QString &value)
{
    if (!index.isValid())
        return false;
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    if (node.toElement().tagName() == "Interactor") {
        QDomElement el = node.firstChildElement("Management");
        while (!el.isNull()) {
            if (el.attribute("lang")==lang) {
                el.setAttribute("text", value);
            }
            el = el.nextSiblingElement("Management");
        }
        return true;
    }
    return true;
}

bool InteractionModel::setLevel(const QModelIndex &index, const QString &lang, const QString &value)
{
    if (!index.isValid())
        return false;
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    if (node.toElement().tagName() == "Interactor") {
        node.toElement().setAttribute("level", value);
        return true;
    }
    return true;
}

bool InteractionModel::setReviewer(const QModelIndex &index, const QString &reviewer)
{
    if (!index.isValid())
        return false;
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    if (node.toElement().tagName() == "Interactor") {
        node.toElement().setAttribute("reviewer", reviewer);
        return true;
    }
    return true;
}

bool InteractionModel::setReviewState(const QModelIndex &index, bool state)
{
    if (!index.isValid())
        return false;
    DomItem *item = static_cast<DomItem*>(index.internalPointer());
    QDomNode node = item->node();
    if (node.toElement().tagName() == "Interactor") {
        if (state)
            node.toElement().setAttribute("review", "true");
        else
            node.toElement().setAttribute("review", "false");
        return true;
    }
    return true;
}

void InteractionModel::startTranslations()
{
    d->m_From = "fr";
    d->m_ToLang = "en";
    d->google = new Utils::GoogleTranslator(this);
    connect(d->google, SIGNAL(translationComplete(QString)), this, SLOT(translationDone(QString)));
    connect(this, SIGNAL(needTranslationSaving()), this, SLOT(saveTranslations()));
    d->m_TranslatingDataId = 0;
    d->getUntranslated();
    translateNextData();
}

void InteractionModel::translateNextData()
{
    qWarning() << d->m_TranslatingDataId;
    // No more translations ?
    if (d->m_TranslatingDataId == d->m_NumberOfTranslationsNeeded) {
        saveTranslations();
        return;
    }

    ToTranslate *t = d->m_Main_ToTranslate.values().at(d->m_TranslatingDataId);
    QString trans = t->risk_fr + ".........." + t->management_fr;

    // skip empty translations
    if (!trans.isEmpty()) {
        qWarning() << "startingTranslationDone" << trans;
        d->google->startTranslation(d->m_From, d->m_ToLang, trans);
    } else {
        ++d->m_TranslatingDataId;
        translateNextData();
    }
}

void InteractionModel::translationDone(const QString &trans)
{
    qWarning() << "translationDone" << trans;
    if (trans.startsWith(" null")) {
        saveTranslations();
        return;
    }

    ToTranslate *t = d->m_Main_ToTranslate.values().at(d->m_TranslatingDataId);
    QString tmp = trans;
    tmp.replace("...........", ".||");
    tmp.replace("..........", "||");
    QStringList val = tmp.split("||");
    t->risk_en = val.at(0).simplified();
    if (val.count() == 2)
        t->management_en = val.at(1).simplified();
    ++d->m_TranslatingDataId;
    translateNextData();
}

void InteractionModel::correctTranslations()
{
    // read resource file
    QFile file(qApp->applicationDirPath() + Core::Constants::MACBUNDLE + Core::Constants::INTERACTIONS_ENGLISHCORRECTIONS_FILENAME);
    QString content;
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    content = QString::fromUtf8(file.readAll());
    QStringList lines = content.split("\n");
    QHash<QString, QString> corrections;
    foreach(const QString &line, lines) {
        if (line.startsWith("//"))
            continue;
        QStringList values = line.split(";");
        if (values.count() != 2)
            continue;
        corrections.insert(values.at(0), values.at(1));
    }
    qWarning() << "getting"<<corrections.count() << "corrections";

    // get english translations item by item
    QString html;
    for(int i = 0; i < rowCount(); ++i) {
        QModelIndex parent = index(i,0);

        for(int j = 0; j < rowCount(parent); ++j) {
            QModelIndex item = index(j,0,parent);
            DomItem *domItem = static_cast<DomItem*>(item.internalPointer());
            QDomNode node = domItem->node();

            if (node.toElement().tagName()=="Interactor") {
                QString risk = getRisk(item, "en");
                QString management = getManagement(item, "en");

                foreach(const QString &uncorrect, corrections.keys()) {
                    if (risk.contains(uncorrect)) {
                        risk.replace(uncorrect, corrections.value(uncorrect));
                    }
                    if (management.contains(uncorrect)) {
                        management.replace(uncorrect, corrections.value(uncorrect));
                    }
                }
                const QString &r = getRisk(item, "en");
                const QString &m = getManagement(item, "en");

                if (risk!=r) {
                    html += QString("<tr><td>%1</td><td>%2</td>")
                            .arg(r)
                            .arg(risk);
                } else {
                    html += "<tr><td></td><td></td>";
                }
                if (management!=m) {
                    html += QString("<td>%1</td><td>%2</td></tr>")
                            .arg(m)
                            .arg(management);
                } else {
                    html += "<td></td><td></td></tr>";
                }
                html += "\n";

            }
        }
    }
    html.prepend("<html><body><table border=1>");
    html.append("</table></body></html>");
    Utils::saveStringToFile(html, qApp->applicationDirPath() + Core::Constants::MACBUNDLE + "/../global_resources/sql/trans.html");
    saveModel();
}

void InteractionModel::saveTranslations()
{
    qWarning() << "saveTranslations" << d->m_TranslatingDataId;

    for(int i=0; i < d->m_TranslatingDataId; ++i) {
        ToTranslate *t = d->m_Main_ToTranslate.values().at(i);
        const QString &mainName = d->m_Main_ToTranslate.key(t);

        qWarning() << "main" << mainName;

        if (!t->risk_en.isEmpty() || !t->management_en.isEmpty()) {
            // find the related node
            QDomElement main = d->m_RootNode.firstChildElement("MainInteractor");
            while (!main.isNull()) {

                // Find the main interactor
                if (main.attribute("name") != mainName) {
                    main = main.nextSiblingElement("MainInteractor");
                    continue;
                }

                qWarning() << "found main";

                // Find the interactor
                QDomElement interactor = main.firstChildElement("Interactor");

                while (!interactor.isNull()) {
                    if (interactor.attribute("name") != t->interactor) {
                        interactor = interactor.nextSiblingElement("Interactor");
                        continue;
                    }
                    qWarning() << "Set data to XML";

                    // set risk
                    QDomElement risk = d->domDocument.createElement("Risk");
                    risk.setAttribute("lang", "en");
                    risk.setAttribute("text", t->risk_en);
                    interactor.appendChild(risk);

                    // set management
                    QDomElement management = d->domDocument.createElement("Management");
                    management.setAttribute("lang", "en");
                    management.setAttribute("text", t->management_en);
                    interactor.appendChild(management);

                    break;
                }
                break;
            }
        }
    }

    saveModel();
}
