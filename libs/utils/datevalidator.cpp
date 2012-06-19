#include <translationutils/constants.h>
#include "datevalidator.h"
#include <QDate>
#include <QDebug>

using namespace Utils;

DateValidator::DateValidator(QObject *parent) :
    QValidator(parent)
{
    m_dateFormatList.clear();

    // split localized dateFormat string and put the parts a separated QStringList
    //: this is a comma separated list of formatStrings used by QDate::fromString();
    //:
    m_dateFormatList = tr("ddMMyy,ddMMyyyy").trimmed().split(",", QString::SkipEmptyParts);
    m_lastValidFormat = QString();

    // always use the default formats
    m_dateFormatList.append(QLocale().dateFormat(QLocale::ShortFormat));
    m_dateFormatList.append(QLocale().dateFormat(QLocale::NarrowFormat));

    // and then the FMF editor default format
    m_dateFormatList.append(Trans::Constants::DATEFORMAT_FOR_EDITOR);
}

/** \brief validates the input string with custom date formats
 *
 * The function checks if the input string matches a string
 * in the format list. This list that is set up with FMF
 * defaults, locale defaults, system locales and user defined settings.
 */
QValidator::State DateValidator::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);

    // check if input string can be converted into a date
    // using a dateFormat of the list
    foreach(QString format, m_dateFormatList) {
        if (QDate::fromString(input, format).isValid()) {
            qDebug() << "Date conversion succeded:" << input;
            return QValidator::Acceptable;
        }
    }

    // no match by now
    // check if the user enters digits or .-/
    // everything else is discouraged
    if(QRegExp("[-./ 0-9]*").exactMatch(input)) {
        qDebug() << "intermediate valid date format:" << input;
        return QValidator::Intermediate;
    }

    // if nothing of the above was possible, the input string is mess.
    qDebug() << "invalid date format:" << input;
    return QValidator::Invalid;
}

/** \brief returns formatString used in the last successful validation */
QString DateValidator::matchedFormat(QString & input) const
{
    foreach(QString format, m_dateFormatList) {
        if (QDate::fromString(input, format).isValid()) {
            return format;
        }
    }
    return QString();
}

void DateValidator::fixup(QString &input) const
{
    input = input.trimmed();
}
