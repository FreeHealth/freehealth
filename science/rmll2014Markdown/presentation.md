### ![FreeMedForms logo](img/logofmf.png)The FreeMedForms project <http://www.freemedforms.com>
###![Free DDI Manager logo](img/logoddi-big.png) Free DDI Manager app
Create and use your own set of drug interactions

* Eric Maeker, MD (Fr)
  * creator, main developer
* Jérôme Pinguet (Fr)
  * today's speaker, contributor
* The FreeMedForms team
####LSM / RMLL Montpellier 2014
![footer](img/960px-footer.png)

!

### ![FreeMedForms logo](img/logoddi.png) The FreeMedForms project

* a suite of medical applications
  * Electronic Medical Record
  * prescription assistant (Computerized Physician Order Entry)
  * drug interactions checker
  * other coding assistants
* Free Software
* License GPLv3

!

### ![FreeMedForms logo](img/logoddi.png) The FreeMedForms community
* not-for-profit community
  * medical doctors
  * computer workers/scientists
  * pharmacist
* about 50 members (10 active members)
* from France, Canada, Germany, Austria, Australia, New-Zealand 
* supported by OpenSuse Medical, Debian Med, Fedora Medical, Arch Linux
* collaboration with free software EMR projects: GNUmed, FreeMed, Care2X, SynapseEMR
* totally independent
 * from pharmaceutical industry
 * from proprietary software industry
* declarations of interests of active members available on website

!

### ![FreeMedForms logo](img/logoddi.png) The FreeMedForms project

* __GNU/Linux, FreeBSD, MacOs X, Win32, Win64__
* C++ using Digia's __Qt__ libs
* documented using Doxygen
* massive usage of MVC architecture and __unit-tests__
* Built using dynamic libraries with __plugins architecture__
* database: SQLite and/or MySQL/MariaDb

!

### ![FreeMedForms logo](img/logoddi.png) Classifications to understand drug interactions

Some useful and sometimes problematic classifications  

* Drugs
 * ATC Anatomical Therapeutical Chemical classification
 * INN International Non-Proprietary Name

* Diseases
 * ICD10 : International Classification of Diseases version 10

!

### ![FreeMedForms logo](img/logoddi.png) What is a drug interaction?

A drug can interact with  

* another drug: __drug-drug interaction__ (__DDI__) 
* patient's conditions: __patient-drug interaction__
 * acute or chronic disease
 * allergy, intolerance
 * renal filtration
 * age
* food & alcohol: __food-drug interactions__ (__FDI__)
* labtest-drug interactions: drug efficiency monitored by lab tests
* Potentially Inappropriate Medications __PIM__ specific interactions regarding elderly

!

### ![FreeMedForms logo](img/logoddi.png) Drug interactions

__Drug interactions create severe even fatal adverse events__

* __Always growing__
 * Number of marketed drugs
 * Number of known drugs interactions
 * Number of prescribed drugs
 * Preventive treatment
* __Elderly are at increased risk__
 * many diseases means many therapies
 * many diseases means altered physiology

!

### ![FreeMedForms logo](img/logoddi.png) Drug Interactions alert system

![slide 7 drug interactions alert system](img/slide7-drug-interactions-alert-system.png)

!

### ![FreeMedForms logo](img/logoddi.png) Timing of the alert system

![slide 8 timing of the alert system](img/slide8-timing-of-the-alert-system.png)

!

### ![FreeMedForms logo](img/logoddi.png) Free DDI Manager

* __Motivation__
 * Allow users to __create__ their own drug interaction set
 * Allow users to __share__ their own drug interaction set
 * Facilitate the contribution process

* __Current status__
 * Beta version, some features still missing, but mainly usable
 * All FreeMedForms drug databases are created by Free DDI Manager

* __Future developments__
 * Include JavaScript inside DDI definitions  
 -> eg anticoagulant management with AVK Plugin: a lab-drug interaction model)  
 -> see roadmap of AVK Plugin for details  
 * create a server to facilitate data sharing

!

### ![FreeMedForms logo](img/logoddi.png) Free DDI Manager workflow: drug database

This part allows users to prepare any drug database

* Get drug database raw source
 * France
 * USA
 * Canada
 * South-africa
 * Belgium
* Transform raw source into computable source
* Extract all drug components

!

### ![FreeMedForms logo](img/logoddi.png) Free DDI Manager workflow: prepare drug recognization

* Edit ATC classification (not required)
* Edit interactors
 * Interactors database is independant of the drug database
 * 1 interactor can be 1 ATC code, drug component, class/group of ATC/interactors/classes
* Edit all drug components
 * Link with an interactor or ATC code (partially by hand)
 * Check, review, comments: done by hand
* data can be shared between users

!

### ![FreeMedForms logo](img/logoddi.png) Free DDI Manager workflow: create a DDI

* Drug-drug interactions are currently limited to 2 interactors interactions
* DDI are defined using a risk model, a preventive/management message and bibliographic references from PubMed
* DDI must be checked and reviewed by hand
* FreeMedForms community wants to create a free and open source DDI database usable in any country
* We created an «Interactions Committee»
* Legal and medico-legal issues are not yet clearly defined and solved.

!

### ![FreeMedForms logo](img/logoddi.png) Interactions Committee

* Shah et al. 2006 [2]: "high user acceptance of ambulatory computerized prescribing alerts when using a selective knowledge base and minimizing workflow interruptions"
* commercial knowledge bases
 * highly inclusive
 * emphasis on breadth of coverage, not on clinical relevance or severity of adverse events
 * conflicts of interests: often directly or indirectly owned by pharmaceutical industry
* FMF Interactions Committee
 * review interactions
 * classify: relevance to setting, severity
 * Evidence Based Interactions: references (PubMed), levels of evidence
 * at first focused on outpatient setting
 * lobby academic, governmental, institutional interactions committees:  
   -> publish under free licenses!  
   -> use & contribute to Free DDI Manager!

!

### ![FreeMedForms logo](img/logoddi.png) Free DDI Manager workflow: manage datapacks

* Create a datapack
* Upload a datapack
* Share a datapack

!


### ![FreeMedForms logo](img/logoddi.png) Alert philosophy[1]

* Two kinds of alerts:
 * __Non-blocking__ → icon and tooltip
 * __Blocking__ → blocking dialog, require user action
* Alert system provides three levels: low, medium, high
* And an infinite number of __categories__ (DDI, PIM, drug
duplication, dose checking, allergies, intolerances...)
* __Threshold__ of alerts can be adapted by the user for each
type of alerts
* Alerts can be __dynamic__ (using scripting)
* We do our best to avoid alert fatigue

!


### ![FreeMedForms logo](img/logoddi.png) Alert philosophy

* __related to__
 * One or all users
 * Application (update notification, backup reminder)
 * One patient
 * All patients (eg: for medical prevention, cancer screening)
* « __one shot alerts__ » or « __cycling alerts__ »
* defined validity period
* edited by user or not
* __validated__ / __overriden__ & __commented__ by user
* scripted

!

### ![FreeMedForms logo](img/logoddi.png) Alert packs
* Alerts can be shared via __alert packs__
* Alert packs are shared via the __datapack manager__
* Alert packs must contain
 * packdescription.xml (datapack description file)
 * All XML alerts
* When the user install an alert pack
 * All alerts are saved
 * All alerts are checked
 * All alerts are executed (if required)
* This is really useful to share medical prevention alerts

!

### ![FreeMedForms logo](img/logoddi.png) FreeMedForms needs you!
* A great community
* Non-profit association -> official support
* __Wanted__: devs, testers, translators, medical doctors, pharmacists, articles, reviews, retweets
* Unfunded... __$__, __€__, __฿__ welcome by the registered not-for-profit association
* Code on GitHub <https://github.com/FreeMedForms/freemedforms>
* Web <http://www.freemedforms.com>
* Mailing List <freemedforms@googlegroups.com>

!

### ![FreeMedForms logo](img/logoddi.png)References

* [1] Phansalkar, S., J. Edworthy, E. Hellier, D. L. Seger, A. Schedlbauer, A. J. Avery, and D. W. Bates. “A Review of Human Factors Principles for the Design and Implementation of Medication Safety Alerts in Clinical Information Systems.” Journal of the American Medical Informatics Association 17, no. 5 (September 6, 2010): 493–501. doi:10.1136/jamia.2010.005264
* [2] Shah, Nidhi R, Andrew C Seger, Diane L Seger, Julie M Fiskio, Gilad J Kuperman, Barry Blumenfeld, Elaine G Recklet, David W Bates, and Tejal K Gandhi. “Improving Acceptance of Computerized Prescribing Alerts in Ambulatory Care.” Journal of the American Medical Informatics Association: JAMIA 13, no. 1 (February 2006): 5–11. doi:10.1197/jamia.M1868.
* FreeMedForms complete bibliograpical references freely available on our Zotero group: https://www.zotero.org/groups/freemedforms

!


### ![FreeMedForms logo](img/logoddi.png)Thank you!

* Thank you for your attention
* Eric says "Kisses to my wife and children (who support my devs...)"
* Jerome says: "Thank you Eric for coding so fast & well, thanks to the FMF community for your contributions and for sticking with us despite our bad english! :)"

Now I will present a short video of the FreeDDIManager. You will see how easy it is to build and share your own set of interactions.

####I'll be around later today (__after__ rms conf) & available for discussion or meeting
* Email/XMPP/Jabber/GPG: jerome@jerome.cc
* mobile +33 6 11 77 02 69
