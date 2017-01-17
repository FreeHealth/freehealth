# ***************************************************************************************
# *                           Drug database compatibility                               *
# ***************************************************************************************
# * By default defined to the latest version, but you can define another version.       *
# * Available versions:                                                                 *
# *     - 0.6.4 : use 0x000604                                                          *
# *     - 0.8.4 : use 0x000804                                                          *
# ***************************************************************************************
# * We suggest to keep this value unchanged unless you know what you are doing.         *
# ***************************************************************************************
# Uncomment the following line to activate the feature
# DEFINES += DRUGS_DATABASE_VERSION=0x000604
DEFINES += DRUGS_DATABASE_VERSION=0x000804

# ***************************************************************************************
# *                                    Auto-locking                                     *
# ***************************************************************************************
# * By default this feature is activated. When activated, users can define time         *
# * to automatically lock the application. User will then be asked for his              *
# * identification.                                                                     *
# * This is a good security feature, but can be redondant with OS configuration.        *
# ***************************************************************************************
# * We would recommend that you not activate this feature as it is not currently stable.*
# ***************************************************************************************
# Uncomment the following line to activate the feature
# DEFINES *= WITH_UI_AUTO_LOCKING

# ***************************************************************************************
# *                                   User auto login                                   *
# ***************************************************************************************
# * By default this feature is not activated. When activated, users can be              *
# * automatically connected to the application according to their last identification   *
# * without any required action.                                                        *
# * This is a high security failure to activate this feature, as it keeps a weak        *
# * crypted version of the credentials of the user on the local hard drive.             *
# * Activating this feature can lead to severe security compromise.                     *
# ***************************************************************************************
# * Dependencies: WITH_LOGINANDPASSWORD_CACHING feature.                                *
# ***************************************************************************************
# * We suggest to never activate this feature                                           *
# ***************************************************************************************
# Uncomment the following line to activate the feature
# DEFINES *= WITH_USER_AUTOLOGIN

# ***************************************************************************************
# *                                 Allow episode removal                               *
# ***************************************************************************************
# * By default this feature is activated. When activated, users can remove episode if   *
# * they are user validated.                                                            *
# ***************************************************************************************
# * We suggest to activate this feature as it does not lead to data loss (all removed   *
# * episodes are still stored in the database and can be reactivated with a simple code *
# * adaptation.                                                                         *
# ***************************************************************************************
# Uncomment the following line to activate the feature
 DEFINES *= WITH_EPISODE_REMOVAL

# ***************************************************************************************
# *                                   Cheque printing                                   *
# ***************************************************************************************
# * By default this feature is activated. When activated, users can print cheques.      *
# ***************************************************************************************
# * We suggest to activate this feature                                                 *
# ***************************************************************************************
# Uncomment the following line to activate the feature
 DEFINES *= WITH_CHEQUE_PRINTING

# ***************************************************************************************
# *                            French specific: printing FSP                            *
# ***************************************************************************************
# * By default this feature is activated. When activated, users can print french        *
# * 'feuilles de soins'.                                                                *
# ***************************************************************************************
# * We suggest to activate this feature if you want to use the application in France    *
# ***************************************************************************************
# Uncomment the following line to activate the feature
 DEFINES *= WITH_FRENCH_FSP

# ***************************************************************************************
# *                          French specific: HPRIM Integrator                          *
# ***************************************************************************************
# * By default this feature is activated. When activated, users can import HPRIM files  *
# * stored in a specific path into any patient file.                                    *
# * This feature can be used to quickly import decrypted files from apicrypt for eg.    *
# ***************************************************************************************
# Uncomment the following line to activate the feature
 DEFINES *= WITH_FRENCH_HPRIM_INTEGRATOR

# ***************************************************************************************
# *                                       QuaZip                                        *
# ***************************************************************************************
# * The application uses the external lib QuaZip to manage file/dir (de)compression.    *
# * On some OS we need to build quazip (like Mac OS and Windows) because it is not      *
# * already available. On Linux, we might have to build QuaZip or not. A package now    *
# * exists for the main Linux distros. If you want to link the application with the     *
# * system's QuaZip lib, leave the config commented.                                    *
# ***************************************************************************************
# * If you don't what you are doing here, keep this line commented.                     *
# ***************************************************************************************
# Uncomment the following line to activate the feature
# CONFIG += dontbuildquazip
