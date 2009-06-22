/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
 *   This code is inspired of the Monkey Studio project                    *
 *   http://www.monkeystudio.org/                                          *
 *   Filipe AZEVEDO aka Nox P@sNox <pasnox@gmail.com>                      *
 *                                                                         *
 *   Adaptations to FreeMedForms by : Eric Maeker, MD                      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

#ifndef MFBASEINTERFACE_H
#define MFBASEINTERFACE_H

#include <mfExporter.h>
#include <QtPlugin>
#include <QPixmap>
#include <QStringList>
#include <QVariant>

/**
 * \file mfBaseInterface.h
 * \author Eric MAEKER, MD <eric.maeker@free.fr>
 * \version 0.0.8
 * \date 18 April 2009
*/

class Q_FMF_EXPORT mfBaseInterface : public QObject
{
     Q_OBJECT
     Q_ENUMS( Type )

public:
     // plugin type enums
     enum Type
     { iAll = 0x0,
       iFormWidget = 0x1,
       iIO = 0x2,
       iPrinter = 0x4,
       iLast = 0x100
     };
     Q_DECLARE_FLAGS( Types, Type )


     QString typeToString( Types t )
     {
         QString tmp;
         if (t & iFormWidget)
             tmp += tr("FormWidget plugin, ");
         else if (t & iIO)
             tmp += tr("IO plugin, ");
         else if (t & iPrinter)
             tmp += tr("Printer plugin, ");
         else if (t & iAll)
             tmp += tr("All plugin, ");
         tmp.chop(2);
         return tmp;
     }


     // plugin info structure
     /** \brief Structure that holds the plugins informations. */
     struct Q_FMF_EXPORT PluginInfos
     {
          QString Caption;                    /*!< the string to show as caption */
          QString Description;                /*!< the plugin description */
          QString Author;                     /*!< the plugin author */
          mfBaseInterface::Types Type;        /*!< the plugin type ( can be or-ded ) */
          QStringList Languages;              /*!< language this plugin is for, default empty mean all */
          QString Name;                       /*!< the plugin name for version control */
          QString Version;                    /*!< the plugin version for version control */
          QString License;                    /*!< the plugin license */
          bool Enabled;                       /*!< to know if this plugin is enabled */
          QString MinimumFreemedformsVersion; /*!<  Minimal FreeMedForms compatible version. */
     };

     /** */
     mfBaseInterface()
     { mPluginInfos.Enabled = false; }

     /** */
     virtual ~mfBaseInterface()
     { if ( isEnabled() ) setEnabled( false ); }

     /** */
     virtual PluginInfos infos() const
     { return mPluginInfos; }

     /** */
     virtual QPixmap pixmap() const
     { return QPixmap(); }

     /** */
     virtual QWidget* settingsWidget()
     { return 0; }

     /** */
     virtual bool isEnabled() const
     { return mPluginInfos.Enabled; }

     /** */
     virtual bool setEnabled( bool )
     { return false; }


/*
     virtual QString settingsKey( const QString& k ) const
     { return QString( "Plugins/%1/%2" ).arg( mPluginInfos.Name ).arg(  k ); }


     virtual QVariant settingsValue( const QString& k, const QVariant& v = QVariant() ) const
     { return mfCore::settings()->value( settingsKey( k ), v ); }


     virtual void setSettingsValue( const QString& k, const QVariant& v ) const
     { mfCore::settings()->setValue( settingsKey( k ), v ); }
*/

protected:
     PluginInfos mPluginInfos;     /*!< Plugins informations structure. */
};
Q_DECLARE_OPERATORS_FOR_FLAGS( mfBaseInterface::Types )
Q_DECLARE_METATYPE( mfBaseInterface::PluginInfos )
Q_DECLARE_INTERFACE( mfBaseInterface, "fr.ericmaeker.FreeMedForms.mfBaseInterface/1.0" )

#endif
