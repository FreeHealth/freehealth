; Inno Setup compiler file for
; FreeToolBox
; (C) Eric Maeker, MD, GPLv3, FreeMedForms project

[Setup]
AppName=FreeToolBox
AppId=FreeToolBox
AppVerName=FreeToolBox,0.1.0~beta
DefaultDirName={pf}FreeToolBox
DefaultGroupName=FreeToolBox
UninstallDisplayIcon={app}FreeToolBox.exe
VersionInfoDescription=FreeToolBox 0.1.0~beta
VersionInfoProductName=FreeToolBox
OutputDir=FreeToolBox
;ShowLanguageDialog=no
;ShowUndisplayableLanguages=yes
AppCopyright=Copyright (C) 2008-2010 Eric Maeker, MD, {cm:FreeMedFormsProject}
AppPublisher={cm:FreeMedFormsProject}
AppPublisherURL=http://www.freemedforms.com/
ChangesAssociations=yes
;LicenseFile=
;OutputDir=SourceDir
;WizardSmallImageFile=mysmallimage.bmp
SourceDir=e:FreeToolBox-0.1.0~beta\packages\win\FreeToolBox
SetupIconFile=e:\FreeToolBox-0.1.0~beta\FreeToolBox\freetoolbox.ico
WizardSmallImageFile=e:\FreeToolBox-0.1.0~beta\global_resources\pixmap\svg\freetoolbox-128.bmp
WizardImageFile=e:\FreeToolBox-0.1.0~beta\global_resources\pixmap\svg\freetoolbox-128.bmp
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
en.FreeToolBoxSetup=FreeMedForms scientific assistant.
fr.FreeToolBoxSetup=Assistant scientifique du projet FreeMedForms.
de.FreeToolBoxSetup=FreeMedForms wissenschaftlicher Assistent.

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


en.CompHelpEn=English help
de.CompHelpEn=Englisch Hilfe
fr.CompHelpEn=Aide : traduction anglaise
en.CompHelpFr=French help translations
de.CompHelpFr=Französisch Hilfe
fr.CompHelpFr=Aide française

en.StartApplication=Launch FreeToolBox
de.StartApplication=Starten FreeToolBox
fr.StartApplication=Démarrer FreeToolBox

[Types]
Name: "full"; Description: {cm:FullInstall}
;Name: "onedb"; Description: {cm:OneDbInstall}
Name: "custom"; Description: {cm:CustomInstall}; Flags: iscustom

[Components]
Name: "program"; Description: "FreeToolBox"; Types: full custom; Flags: fixed
Name: "helpEn"; Description: {cm:CompHelpEn}; Types: full custom
Name: "helpFr"; Description: {cm:CompHelpFr}; Types: full custom


[Files]
Source: "FreeToolBox.exe"; DestDir: "{app}"; Components: program
Source: "*.dll"; DestDir: "{app}"; Components: program
Source: "plugins/*"; DestDir: "{app}/plugins"; Components: program; Flags: recursesubdirs
Source: "Resources/databases/icd10/icd10.db"; DestDir: "{app}/Resources/databases/icd10"; Components: program
Source: "Resources/pixmap/*"; DestDir: "{app}/Resources/pixmap"; Components: program; Flags: recursesubdirs
Source: "Resources/textfiles/*"; DestDir: "{app}/Resources/textfiles"; Components: program; Flags: recursesubdirs
Source: "Resources/translations/*"; DestDir: "{app}/Resources/translations"; Components: program; Flags: recursesubdirs
Source: "Resources/sql/*"; DestDir: "{app}/Resources/sql"; Components: program; Flags: recursesubdirs
Source: "Resources/doc/FreeToolBox/en/*"; DestDir: "{app}/Resources/doc/FreeToolBox/en"; Components: helpEn; Flags: recursesubdirs
Source: "Resources/doc/FreeToolBox/fr/*"; DestDir: "{app}/Resources/doc/FreeToolBox/fr"; Components: helpFr; Flags: recursesubdirs
Source: "Resources/package_helpers/freemedforms.url"; DestDir: "{app}"
Source: "README.txt"; DestDir: "{app}"; Components: program  ; Flags: isreadme
Source: "COPYING.txt"; DestDir: "{app}"; Components: program

[Icons]
Name: "{group}\FreeToolBox"; Filename: "{app}\FreeToolBox.exe"
Name: "{group}\{cm:UninstallProgram,FreeToolBox}"; Filename: "{uninstallexe}"
Name: "{group}\{cm:FreeMedFormsProject}"; FileName: "{app}\freemedforms.url"
Name: "{group}\README.txt"; FileName: "{app}\README.txt"
Name: "{group}\COPYING.txt"; FileName: "{app}\COPYING.txt"

