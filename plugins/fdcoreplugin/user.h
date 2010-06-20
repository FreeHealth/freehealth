#ifndef USER_H
#define USER_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/iuser.h>

class CORE_EXPORT User : public Core::IUser
{
    Q_OBJECT
public:
    User(QObject *parent);
    ~User();

    void clear();
    bool has(const int ref) const;

    QVariant value(const int ref) const;
    bool setValue(const int ref, const QVariant &value);

    QString toXml() const ;
    bool fromXml(const QString &xml);

};

#endif // USER_H
