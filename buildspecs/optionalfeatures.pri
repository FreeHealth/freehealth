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
# * to automatically lock FreeMedForms. FreeMedForms will then ask user for his         *
# * identification.                                                                     *
# * This is a good security feature, but can be redondant with OS configuration.        *
# ***************************************************************************************
# * We suggest to not activate this feature as it is not currently stable.              *
# ***************************************************************************************
# Uncomment the following line to activate the feature
# DEFINES *= WITH_UI_AUTO_LOCKING

# ***************************************************************************************
# *                                   User auto login                                   *
# ***************************************************************************************
# * By default this feature is not activated. When activated, users can get             *
# * automatically connected to FreeMedForms according to their last identification.     *
# * This is a high security failure to activate this feature, as it keeps a weak        *
# * crypted version of the identifiants of the user on the local hard drive.            *
# ***************************************************************************************
# * Dependencies: WITH_LOGINANDPASSWORD_CACHING feature.                                *
# ***************************************************************************************
# * We suggest to never activate this feature                                           *
# ***************************************************************************************
# Uncomment the following line to activate the feature
# DEFINES *= WITH_USER_AUTOLOGIN

# ***************************************************************************************
# *                                  Password caching                                   *
# ***************************************************************************************
# * By default this feature is not activated. When activated, current user's login and  *
# * password are cached on a local file with a very weak encryption.                    *
# * This is a high security failure to activate this feature.                           *
# ***************************************************************************************
# * We suggest to never activate this feature                                           *
# ***************************************************************************************
# Uncomment the following line to activate the feature
# DEFINES *= WITH_LOGINANDPASSWORD_CACHING

# ***************************************************************************************
# *                                 Allow episode removal                               *
# ***************************************************************************************
# * By default this feature is activated. When activated, users can remove episode if   *
# they are user validated.                                                              *
# ***************************************************************************************
# * We suggest to activate this feature as it does not lead to data lose (all removed   *
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
# * We suggest to activate this feature if you want to use FreeMedForms in France       *
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
# * We suggest to activate this feature if you want to use FreeMedForms in France       *
# ***************************************************************************************
# Uncomment the following line to activate the feature
 DEFINES *= WITH_FRENCH_HPRIM_INTEGRATOR

# ***************************************************************************************
# *                                       QuaZip                                        *
# ***************************************************************************************
# * FreeMedForms uses the external lib QuaZip to manage file/dir (de)compression.       *
# * On some OS we need to build quazip (like Mac OS and Windows) because it is not      *
# * already available. On Linux, we can need to build QuaZip or not. A package now      *
# * exists for the main Linux distro. So if you want to link FreeMedForms with the      *
# * system's QuaZip lib, then uncomment the following line.                             *
# ***************************************************************************************
# * If you don't what you are doing here, keep this line commented.                     *
# ***************************************************************************************
# Uncomment the following line to activate the feature
# CONFIG += dontbuildquazip

