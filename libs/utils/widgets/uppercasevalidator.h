#ifndef UPPERCASEVALIDATOR_H
#define UPPERCASEVALIDATOR_H

#include <utils/global_exporter.h>
#include <QValidator>
#include <QString>

/**
 * \file uppercasevalidator.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 22 Jan 2012
*/

/**
  \class Utils::UpperCaseValidator
  Use this validator in QLineEdit when you want the user input to always be uppercase.
*/

namespace Utils {

class UTILS_EXPORT UpperCaseValidator : public QValidator
{
public:
    UpperCaseValidator(QObject *parent) : QValidator(parent) {}  // nothing to do in constructor

    QValidator::State validate(QString &text, int &pos) const  // PS: no UpperCase for the first letter of variables, only for class names
    {
        Q_UNUSED(pos); // pos is not needed
        text = text.toUpper();  // Uppercase the text
        return QValidator::Acceptable;  // return "ok text is like we want it to be"
    }

};

}

#endif // UPPERCASEVALIDATOR_H
