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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SUBFORMINSERTIONPOINT_H
#define SUBFORMINSERTIONPOINT_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <QString>

/**
 * \file subforminsertionpoint.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 01 Oct 2012
*/

namespace Form {
class FormMain;
// TODO: should be internal and not exported

class FORM_EXPORT SubFormInsertionPoint
{
public:
    SubFormInsertionPoint(const QString &receiverUid, const QString &addUid) :
            m_ReceiverFormUid(receiverUid),
            m_SubFormUid(addUid),
            m_AppendToForm(false),
            m_AddAsChild(true),
            m_EmitInsertionSignal(false)
    {}

    SubFormInsertionPoint() :
            m_AppendToForm(false),
            m_AddAsChild(true),
            m_EmitInsertionSignal(false)
    {}

    ~SubFormInsertionPoint() {}

    void setReceiverUid(const QString &uid) {m_ReceiverFormUid = uid;}
    void setSubFormUid(const QString &uid) {m_SubFormUid = uid;}
    void setEmptyRootSubForm(Form::FormMain *emptyRootSubForm) {m_emptyRootSubForm = emptyRootSubForm;}
    void setAppendToForm(bool append) {m_AppendToForm = append;}
    void setAddAsChild(bool asChild) {m_AddAsChild = asChild;}
    void setEmitInsertionSignal(bool emitSignal) {m_EmitInsertionSignal = emitSignal;}

    Form::FormMain *emptyRootSubForm() const {return m_emptyRootSubForm;}
    QString subFormUid() const {return m_SubFormUid;}
    QString receiverUid() const {return m_ReceiverFormUid;}
    bool appendToForm() const {return m_AppendToForm;}
    bool addAsChild() const {return m_AddAsChild;}
    bool emitInsertionSignal() const {return m_EmitInsertionSignal;}

private:
    QString m_ReceiverFormUid, m_SubFormUid;
    Form::FormMain *m_emptyRootSubForm;
    bool m_AppendToForm;
    bool m_AddAsChild;
    bool m_EmitInsertionSignal;
};

}  // end namespace Form

QDebug operator<<(QDebug dbg, const Form::SubFormInsertionPoint &c);
QDebug operator<<(QDebug dbg, const Form::SubFormInsertionPoint *c);

#endif // SUBFORMINSERTIONPOINT_H
