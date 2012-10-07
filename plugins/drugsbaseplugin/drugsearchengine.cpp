/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugsearchengine.h"
#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/idrug.h>

#include <utils/global.h>

#include <QList>

using namespace DrugsDB;
using namespace Internal;

static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}

namespace DrugsDB {
namespace Internal {

struct Engine {
    QString label, url, lang;
    QHash<QString, QString> processedLabel_Url;
};

class DrugSearchEnginePrivate
{
public:
    DrugSearchEnginePrivate() : m_Drug(0)
    {}

    ~DrugSearchEnginePrivate()
    {
        qDeleteAll(m_Engines);
        m_Engines.clear();
    }

public:
    QList<Engine *> m_Engines;
    const IDrug *m_Drug;
};

DrugSearchEngine *DrugSearchEngine::m_Instance = 0;
}
}

DrugSearchEngine::DrugSearchEngine() :
        d(new DrugSearchEnginePrivate)
{
}

DrugSearchEngine::~DrugSearchEngine()
{
    if (d)
        delete d;
    d = 0;
}

DrugSearchEngine *DrugSearchEngine::instance()
{
    if (!m_Instance)
        m_Instance = new DrugSearchEngine;
    return m_Instance;
}

void DrugSearchEngine::clear()
{
    qDeleteAll(d->m_Engines);
    d->m_Engines.clear();
}

void DrugSearchEngine::addNewEngine(const QString &label, const QString &url, const QString &lang)
{
    Engine *e = new Engine;
    e->label = label;
    e->url = url;
    e->lang = lang;
    d->m_Engines.append(e);
}

void DrugSearchEngine::setDrug(const IDrug *drug)
{
    d->m_Drug = drug;
    if (!drug)
        return;

    // Process labels and urls
    // Replace tokens
    // - [[ONE_ATC_CODE]] --> 1 ATC CODE == 1 more link
    // - [[DRUG_ATC]] --> if known the DRUGS.ATC value (ATC code)
    // - [[ATC_CODES]] --> all know ATC Codes of the drug (and its molecular components)
    // - [[ATC_LABELS]] --> all known ATC Label of the drug (and its molecular components)
    // - [[DRUG_NAME]] --> exact value of the DRUGS.NAME field
    // - [[CONSTRUCTED_DRUG_NAME]] --> the constructed drug name like inform in the INFORMATION table
    // - [[DRUG_UID]] --> the uid of the drug
    const QVector<int> &atc_ids = d->m_Drug->allAtcIds();
    QHash<int, QString> atcCodes;
    QHash<int, QString> atcLabels;
    for(int i = 0; i<atc_ids.count(); ++i) {
        int id = atc_ids.at(i);
        const QString &lbl = drugsBase().getAtcLabel(id);
        if (atcLabels.values().contains(lbl))
            continue;
        atcCodes.insert(id, drugsBase().getAtcCode(id));
        atcLabels.insert(id, lbl);
    }

    QHash<QString, QString> tokens;
    const QStringList &codes = atcCodes.values();
    const QStringList &labels = atcLabels.values();
    tokens.insert("DRUG_ATC", d->m_Drug->atcCode());
    tokens.insert("ATC_CODES", codes.join("%20"));
    tokens.insert("ATC_LABELS", labels.join("%20"));
    tokens.insert("DRUG_NAME", d->m_Drug->brandName());
    // TODO: improve this "DRUG_UID"
    tokens.insert("DRUG_UID", d->m_Drug->uids().join(";"));
    tokens.insert("DRUG_LINK_SPC", d->m_Drug->linkToSCP());
//    tokens.insert("CONSTRUCTED_DRUG_NAME", d->m_Drug->);

    foreach(Engine *e, d->m_Engines) {
        e->processedLabel_Url.clear();
        if (e->url.contains("[[ONE_ATC_CODE]]")) {
            foreach(const QString &code, codes) {
                QString url = e->url;
                const QString &lbl = atcLabels.value(atcCodes.key(code));
                url.replace("[[ONE_ATC_CODE]]", code);
                e->processedLabel_Url.insert(QString("%1 (%2)").arg(e->label).arg(lbl), url);
//                qWarning() << QString("%1 (%2)").arg(e->label).arg(lbl);
            }
            continue;
        }
        QString url = e->url;
        bool proceed = true;
        foreach(const QString &token, tokens.keys()) {
            if (url.contains(token) && tokens.value(token).isEmpty())
                proceed = false;
        }
        if (proceed) {
            Utils::replaceTokens(url, tokens);
            e->processedLabel_Url.insert(e->label, url);
//            qWarning() << e->label;
        }
    }
}

QStringList DrugSearchEngine::processedLabels(const QString &lang) const
{
    QStringList toReturn;
    foreach(const Engine *e, d->m_Engines) {
        if (e->lang == lang)
            toReturn << e->processedLabel_Url.keys();
    }
    return toReturn;
}

QStringList DrugSearchEngine::processedUrls(const QString &label, const QString &lang) const
{
    QStringList urls;
    foreach(const Engine *e, d->m_Engines) {
        if (e->lang == lang && (e->label == label || e->processedLabel_Url.keys().contains(label)))
            urls << e->processedLabel_Url.values(label);
    }
    return urls;
}

int DrugSearchEngine::numberOfEngines() const
{
    return d->m_Engines.count();
}


