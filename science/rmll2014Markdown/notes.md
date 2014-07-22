### 1 Notes

Hello,

Please, let me begin  with a very special thanks and congratulations to the organizers of this event and for their work. I'm really impressed by what benevolent women and men have done for this Libre Software Meeting.  I would also like to thank them for allowing us to present our work today.

I'm Jérôme Pinguet. I used FreeMedForms for a few months in a medical practice. My previous EMR was not up-to-date and didn't have a Drug-Drug Interactions checker. I'm an active contributor to the project. I'm currently writing my MD thesis about the efficiency of clinical decision support systems to reduce anticoagulant related adverse events.

Eric Maeker is a french physician, C + + / Qt developer, and is the creator and main developer of FreeMedForms.

You can find more information about the FreeMedForms project on our web site: http://www.freemedforms.com/

Today I will present FreeDDIManager, our drug interactions manager. It allows you to create and use your own set of interactions within the FreeMedForms applications.

- - -

### 2 Notes

FreeMedForms is a suite of medical applications regrouping an Electronic Medical Record manager, a prescriber assistant, some clinical coding assistants. More apps are on the way.
All applications are free, open source and released under the GPLv3 licence.

- - -

### 3 Notes

The project is driven by a community of medical doctors and computer workers and was created on the pulse of a french team.

All contributor must declare his/her conflict of interests. Contributors that are not totally independant of the pharmaceutical industry and/or the proprietary socftware industry will not be able to join our team.

All the FreeMedForms community want to thank the Debian Med team that supports our project. Thanks to this help, FreeMedForms is fully available in the official stable repository since a long time. Eric is now an active member of the Debian Med team.

The project is also supported by the FedoraMedical and OpenSuse Medical teams.

- - -

### 4 Notes

All applications are available for GNU/Linux, Mac, FreeBSD and Windows.

The project is coded in C++ with Qt libraries and a massive usage of the model/view/controller architecture. The code is documented using Doxygen.

__Many unit-tests are available and code safety is regularly checked before any release__.

Applications are built using shared libraries and plugins architecture which allow us to easily add/remove new functionalities.

The roadmap and the issue reporting are available on our wiki web site and Google Code / GitHub.

We are using SQLite and/or MySQL (MariaDB) for the databases.

- - - 

### 5 Notes

The FreeMedForms project can manage some internationally defined classifications :

* For drugs we use the ATC which is copyrighted and owned by WHO. It defines INN for drug component and/or drugs (association for eg).
* For disease, we currently use the ICD version 10. Our implementation of the ICD10 is respectfull of thecomplete specifications of this classification like dag/star coding, diagnosis inclusion/exclusion.

- - -

### 6 Notes

Here are some definition and abbreviations widely used

Drug can interact with multiple source:
- it can interact with another drug creating a DDI.
- it can also interacts with patient condition like drug allergy or intolerance, chronic or acute active diseases, age, renal filtration, weight, body surface...
- some drugs must be screened using labtests (like INR / AVK, digoxin...) defining labtest-drug interaction
- some drugs should not be used or be used with caution with elderlies (old people) defining PIM

??Shouldn't we use drug/disease & drug/allergy??
And reverse lab-drug to drug-lab interaction ?
Problem: drug-lab interaction is used in many articles with the meaning of "attempt to prescribe a molecule contraindicated in renal insufficiency and recent low clearance lab test result --> alert" and the way you present it might refer to "drugs that have an effect on laboratory tests such as increasing or lowering K+" or even "drug interaction with chemical reactant used in a lab test procedure"

- - -

### 7 Notes

A drug interaction can be enough to cause severe and even fatal adverse effects.

The number of marketted drugs is always growing, as the number of prescribed drugs per patient. Our knowledge of the potential interactions is also each year more acute. The number of possible interactions is quickly growing: computerized processing of interactions is a much needed tool to help practitioners in their day-to-day practice.

When FreeMedForms was started a special attention was paid to DDI management. We identified non-conflicting sources freely available and compiled them into one unique database.

This work was the starting point of FreeDiams in 2008. FreeDiams is the FreeMedForms prescriber built as a standalone application. FreeDiams can communicate with external EMRs through XML exchange files.

- - -

### 8 Notes

The interaction engine analyzes all combinations of drugs per couple. Each molecular component is analyzed after being linked to an internationally recognized and persistent over time coding system called ATC (which means Anatomical Therapeutic Chemical). The ATC classification is owned by the World Health Organization.

The DDI engine calculates interactions in real time. Each time a drug is added or removed of the prescription, all interactions are recalculated. The process is done twice when prescribing a cure, I'll explain this later.

If the tool is effective in detecting interactions, it must signal them so that practitioners adhere to the warnings. Otherwise the tool is useless. Alert fatigue is a well known effect of redundant, clinically irrelevant, or even non significant alerts. Practitioners suffering from this syndrome will override all alerts whatever their content and level of risk.

So we had to define a strong alert philosophy.

- - -

### 9 Notes

The timing of the alert system is really important too.

Practitioner may have to be warned of interactions before he selects drugs, during the selection and when the dosing of the drug is completed.

This short diagram shows the different phases of the alert computation and their emergence.

- - -

### 9 Notes

We want to ease the creation, the contributions and sharing of DDI very easy inside the FreeMedForms project. Some tools were tested but none was really efficient.
The FreeDDIManager was created to answer to this need. We chose to write a hard-coded application instead of a webportal as all our applications are still hard-coded. There are roadmap to swich to a webapp design.
Currently, FreeDDIManager is in beta stage. We already use it to create and update our drugs related datapack server.

In a near future, we plan to include javascript inside DDI definition to allow users to access to any other database, to access patient data and interact with user.

- - -

### 10 Notes

In order to create our drug database, we found free drugs databases published by governmental structures for the folowing countries : France, USA, Canada, South-Africa, belgium and portugal. These drug databases can be automatically processed using FreeDDIManager without any user configuration.
Raw source are processed for the needs of the FreeMedForms database scheme.

- - - 

### 11 Notes

The workflow of drug database creation includes needs:

* a full ATC classification (that will be released with the release version)
* a definition of interactors. An interactor can be a unique ATC or multiple ATC codes, it can be a component name, or a groupment of interactors.
* You will also need all the drug components of your database and you will need to link them to an ATC code or an interactor.
* this work must done by hand and we developed tools to help community in this process.
* All data created with FreeDDIManager can be shared between users.

This slide will become clearer after watching the video
component = molecule
interactor = molecule type (ATC)
eg fluindione (main long term anticoagulant in France): no ATC, no INN
Interactor linked to cytochrom P450: all substrate or all inhibitors


- - -

### 12 Notes

- - -

### 13 Notes

- - -

### 14 Notes

Does JavaScript pose a security risk?

- - -

### 15 Notes

FreeMedForms project DataPack management is an easy and fast process to add functionalities to any applications of the project.
Datapacks represent pure data, patient forms, scripts, binaries.
FreeDDIManager makes it possible for you to create in a few clicks a datapack server. A server can be a local path or a remote server. When the server is built, you just have to send to your users the URL of the server and their login information.
Application can manage as many servers as you want. It manages dependencies between packs, application version limitations, pack version, pack update.

Full documentation is available on our website.

- - - 

### 16 Notes

Last year, Eric presented the alert system of the FreeMedForms project at the LSM in Brussels. The video is available.

We defined two systems of alert: one blocking and one non-blocking.

The non-blocking alert is represented by and icon and tooltips in views. While the blocking alerts are blocking dialogs that needs user validation to proceed. Both alerts present minimal and sufficient information. At any time users can access (to) full alert information.

The alert system provides three levels of risk : low medium and high. And each alert can be classified in an inifinite number of categories.

The threshold of each type of alert can be adapted by the user in the user preferences.

- - -

### 17 Notes

Alerts can be related to different types of attendees.

They can be cycling or not, with a validity period defined or not.

The validation method can include a user comment in case of the user override the alert.

Alerts can be dynamic using their script system.

- - -

### 18 Notes

Alerts can be shared between users or group of users through our datapack server. Alerts are then grouped into alert-packs.

A full documentation is about to be released on our main web site.

- - -

### 19 Notes

A french non-profit association was created in january 2013 with the unique objective to support the FreeMedForms community. The association is the unique provider of the official support for the suite.

### 20 Notes


