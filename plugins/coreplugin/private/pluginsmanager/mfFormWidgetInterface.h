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
#ifndef MFFORMWIDGETPLUGININTERFACE_H
#define MFFORMWIDGETPLUGININTERFACE_H

#include <QtPlugin>
// #include <mfExporter.h>
#include <mfObject.h>
#include <mfAbstractWidget.h>
#include <mfBaseInterface.h>

class QStringList;
class QWidget;

/**
 * \file mfFormWidgetInterface.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.03
 * \date 31 octobre 2008
*/

/**
 * This class is an interface for the form's widget plugins. Plugin Must implement this interface.
*/
class /*Q_FMF_EXPORT*/ mfFormWidgetInterface : public mfBaseInterface
{

public:

     /**
      * \~english Just the virtual destructor needed by the compilator.
      * \~french Juste le destructeur virtuel nécessaire au compilateur.
     */
     virtual ~mfFormWidgetInterface() {}

     /**
      * \~english This function is used to know with widget this plugin owns. \n
      * \~french Cette fonction est utiliser pour connaître les éléments de formulaires que distribue le plugin.
      * \~english The application will ask a mfAbstractWidget if it's type is returned in this list.
      * \~french L'application demandera un mfAbstractWidget si son type est renvoyé par cette liste.
      *
      * For exemple : a richtext widget plugin will return :
      * QStringList() << "richtext" << "simplerichtext" << "fullrichtext";
      * 
     */
     virtual QStringList   widgets() const = 0;


     /**
      * \~english This function is used to know if the asked widget is a container.\n
      * \~french Cette fonction est utiliser savoir si l'élément de formulaire demandé est un conteneur.
      * 
      * \~english \param idInStringList is the index of the asked widget in the stringlist returned by widgets().\n
      * \~french \param idInStringList est l'index de l'élément de formulaire demandé dans la stringlist renvoyée par widgets.\n
     */
     virtual bool          isContainer( const int idInStringList ) const = 0;


     /** 
      * \~english This function is called to get the asked widget.\n
      * \~french Cette fonction est appelée pour récupérer l'élément de formulaire demandé.
      * 
      * \~english \param mfo is the mfObject linked to the widget.\n
      * \~english \param parent is the mfAbstractWidget 's parent.\n
      *
      * \~french \param mfo est le mfObject lié à l'élément de formulaire.\n
      * \~french \param parent est l'élément parent de type mfAbstractWidget.\n
     */
     virtual bool          isContainer( const int idInStringList ) const = 0;
     virtual mfAbstractWidget *  getWidget( mfObject * mfo, mfAbstractWidget * parent = 0) const = 0;

};

Q_DECLARE_INTERFACE( mfFormWidgetInterface,
                     "fr.ericmaeker.FreeMedForms.mfFormWidgetInterface/1.0" );

#endif

