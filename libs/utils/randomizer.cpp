#include "randomizer.h"
#include "global.h"
#include "log.h"

#include <QFile>
#include <QVector>


using namespace Utils;
using namespace std;

#define makeRand(max) (int)((double)rand() / ((double)RAND_MAX + 1) * max)

namespace Utils {
namespace Internal {
class RandomizerPrivate
{
public:
    RandomizerPrivate() {}

    ~RandomizerPrivate()
    {
        boysSurnames.clear();
        girlsSurnames.clear();
    }

    void readBoysSurnames()
    {
        if (!boysSurnames.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/boys_surnames.csv");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read boy surnames.");
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            boysSurnames.append(s);
        }
    }

    void readGirlsSurnames()
    {
        if (!girlsSurnames.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/girls_surnames.csv");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read boy surnames.");
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            girlsSurnames.append(s);
        }
    }

    void readZipCodes()
    {
        if (!zipCodes.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/zipcodes.csv");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read zip codes.");
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            QStringList z = s.split("\t");
            if (z.count() != 2)
                continue;
            zipCodes.insert(z.at(1).toInt(), z.at(0));
        }
    }

    void readFrenchWords()
    {
        if (!words.isEmpty())
            return;
        QString c = Utils::readTextFile(m_Path + "/listemotsfr.txt");
        if (c.isEmpty())
            Utils::Log::addError("Randomizer", "Can not read french words.");
        foreach(const QString &s, c.split("\n", QString::SkipEmptyParts)) {
            words.append(s);
        }
    }


public:
    QVector<QString> boysSurnames;
    QVector<QString> girlsSurnames;
    QVector<QString> words;
    QMap<int, QString> zipCodes;
    QString m_Path;
};
}
}


Randomizer::Randomizer() :
        d(0)
{
    d = new Internal::RandomizerPrivate;
    srand(time(NULL));
}

Randomizer::~Randomizer()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void Randomizer::setPathToFiles(const QString &p)
{
    d->m_Path = p;
}

QString Randomizer::getRandomSurname(bool male)
{
    if (d->boysSurnames.isEmpty())
        d->readBoysSurnames();
    if (d->girlsSurnames.isEmpty())
        d->readGirlsSurnames();
    if (male)
        return d->boysSurnames.at(makeRand(d->boysSurnames.size() - 1));
    return d->girlsSurnames.at(makeRand(d->girlsSurnames.size() - 1));
}

QString Randomizer::getRandomString(int length)
{
    static char consonnes[]="BCDFGHJKLMNPQRSTVWXZ";
    static char voyelles[]="AEIOUY";

    QString s;
    int l = length;
    if (length == 0) {
        l = 5;
    }
    for (int i=0; i<l; i++)
    {
        if ((l > 3) && (makeRand(5) == 2))
            s.append(" ");
        else if ((i % 2) == 0)
            s.append(QChar(consonnes[makeRand(20)]));
        else
            s.append(QChar(voyelles[makeRand(6)]));
    }
    return s;
}

QPair<int, QString> Randomizer::getRandomFrenchCity()
{
    if (d->zipCodes.isEmpty())
        d->readZipCodes();
    QPair<int, QString> p;
    int r = makeRand(d->zipCodes.count() - 1);
    p.first = d->zipCodes.keys().at(r);
    p.second = d->zipCodes.value(p.first);
    return p;
}

QString Randomizer::getRandomName()
{
    if (d->words.isEmpty())
        d->readFrenchWords();
    return d->words.at(makeRand(d->words.count() - 1));
}

int Randomizer::randomInt(int max)
{
    return makeRand(max);
}

QDate Randomizer::randomDate(int minYear)
{
    return QDate(minYear + makeRand(QDate::currentDate().year() - minYear), makeRand(12), makeRand(28));
}

