#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <utils/global_exporter.h>

#include <QString>
#include <QPair>
#include <QDate>


namespace Utils {
namespace Internal {
class RandomizerPrivate;
}

class UTILS_EXPORT Randomizer
{
public:
    Randomizer();
    ~Randomizer();

    void setPathToFiles(const QString &path);

    QString getRandomString(int length);

    QString getRandomName();
    QString getRandomSurname(bool male);

    QPair<int, QString> getRandomFrenchCity();

    int randomInt(int max);

    QDate randomDate(int minYear);

private:
    Internal::RandomizerPrivate *d;
};


} // End namespace Utils

#endif // RANDOMIZER_H
