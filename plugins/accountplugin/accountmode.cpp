/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developpers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "accountmode.h"
#include "accountview.h"
#include "receipts/receiptviewer.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <QMessageBox>
#include <QFile>
#include <QPushButton>

using namespace Account::Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

AccountMode::AccountMode(QObject *parent) :
    Core::BaseMode(parent)
{
    setName(tr("Accountancy"));
    setIcon(theme()->icon(Core::Constants::ICONACCOUNTMODE, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_ACCOUNT);
    setUniqueModeName(Core::Constants::MODE_ACCOUNT);
//    const QList<int> &context;
//    setContext();
    m_Stack = new QStackedWidget;
    m_Stack->addWidget(new ReceiptViewer);
    setWidget(m_Stack);
    QString fileFirstTimeString = settings()->path(Core::ISettings::BundleResourcesPath) 
                                              + "/textfiles/accountNotFirstTime.txt";
    QFile fileFirst(fileFirstTimeString,this);
    if (!fileFirst.exists())
    {
    	  QString firstExplanationText = trUtf8("Please read this explanation before using FreeAccount.\n"
    	                                 "FreeAccount is composed of :\n"
    	                                 "Receipts to get your earnings,\n"
    	                                 "Movements in your accountancy,\n"
    	                                 "Asset for your assets,\n"
    	                                 "Ledger to analyse your accountancy and produce and print your ledger.\n"
    	                                 "The shortcuts for those programs are :\n"
    	                                 "for your rapid receipt, CTRL+R,\n"
    	                                 "for the receipts widget, Maj+R,\n"
    	                                 "for the movements widget, CTRL+M,\n"
    	                                 "for assets widget, ALT+Z,\n"
    	                                 "for ledger and analysis widget, ALT+L.\n"
    	                                 "To avoid to see this message again, click the check box.");
    	  
    	  QMessageBox mess ;
    	  mess.setWindowTitle(trUtf8("Read me."));
    	  mess.setInformativeText(firstExplanationText);
    	  mess.setStandardButtons(QMessageBox::Ok);
    	  mess.setDefaultButton(QMessageBox::Ok);
    	  QPushButton * notAgain = mess.addButton(trUtf8("Do not show me this message again."),QMessageBox::ActionRole);
    	  mess.exec();
    	  if (mess.clickedButton() == notAgain )
    	  {
    	  	  qDebug() << __FILE__ << QString::number(__LINE__) << " action " ;
    	  	  createFirstTimeTxt();
    	      }
    	  }
    /** \todo connect actions from the account menu */
    /** \todo connect patient changed and refresh views ? */
}

void AccountMode::setCentralWidget(QWidget *widget)
{
    QWidget *w = m_Stack->currentWidget();
    m_Stack->removeWidget(w);
    delete w;
    w = 0;
    m_Stack->addWidget(widget);
    m_Stack->setCurrentWidget(widget);
}

void AccountMode::createFirstTimeTxt(){
    qWarning() << __FILE__ << QString::number(__LINE__) << "create first time file." ;
    QString fileFirstTimeString = settings()->path(Core::ISettings::BundleResourcesPath) 
                                  + "/textfiles/accountNotFirstTime.txt";
    QFile fileFirstTime(fileFirstTimeString);
    if (!fileFirstTime.open(QIODevice::WriteOnly))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "fileFirstTime cannot be created." ;
        }
    fileFirstTime.close();
}
