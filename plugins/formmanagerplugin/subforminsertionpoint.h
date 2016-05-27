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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
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
namespace Internal {
class EpisodeBase;
}
class FormMain;
// TODO: should be internal and not exported

class FORM_EXPORT SubFormPoint
{
public:
    SubFormPoint(const QString &modeReceiverUid, const QString &formReceiverUid, const QString &addUid) :
        m_ModeUid(modeReceiverUid),
        m_ReceiverFormUid(formReceiverUid),
        m_SubFormUid(addUid),
        m_AllPatients(false)
    {}
    SubFormPoint() : m_AllPatients(false) {}
    virtual ~SubFormPoint() {}

    bool isValid() const {return (!m_SubFormUid.isEmpty() && !m_ReceiverFormUid.isEmpty());}

    void setReceiverUid(const QString &uid);
    void setModeUid(const QString &uid) {m_ModeUid = uid;}
    void setSubFormUid(const QString &uid) {m_SubFormUid = uid;}
    void setForAllPatient(bool allPatients) {m_AllPatients = allPatients;}

    const QString &subFormUid() const {return m_SubFormUid;}
    const QString &receiverUid() const {return m_ReceiverFormUid;}
    const QString &modeUid() const {return m_ModeUid;}
    bool isForAllPatients() const {return m_AllPatients;}

private:
    QString m_ModeUid, m_ReceiverFormUid, m_SubFormUid;
    bool m_AllPatients;
};

class FORM_EXPORT SubFormInsertionPoint : public SubFormPoint
{
    friend class Form::Internal::EpisodeBase;

public:
    SubFormInsertionPoint(const QString &modeReceiverUid, const QString &formReceiverUid, const QString &addUid) :
        SubFormPoint(modeReceiverUid, formReceiverUid, addUid),
        m_AppendToForm(false),
        m_AddAsChild(true),
        m_EmitInsertionSignal(false)
    {
    }

    SubFormInsertionPoint(const QString &receiverUid, const QString &addUid) :
        SubFormPoint(),
        m_AppendToForm(false),
        m_AddAsChild(true),
        m_EmitInsertionSignal(false)
    {
        setReceiverUid(receiverUid);
        setSubFormUid(addUid);
    }

    SubFormInsertionPoint() :
        SubFormPoint(),
        m_AppendToForm(false),
        m_AddAsChild(true),
        m_EmitInsertionSignal(false)
    {}

    ~SubFormInsertionPoint() {}

    void setEmptyRootSubForm(Form::FormMain *emptyRootSubForm) {m_emptyRootSubForm = emptyRootSubForm;}
    void setAppendToForm(bool append) {m_AppendToForm = append;}
    void setAddAsChild(bool asChild) {m_AddAsChild = asChild;}
    void setEmitInsertionSignal(bool emitSignal) {m_EmitInsertionSignal = emitSignal;}

    Form::FormMain *emptyRootSubForm() const {return m_emptyRootSubForm;}
    bool appendToForm() const {return m_AppendToForm;}
    bool addAsChild() const {return m_AddAsChild;}
    bool emitInsertionSignal() const {return m_EmitInsertionSignal;}

protected:
    const QString &receiverUidForDatabase() const;

private:
    mutable QString m_DbReceiver;
    Form::FormMain *m_emptyRootSubForm;
    bool m_AppendToForm;
    bool m_AddAsChild;
    bool m_EmitInsertionSignal;
};

class FORM_EXPORT SubFormRemoval : public SubFormPoint
{
    friend class Form::Internal::EpisodeBase;
public:
    SubFormRemoval(const QString &modeReceiverUid, const QString &formReceiverUid, const QString &addUid) :
        SubFormPoint(modeReceiverUid, formReceiverUid, addUid)
    {
    }
    ~SubFormRemoval() {}
};

}  // end namespace Form

QDebug operator<<(QDebug dbg, const Form::SubFormInsertionPoint &c);
QDebug operator<<(QDebug dbg, const Form::SubFormInsertionPoint *c);

#endif // SUBFORMINSERTIONPOINT_H
