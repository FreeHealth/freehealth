### ![FreeMedForms logo](img/logo.png)The FreeMedForms project <http://www.freemedforms.com>

### Free DDI Manager app

Create and use your own set of drug interactions

* Eric Maeker, MD (Fr)
  * creator, main developer
* Jérôme Pinguet (Fr)
  * today's speaker, contributor
* The FreeMedForms team
- - -
####LSM / RMLL 2014, Montpellier
 
 
 
- - -
- - -
- - - 

### Notes

Hello,

Please, let me begin  with a very special thanks and congratulations to the organizers of this event and for their work. I'm really impressed by what benevolent women and men have done for this Libre Software Meeting.  I would also like to thank them for allowing me to present our work today. 

Eric Maeker, french physician, C + + / Qt developer, is the creator and main developer of FreeMedForms. The project started from scratch in 2008 and is now usable in day-to-day practice.  FreeMedForms project applications are now in production in medical offices.

You can find more information about the FreeMedForms project on our web site: http://www.freemedforms.com/

Today I will present FreeDDIManager, our drug interactions manager. It allows you to create and use your own set of interactions within the FreeMedForms applications.

!

### ![FreeMedForms logo](img/logo.png) The FreeMedForms project

* a suite of medical applications
  * Electronic Medical Record
  * prescription assistant (Computerized Physician Order Entry)
  * drug interactions checker
  * other coding assistants
* Free Software
* License GPLv3
- - -
- - -
- - -
###Notes
FreeMedForms is a suite of medical applications regrouping an Electronic Medical Record manager, a prescriber, some clinical coding assistants. More apps are on the way.
All applications are free, open source and released under the GPLv3 licence.

!

### ![FreeMedForms logo](img/logo.png) The FreeMedForms community
* not-for-profit community
  * medical doctors
  * computer workers/scientists
  * pharmacist
* about 50 members (10 active members)
* supported by OpenSuse Medical, Debian Med, Fedora Medical, Arch Linux
* collaboration with free software EMR projects: GNUmed, FreeMed, Care2X, SynapseEMR
* totally independent
 * from pharmaceutical industry
 * from proprietary software industry
* declarations of interests of active members available on website
- - -
- - -
- - -

###Notes
The project is driven by a community of medical doctors and computer workers and was created on the pulse of a french team.

All applications are free, open source and released under the GPLv3 licence.

The project is supported by the Debian Med team and also the FedoraMedical and OpenSuse Medical teams.

!

### ![FreeMedForms logo](img/logo.png) The FreeMedForms project

* GNU/Linux, MacOs X, Win32, Win64, (FreeBSD)
* C++ using Digia's Qt libs
* documented using Doxygen
* massive usage of MVC architecture
* Built using dynamic libraries with plugins architecture
* database: SQLite and/or MySQL

- - -
- - -
- - -

### Notes
The project is coded in C++ with Qt libraries and a massive usage of the model/view/controller architecture. The code is documented using Doxygen.

Applications are built with shared libraries and plugins. They are available for Linux, Mac, and Windows. The FreeBSD port should require some code adaptations.

The roadmap and the issue reporting are available on our wiki web site and Google Code.

We are using SQLite and/or MySQL (MariaDB) for the databases.

!

### ![FreeMedForms logo](img/logo.png) Classifications to understand drug interactions

Some useful and sometimes problematic classifications  

* Drugs
 * ATC Anatomical Therapeutical classification
 * INN International Non-Proprietary Name

* Diseases
 * ICD10 : International Classification of Diseases version 10

- - -
- - -
- - -

### Notes
?Eric why do you say problematic?
our software inclusion/exclusion dag/star Asterisque prend en charge l'intégralité du cahier des charges de la CIM10
!

### ![FreeMedForms logo](img/logo.png) What is a drug interaction?
* a drug can interact with:
 * another drug: __drug-drug interaction__ __DDI__ 
 * patient's conditions:
  * disease, allergy, renal filtration, age
* food: __food-drug interactions__ __FDI__
* lab-drug interactions when interacts with lab tests
* Potentially Inappropriate Medications __PIM__ specific interactions regarding elderly

- - -
- - -
- - -
- - -


###Notes

Here are some definition and abbreviations widely used

Drug can interact with multiple source:
- it can interact with another drug creating a DDI.

??Shouldn't we use drug/disease & drug/allergy??
And reverse lab-drug to drug-lab interaction ?
Problem: drug-lab interaction is used in many articles with the meaning of "attempt to prescribe a molecule contraindicated in renal insufficiency and recent low clearance lab test result --> alert" and the way you present it might refer to "drugs that have an effect on laboratory tests such as increasing or lowering K+" or even "drug interaction with chemical reactant used in a lab test procedure"

- or with some patient's physiologic states creating drug allergies, intolerances. But it can also interact with patient's chronic or acute diseases, his renal ability to filter and to secrete the drug. 
- Also so drug should not be used with elderlies (old people) we call these : PIM
- finally drug can interact with food (and especially with alcohol)

A drug interaction can be enough to cause severe and even fatal adverse effects. As more drugs are added to the list each year, the number of possible interactions is quickly growing: computerized processing of interactions is a much needed tool to help medical practitioners.

We use these international classifications:
- ICDv10 licensed by the World Health Organization to classify diseases
- ATC to classify drugs. The ATC defines, for some drugs, their INN.

!

### ![FreeMedForms logo](img/logo.png) Drug-drug interactions
* DDI can be enough to create severe even fatal adverse events
* __Always growing__
 * Number of marketed drug
 * Number of known drug interactions
 * Number of prescribed drugs
 * Preventive treatment
* __Older people are at increased risk__
 * many diseases means many therapies
 * many diseases means altered physiology

- - -
- - -
- - -
- - -
###Notes

??Use elderly persons instead of older people??
When FreeMedForms was started a special attention was paid to DDI management. We identified many sources freely available and compiled them into one unique database.

We also found governemental drug databases that are compiled in the same database (using our tool FreeToolBox). We have processed databses from France, USA, Canada, Belgium, South Africa, Portugal and Brazil. Some of them are still in developement
??Which ones are still in development??

This work was the starting point of FreeDiams. FreeDiams is the FreeMedForms prescriber built as a standalone application. FreeDiams can communicate with external EMRs through XML exchange files.

!

### ![FreeMedForms logo](img/logo.png) Drug Interactions alert system

![slide 7 drug interactions alert system](img/slide7-drug-interactions-alert-system.png)
- - -
- - -
- - -
###Notes
The interaction engine analyzes all combinations of drugs per couple. Each molecular component is analyzed after being linked to an internationally recognized and persistent over time coding system called ATC (which means Anatomical Therapeutic Chemical). The ATC classification is owned by the World Health Organization.

The DDI engine calculates interactions in real time. Each time a drug is added or removed of the prescription, all interactions are recalculated. The process is done twice when prescribing a cure, I'll explain this later.

If the tool is effective in detecting interactions, it must signal them so that practitioners adhere to the warnings. Otherwise the tool is useless. Alert fatigue is a well known effect of redundant, clinically irrelevant, or even non significant alerts. Practitioners suffering from this syndrome will override all alerts whatever their content and level of risk.

So we had to define a strong alert philosophy.

!

### ![FreeMedForms logo](img/logo.png) Timing of the alert system

![slide 8 timing of the alert system](img/slide8-timing-of-the-alert-system.png)

- - -
- - -
- - -

###Notes
The timing of the alert system is really important too.

Practitioner may have to be warned of interactions before he selects drugs, during the selection and when the dosing of the drug is completed.

This short diagram shows the different phases of the alert computation and their emergence.

!

### ![FreeMedForms logo](img/logo.png) Free DDI Manager
* __Motivation__
 * Allow users to __create__ their own drug interaction set
 * Allow users to __share__ their own drug interaction set
 * Facilitate the contribution process
* __Current status__
 * Beta version, some features still missing, but mainly usable
 * All FreeMedForms drug databases are created by Free DDI Manager
* Future developments
 * Include javascript in drug interaction management (eg anticoagulant management: a lab-drug interaction model)
 * create a server to facilitate data sharing between teams

- - -
- - -
- - -
###Notes
?? can we break this slide into 3 slides ??

!

### ![FreeMedForms logo](img/logo.png) Free DDI Manager workflow: drug database

* This part allow users to prepare any drug database
* Get drug database raw source
 * France
 * USA
 * Canada
 * South-africa
 * Belgium
* Transform raw source into computable source
* Extract all drug components

- - -
- - -
- - -
###Notes

!

### ![FreeMedForms logo](img/logo.png) Free DDI Manager workflow: prepare drug recognization
* Edit ATC classification (not required)
* Edit interactors
 * Can be ATC codes
 * Or you can create your own (even class of components)
* Edit all drug components
 * Link with an ATC code
 * Check and review, comment
* data can be shared between users
- - -
- - -
- - -
- - -
###Notes

!

### ![FreeMedForms logo](img/logo.png) Free DDI Manager workflow: create a DDI
First – second interacting interactor

- - -
- - -
- - -
###Notes

!

### ![FreeMedForms logo](img/logo.png) FreeDDIManager – Futur devs
Ajout scripts pour protocoles thérapeutiques ou
process d'alerte à des moments donnés de la
prescription

- - -
- - -
- - -
###Notes

!

### ![FreeMedForms logo](img/logo.png) Free DDI Manager workflow: manage datapacks

* Create a datapack
* Upload a datapack
* Share a datapack

- - -
- - -
- - -
###Notes

!

### ![FreeMedForms logo](img/logo.png) Alert management

####Alert philosophy

- - -
- - -
- - -
###Notes

It is always difficult to alert medical practitioners using a software.

Many alerts are overriden or totally ignored.

We tried to build a performant system, fully configurable to avoid this problem.

!

### ![FreeMedForms logo](img/logo.png) Alert philosophy

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

- - -
- - -
- - -

###Notes

We defined two systems of alert: one blocking and one non-blocking.

The non-blocking alert is represented by and icon and tooltips in views. While the blocking alerts are blocking dialogs that needs user validation to proceed. Both alerts present minimal and sufficient information. At any time users can access (to) full alert information.

The alert system provides three levels of risk : low medium and high. And each alert can be classified in an inifinite number of categories.

The threshold of each type of alert can be adapted by the user in the user preferences.

###References

* Phansalkar, S., J. Edworthy, E. Hellier, D. L. Seger, A. Schedlbauer, A. J. Avery, and D. W. Bates. “A Review of Human Factors Principles for the Design and Implementation of Medication Safety Alerts in Clinical Information Systems.” Journal of the American Medical Informatics Association 17, no. 5 (September 6, 2010): 493–501. doi:10.1136/jamia.2010.005264.

!

### ![FreeMedForms logo](img/logo.png) Alert philosophy
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

- - -
- - -
- - -
###Notes
Alerts can be related to different types of attendees.

They can be cycling or not, with a validity period defined or not.

The validation method can include a user comment in case of the user override the alert.

Alerts can be dynamic using their script system.

!

### ![FreeMedForms logo](img/logo.png) Alert packs
* Alerts can be shared via __alert packs__
* Alert packs are shared via the __datapack manager__
* Alert packs must contain
 * packdescription.xml (datapack description file)
 * All XML alerts
* When the user install an alert pack
 * All alerts are saved
 * All alerts are checked
 * All alerts are executed (if required)
*This is really useful to share medical prevention alerts

- - -
- - -
- - -
###Notes
Alerts can be shared between users or group of users through our datapack server. Alerts are then grouped into alert-packs.

A full documentation is about to be released on our main web site.

!

### ![FreeMedForms logo](img/logo.png) FreeMedForms needs you!
* A great community
* A big project
* __Wanted__: devs, testers, translators, medical doctors, pharmacists, articles, reviews, retweets
* Unfunded... __$__, __€__, __฿__ welcome by the registered not-for-profit association
* Code on GitHub <https://github.com/FreeMedForms/freemedforms>
* Web <http://www.freemedforms.com>
* Mailing List <freemedforms@googlegroups.com>

- - -
- - -
- - -
###Notes
??Should we talk about the association??i

!

### ![FreeMedForms logo](img/logo.png)Thank you!

* Thank you for listening
* Eric says "Kisses to my wife and children (who support my devs...)"
* Jerome says: "Thank you Eric for coding so fast and so well, thanks to the international FMF community for your contributions and for sticking with us despite our bad english! :)"

!

### ![FreeMedForms logo](img/logo.png)Interactions... with you!
##We would like to hear from you now!
* mistakes?
* critics?
* questions?
* contributions?

###I'll be around today & available for discussion or meeting: Email/XMPP/Jabber/GPG: jerome@jerome.cc
