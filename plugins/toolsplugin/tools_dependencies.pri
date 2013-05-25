include($${SOURCES_PLUGINS_PATH}/coreplugin/coreplugin.pri)

# PatientBase plugin is needed by the HPRIM integrator (in order to find patients uuid from their names/dob)
include($${SOURCES_PLUGINS_PATH}/patientbaseplugin/patientbaseplugin.pri)

QT *= xml
