/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DDI::Internal::ServerUpdate
 * \brief short description of class
 *
 * Long description of class
 * \sa DDI::
 */

#include "serverupdate.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {

const char * const T_ATC =
        "===== [[CODE]][ / [En]][ / [Fr]] ====="
        "|  ^  Value  ^"
        "^  HashTag (do not edit)  |  [[HASHTAG]]  |\n"
        "^  Uid  |  [[UID]]  |\n"
        "^  IsValid (true/false)  |  [[ISVALID]]  |\n"
        "^  Code  |  [[CODE]]  |\n"
        "^  Fr  |  [[Fr]]  |\n"
        "^  En  |  [[En]]  |\n"
        "^  De  |  [[De]]  |\n"
        "^  PreviousCode  |  [[PREVCODE]]  |\n"
        "^  WhoYearUpdate  |  [[WHOYEARUPDATE]]  |\n"
        "^  Comment  |  [[COMMENT]]  |\n"
        ;

const char * const T_INTERACTOR =
        "===== [[UID]][ / [En]][ / [Fr]] ====="
        "|  ^  Value  ^"
        "^  HashTag (do not edit)  |  [[HASHTAG]]  |\n"
        "^  Uid  |  [[UID]]  |\n"
        "^  IsValid (true/false)  |  [[ISVALID]]  |\n"
        "^  IsReviewed (true/false)  |  [[ISREVIEWED]]  |\n"
        "^  IsClass (true/false)  |  [[ISCLASS]]  |\n"
        "^  WarnDuplicates (true/false)  |  [[WARNDUPLICATES]]  |\n"
        "[^  ATC  |  [ATC]  |\n]"
        "[^  Children  |  [Children]]  |\n]"
        "^  Fr  |  [[Fr]]  |\n"
        "^  En  |  [[En]]  |\n"
        "^  De  |  [[De]]  |\n"
        "^  InfoFr  |  [[InfoFr]]  |\n"
        "^  InfoEn  |  [[InfoEn]]  |\n"
        "^  InfoDe  |  [[InfoDe]]  |\n"
        "^  PMIDS  |  [[PMIDS]]  |\n"
        "^  Comment  |  [[COMMENT]]  |\n"
        ;

// TODO: manage drug-drug interactions dosage definition && routes
const char * const T_DDI =
        "===== [[LEVELNAME]]: [[FIRSTINTERACTOR]] / [[SECONDINTERACTOR]] ====="
        "|  ^  Value  ^"
        "^  HashTag (do not edit)  |  [[HASHTAG]]  |\n"
        "^  Uid  |  [[UID]]  |\n"
        "^  IsValid (true/false)  |  [[ISVALID]]  |\n"
        "^  IsReviewed (true/false)  |  [[ISREVIEWED]]  |\n"
        "^  First  |  [[FIRSTINTERACTORUID]]  § [[FIRSTINTERACTOR]] §  |\n"
        "^  Second  |  [[SECONDINTERACTORUID]]  § [[SECONDINTERACTOR]] §  |\n"
        "^  LevelCode  |  [[LEVELCODE]]  § [[LEVELNAME]] §  |\n"
        "^  RiskFr  |  [[RISKFR]]  |\n"
        "^  RiskEn  |  [[RISKEN]]  |\n"
        "^  ManagementFr  |  [[MANAGEMENTFR]]  |\n"
        "^  ManagementEn  |  [[MANAGEMENTEN]]  |\n"
        "^  Source  |  [[SOURCE]]  |\n"
        "^  Comment  |  [[COMMENT]]  |\n"
        "^  PMIDS  |  [[PMIDS]]  |\n"
        ;
}

namespace DDI {
namespace Internal {
class ServerUpdatePrivate
{
public:
    ServerUpdatePrivate(ServerUpdate *parent) :
        _type(ServerUpdate::New),
        _subType(ServerUpdate::Atc),
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~ServerUpdatePrivate()
    {
    }

    // Returns the wiki mask to use to create the wiki content using the tokens
    // _subtype must be defined
    QString wikiMask() const
    {
        switch (_subType) {
        case ServerUpdate::Atc: return ::T_ATC;
        case ServerUpdate::Interactor: return ::T_INTERACTOR;
        case ServerUpdate::DrugDrugInteraction: return ::T_DDI;
        }
        return QString::null;
    }

    // Transforms a wiki content to hashed tokens
    // _type & _subType must be defined
    bool wikiContentToTokens(const QString &wikiContent)
    {
        // TODO: code here
        Q_UNUSED(wikiContent);
        return false;
    }
    
public:
    ServerUpdate::UpdateType _type;
    ServerUpdate::UpdateSubType _subType;
    QHash<QString,QString> _tokens;
    QString _hashTag;
    
private:
    ServerUpdate *q;
};
} // namespace Internal
} // end namespace DDI


/*! Constructor of the DDI::Internal::ServerUpdate class */
ServerUpdate::ServerUpdate() :
    d(new ServerUpdatePrivate(this))
{
}

/*! Destructor of the DDI::Internal::ServerUpdate class */
ServerUpdate::~ServerUpdate()
{
    if (d)
        delete d;
    d = 0;
}

ServerUpdate::ServerUpdate(const ServerUpdate &cp) :
    d(new ServerUpdatePrivate(this))
{
    d->_type = cp.d->_type;
    d->_subType = cp.d->_subType;
    d->_tokens = cp.d->_tokens;
    d->_hashTag = cp.d->_hashTag;
}

void ServerUpdate::setType(UpdateType type)
{
    d->_type = type;
}

void ServerUpdate::setSubType(UpdateSubType subtype)
{
    d->_subType = subtype;
}

void ServerUpdate::setTokens(const QHash<QString, QString> &tokens)
{
    d->_tokens = tokens;
}

ServerUpdate::UpdateType ServerUpdate::type() const
{
    return d->_type;
}

ServerUpdate::UpdateSubType ServerUpdate::subType() const
{
    return d->_subType;
}

QString ServerUpdate::wikiContent() const
{
    return QString::null;
}

QString ServerUpdate::hashTag() const
{
    return d->_hashTag;
}

bool ServerUpdate::populateFromServerContent(const QString &hashTag)
{
    Q_UNUSED(hashTag);
    return false;
}

bool ServerUpdate::operator==(const ServerUpdate &other) const
{
    return d->_type == other.d->_type
            && d->_subType == other.d->_subType
            && d->_tokens == other.d->_tokens
            && d->_hashTag == other.d->_hashTag;
}

ServerUpdate &ServerUpdate::operator=(const ServerUpdate &cp)
{
    d->_type = cp.d->_type;
    d->_subType = cp.d->_subType;
    d->_tokens = cp.d->_tokens;
    d->_hashTag = cp.d->_hashTag;
    return *this;
}
