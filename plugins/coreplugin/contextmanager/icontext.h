/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef CORE_ICONTEXT_H
#define CORE_ICONTEXT_H

// Adapted from QtCreator 2.5.2

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QWidget>
#include <QPointer>

/**
 * \file icontext.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 20 Sept 2012
*/

namespace Core {

class CORE_EXPORT Context
{
public:
    Context() {}

    explicit Context(const char *c1) { add(c1); }
    Context(const char *c1, const char *c2) { add(c1); add(c2); }
    Context(const char *c1, const char *c2, const char *c3) { add(c1); add(c2); add(c3); }
    Context(const char *base, int offset);
    void add(const char *c);
    bool contains(const char *c) const;
    bool contains(int c) const { return d.contains(c); }
    int size() const { return d.size(); }
    bool isEmpty() const { return d.isEmpty(); }
    int at(int i) const { return d.at(i); }

    // FIXME: Make interface slimmer.
    typedef QList<int>::const_iterator const_iterator;
    const_iterator begin() const { return d.begin(); }
    const_iterator end() const { return d.end(); }
    int indexOf(int c) const { return d.indexOf(c); }
    void removeAt(int i) { d.removeAt(i); }
    void prepend(int c) { d.prepend(c); }
    void add(const Context &c) { d += c.d; }
    void add(int c) { d.append(c); }
    bool operator==(const Context &c) const { return d == c.d; }

private:
    QList<int> d;
};

class CORE_EXPORT IContext : public QObject
{
public:
    IContext(QObject *parent = 0) : QObject(parent), m_widget(0) {}
    virtual ~IContext() {}

    virtual Context context() const { return m_context; }
    virtual QWidget *widget() const { return m_widget; }
    virtual QString contextHelpId() const { return m_contextHelpId; }

    virtual void setContext(const Context &context) { m_context = context; }
    virtual void setWidget(QWidget *widget) { m_widget = widget; }
    virtual void setContextHelpId(const QString &id) { m_contextHelpId = id; }

protected:
    Context m_context;
    QPointer<QWidget> m_widget;
    QString m_contextHelpId;
};

}  // End Core

#endif // CORE_ICONTEXT_H
