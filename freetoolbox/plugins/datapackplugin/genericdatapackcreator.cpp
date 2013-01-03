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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "genericdatapackcreator.h"

using namespace DataPackPlugin;

/**
  <datapack description="">
    <content type=""></content>
  </datapack>
 */

GenericDataPackCreator::GenericDataPackCreator(QObject *parent) :
    Core::IFullReleaseStep(parent)
{
}

GenericDataPackCreator::Steps GenericDataPackCreator::stepNumber() const
{
    return Extras;
}

bool GenericDataPackCreator::createTemporaryStorage()
{
    return true;
}

bool GenericDataPackCreator::cleanTemporaryStorage()
{
    return true;
}

bool GenericDataPackCreator::startDownload()
{
    Q_EMIT downloadFinished();
    return true;
}

bool GenericDataPackCreator::postDownloadProcessing()
{
    Q_EMIT postDownloadProcessingFinished();
    return true;
}

bool GenericDataPackCreator::process()
{
    Q_EMIT processFinished();
    return true;
}

bool GenericDataPackCreator::registerDataPack()
{
    // Read XML file
    // Create pack foreach registered XMLpack
    return true;
}

QString GenericDataPackCreator::processMessage() const
{
    return QString::null;
}
