#include <QFile>
#include <QTextStream>
#include <QScriptEngine>

#include "pregnancy_importer.h"

namespace PregnancyImporter {
bool load(const QString &fileName, QList<PregnancyRecord> &records, QString *errorMsg) {
    QFile scriptFile(fileName);
    if (!scriptFile.open(QIODevice::ReadOnly)) {
        if (errorMsg)
            *errorMsg = QString("Error while opening %1").arg(fileName);
        return false;
    }
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();
    QScriptEngine myEngine;
    QScriptValue fun = myEngine.evaluate("(" + contents + ")", fileName);

    if (!fun.isValid() || !fun.isFunction()) {
        if (errorMsg)
            *errorMsg = QString("Error while evaluating the function inside %1").arg(fileName);
        return false;
    }

    QScriptValue obj = fun.call();

    if (!obj.isValid() || !obj.isObject()) {
        if (errorMsg)
            *errorMsg = QString("Error: the function does not return a valid object");
        return false;
    }

    records.clear();
    QVariantMap drugsMap = obj.toVariant().toMap();

    foreach (const QString &key, drugsMap.keys()) {
        QVariant val = drugsMap[key];
        if (val.toList().empty())
            continue;
        QVariantMap rMap = val.toList()[0].toMap();
        PregnancyRecord rec;
        rec.insert(Field_Name, key);
        rec.insert(Field_Category, rMap["Category"].toString());
        rec.insert(Field_Safety, rMap["Safety"].toString());
        rec.insert(Field_Class1, rMap["Class1"].toString());
        rec.insert(Field_Class2, rMap["Class2"].toString());
        rec.insert(Field_Class3, rMap["Class3"].toString());
        records << rec;
    }

    return true;
}
}

