#ifndef SUBFORMINSERTIONPOINT_H
#define SUBFORMINSERTIONPOINT_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <QString>

namespace Form {

class FORM_EXPORT SubFormInsertionPoint
{
public:
    SubFormInsertionPoint(const QString &receiverUid, const QString &addUid) :
            m_ReceiverFormUid(receiverUid),
            m_AddFormUid(addUid),
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
    void setSubFormUid(const QString &uid) {m_AddFormUid = uid;}
    void setAppendToForm(bool append) {m_AppendToForm = append;}
    void setAddAsChild(bool asChild) {m_AddAsChild = asChild;}
    void setEmitInsertionSignal(bool emitSignal) {m_EmitInsertionSignal = emitSignal;}

    QString subFormUid() const {return m_AddFormUid;}
    QString receiverUid() const {return m_ReceiverFormUid;}
    bool appendToForm() const {return m_AppendToForm;}
    bool addAsChild() const {return m_AddAsChild;}
    bool emitInsertionSignal() const {return m_EmitInsertionSignal;}

private:
    QString m_ReceiverFormUid, m_AddFormUid;
    bool m_AppendToForm;
    bool m_AddAsChild;
    bool m_EmitInsertionSignal;
};

}  // end namespace Form

#endif // SUBFORMINSERTIONPOINT_H
