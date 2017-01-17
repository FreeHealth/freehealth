/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#ifndef COMMANDMAPPINGS_H
#define COMMANDMAPPINGS_H

#include <coreplugin/dialogs/ioptionspage.h>

#include <QObject>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace Core {

class Command;

namespace Internal {

namespace Ui { class CommandMappings; }

class ActionManagerPrivate;
class MainWindow;

}

class CORE_EXPORT CommandMappings : public Core::IOptionsPage
{
    Q_OBJECT

public:
    CommandMappings(QObject *parent = 0);
    ~CommandMappings();

protected slots:
    void commandChanged(QTreeWidgetItem *current);
    void filterChanged(const QString &f);
    virtual void importAction() {}
    virtual void exportAction() {}
    virtual void defaultAction() = 0;

protected:
    // IOptionsPage
    QWidget *createPage(QWidget *parent);
    virtual void apply() {}
    virtual void finish();

    virtual void initialize() = 0;
    bool filter(const QString &f, const QTreeWidgetItem *item);

    // access to m_page
    void setImportExportEnabled(bool enabled);
    QTreeWidget *commandList() const;
    QLineEdit *targetEdit() const;
    QString filterText() const;
    void setPageTitle(const QString &s);
    void setTargetLabelText(const QString &s);
    void setTargetEditTitle(const QString &s);
    void setTargetHeader(const QString &s);
    void setModified(QTreeWidgetItem *item, bool modified);
    virtual void markPossibleCollisions(QTreeWidgetItem *) {}
    virtual void resetCollisionMarkers() {}
private:
    Internal::Ui::CommandMappings *m_page;
};

} // namespace Core

#endif // COMMANDMAPPINGS_H
