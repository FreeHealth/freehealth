; Inno Setup compiler file for
; FreeICD
; (C) Eric Maeker, MD, GPLv3, FreeMedForms project

[Setup]
AppName=FreeICD
AppId=FreeICD
AppVerName=FreeICD,__version__
DefaultDirName={pf}\FreeICD
DefaultGroupName=FreeICD
UninstallDisplayIcon={app}\FreeICD.exe
VersionInfoDescription=FreeICD __version__
VersionInfoProductName=FreeICD
OutputDir=FreeICD
;ShowLanguageDialog=no
;ShowUndisplayableLanguages=yes
AppCopyright=Copyright (C) 2008-2012 Eric Maeker, MD, {cm:FreeMedFormsProject}
AppPublisher={cm:FreeMedFormsProject}
AppPublisherURL=http://www.freemedforms.com/
ChangesAssociations=yes
;LicenseFile=
;OutputDir=SourceDir
;WizardSmallImageFile=mysmallimage.bmp
SourceDir=e:\freemedforms-__version__\packages\win\FreeICD
SetupIconFile=e:\freemedforms-__version__\freeicd\freeicd-src\freeicd.ico
WizardSmallImageFile=e:\freemedforms-__version__\global_resources\pixmap\svg\freeicd-128.bmp
WizardImageFile=e:\freemedforms-__version__\global_resources\pixmap\svg\freeicd-128.bmp
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
en.FreeICDSetup=Free ICD10 coding assistant.
fr.FreeICDSetup=Assistant au codage en CIM10.
de.FreeICDSetup=Freie ICD10 Codierung Assistenz.

en.FreeMedFormsProject=The FreeMedForms project
fr.FreeMedFormsProject=Le projet FreeMedForms
de.FreeMedFormsProject=Das Projekt FreeMedForms

en.FullInstall=Full Install
de.FullInstall=Vollständige Installation
fr.FullInstall=Installation complète

en.CustomInstall=Custom Install
de.CustomInstall=Benutzerdefinierte Installation
fr.CustomInstall=Installation personnalisée

en.UninstallProgram=Uninstall %1
fr.UninstallProgram=Désinstaller %1
de.UninstallProgram=Deinstallieren %1


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
Name: "helpEn"; Description: {cm:CompHelpEn}; Types: full custom
Name: "helpFr"; Description: {cm:CompHelpFr}; Types: full custom


[Files]
Source: "FreeICD.exe"; DestDir: "{app}"; Components: program
Source: "*.dll"; DestDir: "{app}"; Components: program
Source: "plugins/*"; DestDir: "{app}/plugins"; Components: program; Flags: recursesubdirs
Source: "Resources/databases/icd10/icd10.db"; DestDir: "{app}/Resources/databases/icd10"; Components: program
Source: "Resources/datapacks/appinstalled/defaultservers.txt"; DestDir: "{app}/Resources/datapacks/appinstalled"; Components: program
Source: "Resources/pixmap/*"; DestDir: "{app}/Resources/pixmap"; Components: program; Flags: recursesubdirs
Source: "Resources/textfiles/*"; DestDir: "{app}/Resources/textfiles"; Components: program; Flags: recursesubdirs
Source: "Resources/translations/*"; DestDir: "{app}/Resources/translations"; Components: program; Flags: recursesubdirs
Source: "Resources/sql/*"; DestDir: "{app}/Resources/sql"; Components: program; Flags: recursesubdirs
Source: "Resources/doc/FreeICD/en/*"; DestDir: "{app}/Resources/doc/FreeICD/en"; Components: helpEn; Flags: recursesubdirs
Source: "Resources/doc/FreeICD/fr/*"; DestDir: "{app}/Resources/doc/FreeICD/fr"; Components: helpFr; Flags: recursesubdirs
Source: "Resources/package_helpers/freemedforms.url"; DestDir: "{app}"
Source: "README.txt"; DestDir: "{app}"; Components: program  ; Flags: isreadme
Source: "COPYING.txt"; DestDir: "{app}"; Components: program

[Icons]
Name: "{group}\FreeICD"; Filename: "{app}\FreeICD.exe"
Name: "{group}\{cm:UninstallProgram,FreeICD}"; Filename: "{uninstallexe}"
Name: "{group}\{cm:FreeMedFormsProject}"; FileName: "{app}\freemedforms.url"
Name: "{group}\README.txt"; FileName: "{app}\README.txt"
Name: "{group}\COPYING.txt"; FileName: "{app}\COPYING.txt"

