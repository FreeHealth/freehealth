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
            m_AddAsChild(true)
    {}

    SubFormInsertionPoint() :
            m_AppendToForm(false),
            m_AddAsChild(true)
    {}

    ~SubFormInsertionPoint() {}

    void setReceiverUid(const QString &uid) {m_ReceiverFormUid = uid;}
    void setSubFormUid(const QString &uid) {m_AddFormUid = uid;}
    void setAppendToForm(bool append) {m_AppendToForm = append;}
    void setAddAsChild(bool asChild) {m_AddAsChild = asChild;}

    QString subFormUid() const {return m_AddFormUid;}
    QString receiverUid() const {return m_ReceiverFormUid;}
    bool appendToForm() const {return m_AppendToForm;}
    bool addAsChild() const {return m_AddAsChild;}

private:
    QString m_ReceiverFormUid, m_AddFormUid;
    bool m_AppendToForm;
    bool m_AddAsChild;
};

}  // end namespace Form

#endif // SUBFORMINSERTIONPOINT_H
