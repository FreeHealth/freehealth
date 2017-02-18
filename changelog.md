Changelog
=========

See https://github.com/FreeHealth/freehealth/issues for more details


0.9.10
------

### Database
  * Database fmf_pmh: CREATIONDATETIME field added to table PMH_MASTER

0.9.9
-----

### Summary

FreeHealth is a fork of FreeMedForms. You can upgrade automatically from
FreeMedForms EMR 0.9.8 to FreeHealth EHR 0.9.9 just by installing FreeHealth 0.9.9

Several bug fixes, somes new feature, many enhancements.

### New features
  * forms: combo items can be listed in alphabetical order
  * forms: embed prescription widget inside a form with ui file
  * added administrative gender: Unknown
  * episodes: differentiate episode date and time (can be modified by user) and episode
  creation date and time (timestamp, cannot be modified)
  * problem oriented medical record: BMI calculator

### Enhancements
  * improved print preview
  * improved PMHx dialog:
    * you can now add comment text
    * empty items don't appear in tree view
    * display name of PMH entry creator 
  * administrative gender: "hermaphrodite" becomes "Other"
                                                                                
### Bug
  * PMHx modifications and deletions are now correctly recorded
  * FreeRx: GUI of frequency of administration https://github.com/FreeMedForms/freemedforms/issues/152
  * forms: translation of measurement widgets
