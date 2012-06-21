#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H

#include "utils/global_exporter.h"
#include <QValidator>
#include <QStringList>

namespace Utils {

class UTILS_EXPORT DateValidator : public QValidator
{
    Q_OBJECT
public:
    explicit DateValidator(QObject *parent = 0);
    State validate(QString &input, int &pos) const;
    void fixup(QString &input) const;
//    void setDateFormats(QString& formats = "");

    QString matchedFormat(QString &input) const;

private:
    QStringList m_dateFormatList;
    QString m_lastValidFormat;

signals:
    
public slots:
    
};
} // end Utils
#endif // DATEVALIDATOR_H
