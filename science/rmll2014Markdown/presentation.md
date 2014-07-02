### ![FreeMedForms logo](img/logo.png) www.freemedforms.com

#### The FreeMedForms project

#### Free DDI Manager app

Créez et utilisez votre propre jeu d'interactions médicamenteuses

Creating and using your own set of drug interactions

* Eric Maeker, MD (Fr)
  * creator, main developper
* Jérôme Pinguet (Fr)
  * speaker
* The FreeMedForms team
* LSM / RMLL 2014, Montpellier
 
 
 
 
* * * 
* * * 
* * *

### Notes

Hello,

Please, let me begin  with a very special thanks and congratulations to the organizers of this event and for their work. I'm really impressed by what benevolent women and men have done for this Libre Software Meeting.  I would also like to thank them for allowing me to present our work today. 

Eric Maeker, french physician, C + + / Qt developer, is the creator and main developer of FreeMedForms. The project started from scratch in 2008 from scratch and is now usable in day-to-day practice.  FreeMedForms' project applications are now in production in some medical offices.

You can find all information about the FreeMedForms project on our web site: http://www.freemedforms.com/

Today I will present FreeDDIManager, our drug interaction manager. It allows you to create and use your own set of interactions within the FreeMedForms applications.

!

### ![FreeMedForms logo](img/logo.png) The FreeMedForms project

* suite of medical applications
  * Electronic Medical Record
  * prescription assistant
  * drug interactions checker
  * other coding assistants
* by a not-for-profit community
  * medical doctors
  * computer workers/scientists
* about 50 members (10 active members)
* supported by OpenSuse Medical, Debian Med, Fedora Medical, Arch Linux...
* collaboration with free softwarehealth projects: GNUmed, FreeMed, Care2X, SynapseEMR

* * *
* * *
* * *

###Notes
FreeMedForms is a suite of medical applications regrouping an Electronic Medical Record manager, a prescriber, some clinical coding assistants and many little utilities that are on the way.

The project is driven by a community of medical doctors and computer workers and was created on the pulse of a french team.

All applications of this project are free, open source and released under the GPLv3 licence.

The project is supported by the Debian Med team and also the FedoraMedical and OpenSuse Medical teams.

!

### ![FreeMedForms logo](img/logo.png) The FreeMedForms project

* GNU/Linux, MacOsX, Win32, Win64, (FreeBSD)
* C++ using Digia's Qt libs
* documented using Doxygen
* massive usage of MVC architecture
* Built using dynamic libraries with plugins architecture
* database: SQLite and/or MySQL

* * *
* * *
* * *

###Notes
The project is coded in C++ with Qt libraries and a massive usage of the model/view/controller architecture. The code is documented using Doxygen.

Applications are built with shared libraries and plugins. They are available for Linux, Mac, and Windows. The FreeBSD port should require some code adaptations.

The roadmap and the issue reporting are available on our wiki web site and Google Code.

We are using SQLite and/or MySQL (MariaDB) for the databases.

!

Some classifications to
understand drug interaction
●
●
●
Some usefull and sometimes problematic
classifications
For drugs
● ATC : anatomical therapeutical classification
– INN == international non-proprietary name
For diseases
● ICD10 : International classification of diseases
version 10
● Others exist but are actually not used within our
project
http://www.freemedforms.com/
RMLL 2014
4
What is a drug interaction ?
●
Some short definitions, a drug can interact with:
● drug-drug interaction (DDI) when interacts with
another drug
● drug-patient interaction when interacts with
patient's conditions and diseases (allergies, diseases,
renal filtration, age...)
● food-drug interactions (FDI) when interacts with
food
● lab-drug interactions when interacts with a lab tests
● potentially inappropriate medications (PIM)
specific interactions regarding elderly
● Some other items that are not listed here...
http://www.freemedforms.com/
RMLL 2014
5
Drug-drug interactions
●
●
●
DDI can be enough to create severe even fatal
adverse effects
Always growing
● Number of marketed drug
● Number of known drug interactions
● Number of prescribed drugs
● Preventive treatment
Older people are at increased risk:
● many diseases means many therapies
● many diseases means altered physiology
http://www.freemedforms.com/
RMLL 2014
6
The drug Interactions alert
system
Drug 1
ATC
Active substance 1.1
Active substance 1.2
Active substance 1.3
C
AT
To
DDI Engine
Drug Database
Blocking
REAL TIME
Active substance 2.1
Active substance 2.2
Active substance 2.3
Drug 2
To
AT
C
ATC
http://www.freemedforms.com/
RMLL 2014
Non blocking
!
7
Fatigue Alert
Timing of the alert
system
Searching drugs
Drug selection
Dosage validation
Dose processing
Time
Drugs allergies
Drugs intolerances
Processed
Noticed
* DDI & PIM not dose-related:
processed
* DDI & PIM dose-related:
noticed in some special cases or
if prescription is only a selection
of drugs (w/o dosing)
* Management of route
DDI & PIM dose-related
processed and noticed
http://www.freemedforms.com/
RMLL 2014
Blocking alert
And Alert processing
8
Informative alerts
FreeDDIManager
●
●
●
Motivation
●
Allow users to create their own drug interaction set
●
Allow users to share their own drug interaction set
●
Ease the contribution process
Current status
●
Beta version, some features still missing, but mainly usable
●
All FreeMedForms drug database are created by
FreeDDIManager
Futur developments
●
Include javascript in drug interaction management (eg VKA
management : a lab-drug interaction model)
●
Create a server to ease the data sharing between teams
http://www.freemedforms.com/
RMLL 2014
9
FreeDDIManager – Workflow
Drugs database part
● This part allow users to prepare any drug database
● Get drug database raw source
●
●
●
●
●
●
●
France
USA
Canada
South-africa
Belgium
Transform raw source into computable source
Extract all drug components
http://www.freemedforms.com/
RMLL 2014
10
FreeDDIManager – Workflow
Preparing drug recognization
● Edit ATC classification (not required)
● Edit interactors
●
●
●
Edit all drug components
●
●
●
Can be ATC codes
Or you can create your own (even class of components)
Link with an ATC code
Check and review, comment
These data can be shared between users
http://www.freemedforms.com/
RMLL 2014
11
FreeDDIManager – Create a
DDI
First – second interacting interactor
http://www.freemedforms.com/
RMLL 2014
12
FreeDDIManager – Futur devs
●
Ajout scripts pour protocoles thérapeutiques ou
process d'alerte à des moments données de la
prescription
http://www.freemedforms.com/
RMLL 2014
13
FreeDDIManager – workflow
Managing datapacks
●
●
●
Créer un datapack
Uploader un datapack
Partager un datapack
http://www.freemedforms.com/
RMLL 2014
14
Alert management
http://www.freemedforms.com/
RMLL 2014
15
Alert philosophy [9]
●
●
●
●
●
●
Two kind of alerts: non-blocking and blocking alerts
● Non-blocking → icons and tooltips
● Blocking → blocking dialogs
Alert system provides three levels: low, medium, high;
And an infinite number of categories (DDI, PIM, drug
duplication, dose checking, allergies, intolerances...)
Threshold of alerts can be adapted by the user for each
type of alerts
Alerts can be dynamic (using scripting)
Try a maximum to avoid alert fatigue
[9]: J Am Med Inform Assoc. 2010 Sep-Oct;17(5):493-501.
http://www.freemedforms.com/
RMLL 2014
16
Alert philosophy
●
●
●
●
●
●
Can be related to
● One or all users
● Application (update notification, backup reminder...)
● One patient
● All patients (eg: for medical prevention, cancer
screening...)
Can be « one shot alerts » or « cycling alerts »
Can have a defined validity period
Can be edited by user or not
Can be validated/overriden & commented by the user
Can be scripted
http://www.freemedforms.com/
RMLL 2014
17
Alert packs
●
●
●
●
●
Alerts can be shared via alert packs
Alert packs are shared via the datapack manager
Alert packs must contains
● packdescription.xml (datapack description file)
● All XML alerts
When the user install an alert pack
● All alerts are saved
● All alerts are checked
● All alerts are executed (if required)
This is really useful to share medical prevention alerts
http://www.freemedforms.com/
RMLL 2014
18
http://www.freemedforms.com/
RMLL 2014
19
http://www.freemedforms.com/
RMLL 2014
20
FreeMedForms
Needs you !
●
●
●
●
FreeMedForms is a great and big project
Actually unfunded
Need devs, testers, translators, MD,
pharmacists... (and funds)
Code: GitHub
https://github.com/FreeMedForms/freemedforms
●
●
●
●
Web site: http://www.freemedforms.com/
Mailing: freemedforms@googlegroups.com
Kisses to my wife and children (who support my devs...)
Thanks, The FreeMedForms Team
http://www.freemedforms.com/
RMLL 2014
21

