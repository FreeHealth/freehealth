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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "episodemodel.h"
#include "constants_db.h"

#include <translationutils/constanttranslations.h>

using namespace Patients;

using namespace Trans::ConstantTranslations;

namespace Patients {
namespace Internal {
class EpisodeModelPrivate
{
public:
    EpisodeModelPrivate(EpisodeModel *parent) : q(parent) {}
    ~EpisodeModelPrivate () {}

private:
    EpisodeModel *q;
};
}
}



EpisodeModel::EpisodeModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::EpisodeModelPrivate(this))
{
    setObjectName("EpisodeModel");
}

EpisodeModel::~EpisodeModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int EpisodeModel::rowCount(const QModelIndex &parent) const
{}

int EpisodeModel::columnCount(const QModelIndex &parent) const
{}

bool EpisodeModel::hasChildren(const QModelIndex &parent) const
{}

QVariant EpisodeModel::data(const QModelIndex &index, int role) const
{}

bool EpisodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{}


QVariant EpisodeModel::headerData(int section, Qt::Orientation orientation, int role) const
{}

bool EpisodeModel::insertRows(int row, int count, const QModelIndex &parent)
{}

bool EpisodeModel::removeRows(int row, int count, const QModelIndex &parent)
{}


