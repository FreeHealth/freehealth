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
# 'feuilles de soins'.                                                                  *
# ***************************************************************************************
# * We suggest to activate this feature if you want to use FreeMedForms in France       *
# ***************************************************************************************
# Uncomment the following line to activate the feature
 DEFINES *= WITH_FRENCH_FSP
