; Inno Setup compiler file for
; FreeICD
; (C) Eric Maeker, MD, GPLv3, FreeMedForms project

[Setup]
AppName=FreeICD
AppId=FreeICD
AppVerName=FreeICD,0.1.0
DefaultDirName={pf}\FreeICD
DefaultGroupName=FreeICD
UninstallDisplayIcon={app}\FreeICD.exe
VersionInfoDescription=FreeICD 0.1.0
VersionInfoProductName=FreeICD
OutputDir=FreeICD
;ShowLanguageDialog=no
;ShowUndisplayableLanguages=yes
AppCopyright=Copyright (C) 2008-2010 Eric Maeker, MD, {cm:FreeMedFormsProject}
AppPublisher={cm:FreeMedFormsProject}
AppPublisherURL=http://www.freemedforms.com/
ChangesAssociations=yes
;LicenseFile=
;OutputDir=SourceDir
;WizardSmallImageFile=mysmallimage.bmp
SourceDir=e:\FreeICD-0.1.0\packages\win\FreeICD
SetupIconFile=e:\FreeICD-0.1.0\FreeICD\freeicd.ico
WizardSmallImageFile=e:\FreeICD-0.1.0\global_resources\pixmap\svg\freeicd-128.bmp
WizardImageFile=e:\FreeICD-0.1.0\global_resources\pixmap\svg\freeicd-128.bmp
;WizardImageStretch=no
Compression=lzma/normal

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"
Name: fr; MessagesFile: "compiler:Languages\French.isl"
Name: de; MessagesFile: "compiler:Languages\German.isl"

[Messages]
en.BeveledLabel=English
fr.BeveledLabel=Français
de.BeveledLabel=Deutsch

[CustomMessages]
en.FreeICDSetup=Free pharmaceutical prescriber and drugs interactions checker.
fr.FreeICDSetup=Assistant de prescription médicamenteuse et de recherche d'interactions médicamenteuse.
de.FreeICDSetup=Freie Arzt-und pharmazeutischen Medikamenten Wechselwirkungen checker.

en.FreeMedFormsProject=The FreeMedForms project
fr.FreeMedFormsProject=Le projet FreeMedForms
en.FreeMedFormsProject=Das Projekt FreeMedForms

en.FullInstall=Full Install
de.FullInstall=Vollständige Installation
fr.FullInstall=Installation complète

en.CustomInstall=Custom Install
de.CustomInstall=Benutzerdefinierte Installation
fr.CustomInstall=Installation personnalisée

en.UninstallProgram=Uninstall %1
fr.UninstallProgram=Désinstaller %1
de.UninstallProgram=Deinstallieren %1

;en.OneDbInstall=One drugs database install
;de.OneDbInstall=
;fr.OneDbInstall=Installation avec une seule base de données thérapeutique

en.CompDbFr=French drugs database
de.CompDbFr=French drugs database
fr.CompDbFr=Base de données thérapeutique française
en.CompDbCa=Canadian drugs database
de.CompDbCa=Canadian drugs database
fr.CompDbCa=Base de données thérapeutique canadienne
en.CompDbUs=USA drugs database
de.CompDbUs=USA drugs database
fr.CompDbUs=Base de données thérapeutique américaine
en.CompDbZa=South african drugs database
de.CompDbZa=South african drugs database
fr.CompDbZa=Base de données thérapeutique sud africaine

;en.TypeDbFr=custom
;de.TypeDbFr=custom
;fr.TypeDbFr=custom onedb
;en.TypeDbCa=custom onedb
;de.TypeDbCa=custom
;fr.TypeDbCa=custom
;en.TypeDbUs=custom onedb
;de.TypeDbUs=custom
;fr.TypeDbUs=custom
;en.TypeDbZa=custom
;de.TypeDbZa=custom
;fr.TypeDbZa=custom

en.CompHelpEn=English help
de.CompHelpEn=Englisch Hilfe
fr.CompHelpEn=Aide : traduction anglaise
en.CompHelpFr=French help translations
de.CompHelpFr=Französisch Hilfe
fr.CompHelpFr=Aide française

en.StartApplication=Launch FreeICD
de.StartApplication=Starten FreeICD
fr.StartApplication=Démarrer FreeICD

[Types]
Name: "full"; Description: {cm:FullInstall}
;Name: "onedb"; Description: {cm:OneDbInstall}
Name: "custom"; Description: {cm:CustomInstall}; Flags: iscustom

[Components]
Name: "program"; Description: "FreeICD"; Types: full custom; Flags: fixed
Name: "dbFr"; Description: {cm:CompDbFr}; Types: full custom
Name: "dbCa"; Description: {cm:CompDbCa}; Types: full custom
Name: "dbUs"; Description: {cm:CompDbUs}; Types: full custom
Name: "dbZa"; Description: {cm:CompDbZa}; Types: full custom
Name: "helpEn"; Description: {cm:CompHelpEn}; Types: full custom
Name: "helpFr"; Description: {cm:CompHelpFr}; Types: full custom


[Files]
Source: "FreeICD.exe"; DestDir: "{app}"; Components: program
Source: "*.dll"; DestDir: "{app}"; Components: program
Source: "plugins/*"; DestDir: "{app}/plugins"; Components: program; Flags: recursesubdirs
Source: "Resources/databases/drugs/iam.db"; DestDir: "{app}/Resources/databases/drugs/"; Components: program
Source: "Resources/databases/drugs/drugs-fr_FR.db"; DestDir: "{app}/Resources/databases/drugs"; Components: dbFr
Source: "Resources/databases/drugs/drugs-en_CA.db"; DestDir: "{app}/Resources/databases/drugs/"; Components: dbCa
Source: "Resources/databases/drugs/drugs-en_US.db"; DestDir: "{app}/Resources/databases/drugs/"; Components: dbUs
Source: "Resources/databases/drugs/drugs-en_ZA.db"; DestDir: "{app}/Resources/databases/drugs/"; Components: dbZa
Source: "Resources/pixmap/*"; DestDir: "{app}/Resources/pixmap"; Components: program; Flags: recursesubdirs
Source: "Resources/textfiles/*"; DestDir: "{app}/Resources/textfiles"; Components: program; Flags: recursesubdirs
Source: "Resources/translations/*"; DestDir: "{app}/Resources/translations"; Components: program; Flags: recursesubdirs
Source: "Resources/doc/FreeICD/en/*"; DestDir: "{app}/Resources/doc/FreeICD/en"; Components: helpEn; Flags: recursesubdirs
Source: "Resources/doc/FreeICD/fr/*"; DestDir: "{app}/Resources/doc/FreeICD/fr"; Components: helpFr; Flags: recursesubdirs
Source: "Resources/package_helpers/freemedforms.url"; DestDir: "{app}"
Source: "README"; DestDir: "{app}"; Components: program  ; Flags: isreadme
Source: "COPYING"; DestDir: "{app}"; Components: program

[Icons]
Name: "{group}\FreeICD"; Filename: "{app}\FreeICD.exe"
Name: "{group}\{cm:UninstallProgram,FreeICD}"; Filename: "{uninstallexe}"
Name: "{group}\{cm:FreeMedFormsProject}"; FileName: "{app}\freemedforms.url"
Name: "{group}\README"; FileName: "{app}\README"
Name: "{group}\COPYING"; FileName: "{app}\COPYING"

