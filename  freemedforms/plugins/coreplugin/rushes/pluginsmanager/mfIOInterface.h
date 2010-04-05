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
#ifndef MFIOINTERFACE_H
#define MFIOINTERFACE_H

#include <QtPlugin>
#include <mfExporter.h>
#include <mfObject.h>
#include <mfAbstractWidget.h>
#include <mfBaseInterface.h>
#include <mfIOPlace.h>

/**
 * \file mfFormWidgetInterface.h
 * \author Guillaume DENRY <guillaume.denry@gmail.com>
 * \version 0.01
 * \date 15 novembre 2008
*/

/**
 * This class is an interface for object IO. Plugin Must implement this interface.
*/
class Q_FMF_EXPORT mfIOInterface : public mfBaseInterface
{

public:
     /**
      * \brief Load an mfObject from a place
      * @param place The place where the mfObject will be read
      * @return a newly allocated mfObject or 0 if an error occurred
      */
     virtual mfObject *loadForm(const mfIOPlace &place ) = 0;

     /**
      * \brief Save an mfObject to a place
      * @param place The place where the mfObject will be saved
      * @param object The object to save
      * @return true if the save operation was successful
      */
     virtual bool saveForm(const mfIOPlace &place , const mfObject &object ) = 0;

     /**
      * \brief Load mfObject data from a place
      * @param place The place where the mfObject will be read
      * @param object The object in wich data will be set
      * @return true if the load operation if successful
      */
     virtual bool loadFormData(const mfIOPlace &place, mfObject &object ) = 0;

     /**
      * \brief Save an mfObject data to a place
      * @param place The place where the mfObject data will be saved
      * @param object The object from which the data will be saved
      * @return true if the save operation was successful
      */
     virtual bool saveFormData(const mfIOPlace &place , const mfObject &object ) = 0;
};

Q_DECLARE_INTERFACE( mfIOInterface,
                     "fr.ericmaeker.FreeMedForms.mfIOInterface/1.0" );

#endif
