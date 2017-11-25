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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "ifullreleasestep.h"

/*!
 * \class Core::IFullReleaseStep
 * \brief Provides a generic interface for generating data for FreeToolBox/FreeMedForms.
 *
 * It is a plugin extension point that is called during the FreeToolBox Core::IFullReleaseStep action.
 * Classes that implement this interface can provide a data source and storage for arbitrary data
 * that can be used then in FreeMedForms or one of its sub-applications.
 *
 * Life cycle of the object:\n
 * The Object should be created by your inherited Core::IToolPage classes and added to the
 * plugin manager object pool.
 *
 * Actions & order:\n
 * When the user selects a "Create Full Release", all IFullReleaseStep objects are retrieved from the
 * plugin manager object pool and actions are called in the following order:
 * - createTemporaryStorage()
 * - downloadFiles(): download your files and when done, emit the downloadFinished() signal
 * - postProcessDownload(): process whatever just after the download is finished then emit the postProcessDownloadFinished() signal
 * - process(): process your files and databases and when done emit the processFinished() signal
 * - cleanTemporaryStorage(): cleans all temporary created files, other cleanup
 *
 * The whole IFullReleaseStep Object is designed to work completely without UI, could be used in a command line tool
 * and must not depend on or call any UI widgets. The methods should be thread-safe, so any connection to the UI must be
 * done via Qt's Signal/Slot mechanism.
 *
 * Getting connected with the UI:\n
 * The whole Step object is a non-GUI object which must not have any direct GUI access. It may run threaded, and
 * direct GUI access is not thread save. Instead you can connect your objects to various signals.
 *
 * DEPRECATED: ***** this should be done by the callee! NO direct UI calls! *****\n
 * When a step is started, a modal progress dialog is executed. During all your sub-processes
 * you can adapt this progress dialog using the following signals:
 * - progressRangeChanged() to change the range of the progress dialog
 * - progressLabelChanged() to change the label of the progress dialog
 * - progress() to change the current progress dialog value
 *
 * Managing your own datapacks:\n
 * You can register datapacks by overloading the registerDataPack().
 */

/*!
 * \fn virtual QString Core::IFullReleaseStep::id() const = 0;
 * Returns a (meaningful) unique identifier for this step.
 */

/*!
 * \fn virtual Steps Core::IFullReleaseStep::stepNumber() const = 0;
 * \brief Represents the step number in the list of steps. The steps are
 * called in ascending order taken from this method.
 */

/*!
 * \fn virtual bool Core::IFullReleaseStep::createTemporaryStorage() = 0;
 * \brief Creates a temporary directory.
 * \returns \e true if successful, else \e false.
 */

/*!
 * \fn virtual bool Core::IFullReleaseStep::cleanTemporaryStorage() = 0;
 * \brief Removes the temporary directory and files.
 * \returns \e true if successful, else \e false.
 */

/*!
 * \fn virtual bool Core::IFullReleaseStep::startDownload() = 0;
 * \brief Starts downloading some files from a server.
 *
 * This method should not lock the event loop and
 * download all necessary files. It should just start the Download in a separate process, e.g. using
 * Utils::HttpDownloader, and return back to the main event loop.
 *
 * \returns \e true if successful, else \e false.
 */

/*! \fn virtual bool Core::IFullReleaseStep::process() = 0;
 * \brief Handles all the logic of the release step after finished downloading files.
 * \returns \e true if successful, else \e false.
 */

/*!
 * \fn virtual QString Core::IFullReleaseStep::processMessage() const = 0;
 * \brief Callback function, should provide a message that describes the current process.
 *
 * This message is presented in the modal ProgressDialog.
 */

/*!
 * \fn virtual bool Core::IFullReleaseStep::postDownloadProcessing()
 * \brief Implementations of this method can process the data after downloading.
 *
 * The default implementation emits \e postProcessDownloadFinished(). This should be done
 * in inherited implementations too.
 * \returns \e true if successful, else \e false.
 */

/*! \fn virtual bool Core::IFullReleaseStep::registerDataPack() = 0;
 * \brief Implementations of this method can register a custom datapack.
 *
 * This method is called after all processing methods (download, process...).
 * In your private implementation, you can create a DataPackPlugin::DataPackQuery and register it to
 * a specific datapack server. Basically, a datapack only contains one file (a database in general or a
 * zip file). You can register more than one datapack per object. \n
 * When all objects have registered their datapacks in the DataPackPlugin::DataPackCore
 * the final processing is an automated datapack server creation. Please refer to the
 * documentation of the DataPackPlugin::DataPackCore for further information.
 * \returns \e true if successful, else \e false.
 * \sa DataPackPlugin::DataPackCore::registerDataPack()
 */

/*!
 * \fn virtual QStringList Core::IFullReleaseStep::errors() const = 0;
 * \brief Contains a list of error strings if something went wrong.
 */

/*!
 * \fn static bool Core::IFullReleaseStep::lessThan(const Core::IFullReleaseStep::IFullReleaseStep *s1, const Core::IFullReleaseStep::IFullReleaseStep *s2)
 * Provides a comparison of two steps, used by the sort algorythm.
 */

/** Clear all registered errors */
void Core::IFullReleaseStep::clearErrors()
{
    _errors.clear();
}

/** Add an error message \e message for a specific \e timing and \e subProcess */
void Core::IFullReleaseStep::addError(ProcessTiming timing, SubProcess subProcess, const QString &message)
{
    Core::Internal::SubProcessError error;
    error.timing = timing;
    error.subProcess = subProcess;
    error.errorMsg = message;
    _errors << error;
}

/** Return \e true there is at least one error registered, otherwise return \e false. */
bool Core::IFullReleaseStep::hasError() const
{
    return (!_errors.isEmpty());
}

/** Return \e true if an error message is registered for a specific \e timing and \e subProcess, otherwise return \e false. */
bool Core::IFullReleaseStep::hasError(ProcessTiming timing, SubProcess subProcess) const
{
    foreach(const Core::Internal::SubProcessError &error, _errors) {
        if (error.timing == timing && error.subProcess == subProcess)
            return true;
    }
    return false;
}

/** Return the error message registered for a specific \e timing and \e subProcess or a null QString. */
QString Core::IFullReleaseStep::errorMessage(ProcessTiming timing, SubProcess subProcess) const
{
    foreach(const Core::Internal::SubProcessError &error, _errors) {
        if (error.timing == timing && error.subProcess == subProcess)
            return error.errorMsg;
    }
    return QString::null;
}
