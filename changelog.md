Changelog
=========

See https://github.com/FreeHealth/freehealth/issues for more details


0.10.0
------

### Summary

  * Several bug fixes, somes new features, many enhancements.
  * Requires MySQL 5.7 or MariaDB 10.1
  * Last version supporting SQLite

### New features
  * All phone numbers visible direclty under the patients' list
  * All patient phone numbers can be added during patient creation
  * User phone numbers can be edited inside user manager
  * New MedForm option: NoHeader
  * Patient date of birth appears on main screen
  * PMHx: date of creation and identity of creator are now visible

### Database
  * Database fmf_pmh: CREATIONDATETIME field added to table PMH_MASTER
  * Database fmf_patients: in table PATIENTS_IDENTITY, fields MOBILE_PHONE and
  WORK_PHONE added, new versionning table added: SCHEMA_CHANGES, old versionning
  table VERSION deleted, see https://github.com/FreeHealth/freehealth/issues/57

### Bugs fixed
Here are the 19 issues that were closed since the release of FreeHealth 0.9.0:

https://github.com/FreeHealth/freehealth/issues?utf8=%E2%9C%93&q=is%3Aissue%20closed%3A%222016-09-03%20..%202017-04-14%22%20

0.9.9
-----

### Summary

FreeHealth 0.9.9 is a fork of FreeMedForms. You can upgrade automatically from
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
                                                                                
### Bugs fixed
  * PMHx modifications and deletions are now correctly recorded
  * FreeRx: GUI of frequency of administration https://github.com/FreeMedForms/freemedforms/issues/152
  * forms: translation of measurement widgets
  * Episodes: UserDate, CreationDate, timestamp
  * User manager: title changed not processed and saved correctly
  * fmf_admin cannot create new user
  * accessibility features of Qt
  * GUI frequency of administration labels not entirely visible
  * Duration value widget need not be doublespinbox
