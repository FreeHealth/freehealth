# Use this file to define the optional plugins to build for the EHR
# This file only applies to the EHR application

include(compiling.pri)
contains(FREEHEALTH,1){

    # In fact, you can use two methods:
    # 1) add to the qmake commande line:
    #    CONFIG+=with-plugin
    #    where 'plugin' is the config name of the plugin
    # 2) Use this file
    #    Uncomment required plugins

    # ***************************************************************************************
    # *                                  Agenda Plugin                                      *
    # ***************************************************************************************
    # Uncomment the following line to build the plugin
      CONFIG *= with-agenda
    # ***************************************************************************************


    # ***************************************************************************************
    # *                                  Alert Plugin                                       *
    # ***************************************************************************************
    # Uncomment the following line to build the plugin
      CONFIG *= with-alerts
    # ***************************************************************************************


    # ***************************************************************************************
    # *                                 Feedback Plugin                                     *
    # ***************************************************************************************
    # Uncomment the following line to build the plugin
      CONFIG *= with-feedback
    # ***************************************************************************************


    # ***************************************************************************************
    # *                                 PadTools Plugin                                     *
    # ***************************************************************************************
    # Uncomment the following line to build the plugin
    # FIXME: PadTools is not optional anymore
      CONFIG *= with-pad
    # ***************************************************************************************


    # ***************************************************************************************
    # *                                   PMHx Plugin                                       *
    # ***************************************************************************************
    # Uncomment the following line to build the plugin
      CONFIG *= with-pmh
    # ***************************************************************************************


    # ***************************************************************************************
    # *                                  Webcam Plugin                                      *
    # ***************************************************************************************
    # Uncomment the following line to build the plugin
    # CONFIG *= with-webcam
    # ***************************************************************************************


    # ***************************************************************************************
    # *                                French eDRC Plugin                                   *
    # ***************************************************************************************
    # Uncomment the following line to build the plugin
    # CONFIG *= with-edrc
    # ***************************************************************************************

    # ***************************************************************************************
    # *                                  Patientimport Plugin                                      *
    # ***************************************************************************************
    # Uncomment the following line to build the plugin
    # CONFIG *= with-patientimport
    # ***************************************************************************************

    # ***************************************************************************************
    # *                                  Record import plugin                               *
    # ***************************************************************************************
    # Uncomment the following line to build the plugin
    # CONFIG *= with-recordimport
    # ***************************************************************************************

    # The following lines must not be modified
    with-agenda         { DEFINES *= WITH_AGENDA }
    with-alerts         { DEFINES *= WITH_ALERTS }
    with-feedback       { DEFINES *= WITH_FEEDBACK }
    with-pad            { DEFINES *= WITH_PAD }
    with-pmh            { DEFINES *= WITH_PMH }
    with-webcam         { DEFINES *= WITH_WEBCAM }
    with-edrc           { DEFINES *= WITH_EDRC }
    with-patientimport  { DEFINES *= WITH_PATIENTIMPORT }
    with-recordimport   { DEFINES *= WITH_RECORDIMPORT }

}
