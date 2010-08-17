/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "drugsearchengine.h"
#include "drugsdata.h"
#include "drugsbase.h"

#include <utils/global.h>

#include <QList>

using namespace DrugsDB;
using namespace Internal;

static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}

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
    const DrugsData *m_Drug;
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

void DrugSearchEngine::addNewEngine(const QString &label, const QString &url, const QString &lang)
{
    Engine *e = new Engine;
    e->label = label;
    e->url = url;
    e->lang = lang;
    d->m_Engines.append(e);
}

void DrugSearchEngine::setDrug(const DrugsData *drug)
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
    // - [[CONSTRUCTED_DRUG_NAME]] --> the constructed drug name like inform in the INFORMATIONS table
    // - [[DRUG_UID]] --> the uid of the drug
    const QSet<int> atc_ids = d->m_Drug->allAtcIds();
    QHash<int, QString> atcCodes;
    QHash<int, QString> atcLabels;
    foreach(int id, atc_ids) {
        const QString &lbl = drugsBase()->getAtcLabel(id);
        if (atcLabels.values().contains(lbl))
            continue;
        atcCodes.insert(id, drugsBase()->getAtcCode(id));
        atcLabels.insert(id, lbl);
    }

    QHash<QString, QString> tokens;
    const QStringList &codes = atcCodes.values();
    const QStringList &labels = atcLabels.values();
    tokens.insert("DRUG_ATC", d->m_Drug->ATC());
    tokens.insert("ATC_CODES", codes.join("%20"));
    tokens.insert("ATC_LABELS", labels.join("%20"));
    tokens.insert("DRUG_NAME", d->m_Drug->denomination());
    tokens.insert("DRUG_UID", QString(d->m_Drug->UID()));
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


