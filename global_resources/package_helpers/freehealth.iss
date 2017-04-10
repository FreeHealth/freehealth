; Inno Setup compiler file for
; FreeMedForms
; (C) Eric Maeker, MD, GPLv3, FreeMedForms project

[Setup]
AppName=FreeHealth
AppId=FreeHealth
AppVerName=FreeHealth,0.9.9
DefaultDirName={pf}\FreeHealth
DefaultGroupName=FreeHealth
UninstallDisplayIcon={app}\freehealth.exe
VersionInfoDescription=FreeHealth 0.9.9
VersionInfoProductName=FreeHealth
;OutputDir=
;ShowLanguageDialog=no
;ShowUndisplayableLanguages=yes
AppCopyright=Copyright (C) 2008-2016 Eric Maeker, MD, The FreeMedForms project. Copyright © 20016 Jerome Pinguet {cm:FreeHealth}
AppPublisher={cm:FreeHealth}
AppPublisherURL=https://freehealth.io/
ChangesAssociations=yes
;LicenseFile=
;OutputDir=SourceDir
;WizardSmallImageFile=mysmallimage.bmp
;You may need to manually set SourceDir and SetupIconFile
;SourceDir=
SetupIconFile=ehr.ico
;WizardSmallImageFile=c:\freemedforms-0.9.6\global_resources\pixmap\svg\freemedforms-128.bmp
;WizardImageFile=c:\freemedforms-0.9.6\global_resources\pixmap\svg\freemedforms-128.bmp
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
en.FreeDiamsSetup=Free pharmaceutical prescriber and drugs interactions checker.
fr.FreeDiamsSetup=Assistant de prescription médicamenteuse et de recherche d'interactions médicamenteuse.
de.FreeDiamsSetup=Freie Arzt-und pharmazeutischen Medikamenten Wechselwirkungen checker.

en.FreeHealth=FreeHealth
fr.FreeHealth=FreeHealth
de.FreeHealth=FreeHealth

en.FullInstall=Full Install
de.FullInstall=Vollständige Installation
fr.FullInstall=Installation complète

en.CustomInstall=Custom Install
de.CustomInstall=Benutzerdefinierte Installation
fr.CustomInstall=Installation personnalisée

en.UninstallProgram=Uninstall %1
fr.UninstallProgram=Désinstaller %1
de.UninstallProgram=Deinstallieren %1

en.CompDbFr=Drugs database
de.CompDbFr=Drugs database
fr.CompDbFr=Base de données thérapeutiques

en.CompHelpEn=English help
de.CompHelpEn=Englisch Hilfe
fr.CompHelpEn=Aide : traduction anglaise

en.CompHelpFr=French help translations
de.CompHelpFr=Französisch Hilfe
fr.CompHelpFr=Aide française

en.StartApplication=Launch FreeHealth
de.StartApplication=Starten FreeHealth
fr.StartApplication=Démarrer FreeHealth

[Types]
Name: "full"; Description: {cm:FullInstall}
;Name: "onedb"; Description: {cm:OneDbInstall}
Name: "custom"; Description: {cm:CustomInstall}; Flags: iscustom

[Components]
Name: "program"; Description: "FreeHealth"; Types: full custom; Flags: fixed
Name: "helpEn"; Description: {cm:CompHelpEn}; Types: full custom
Name: "helpFr"; Description: {cm:CompHelpFr}; Types: full custom


[Files]
Source: "freehealth.exe"; DestDir: "{app}"; Components: program
Source: "*.dll"; DestDir: "{app}"; Components: program
Source: "plugins/*"; DestDir: "{app}/plugins"; Components: program; Flags: recursesubdirs
Source: "platforms/*"; DestDir: "{app}/platforms"; Components: program; Flags: recursesubdirs
Source: "Resources/datapacks/appinstalled/drugs/*"; DestDir: "{app}/Resources/datapacks/appinstalled/drugs"; Components: program
Source: "Resources/datapacks/appinstalled/defaultservers.txt"; DestDir: "{app}/Resources/datapacks/appinstalled"; Components: program
Source: "Resources/forms/*"; DestDir: "{app}/Resources/forms/"; Components: program; Flags: recursesubdirs
Source: "Resources/pixmap/*"; DestDir: "{app}/Resources/pixmap"; Components: program; Flags: recursesubdirs
Source: "Resources/profiles/*"; DestDir: "{app}/Resources/profiles"; Components: program; Flags: recursesubdirs
Source: "Resources/sql/*"; DestDir: "{app}/Resources/sql"; Components: program; Flags: recursesubdirs
Source: "Resources/textfiles/*"; DestDir: "{app}/Resources/textfiles"; Components: program; Flags: recursesubdirs
Source: "Resources/translations/*"; DestDir: "{app}/Resources/translations"; Components: program; Flags: recursesubdirs
Source: "Resources\doc\freehealth\en\*"; DestDir: "{app}\Resources\doc\freehealth\en"; Components: helpEn; Flags: recursesubdirs
Source: "Resources\doc\freehealth\fr\*"; DestDir: "{app}\Resources\doc\freehealth\fr"; Components: helpFr; Flags: recursesubdirs
Source: "Resources\package_helpers\freehealth.url"; DestDir: "{app}"
Source: "README.txt"; DestDir: "{app}"; Components: program  ; Flags: isreadme
Source: "COPYING.txt"; DestDir: "{app}"; Components: program

[Icons]
Name: "{group}\FreeHealth"; Filename: "{app}\FreeHealth.exe"
Name: "{group}\{cm:UninstallProgram,FreeHealth}"; Filename: "{uninstallexe}"
Name: "{group}\website"; FileName: "{app}\freehealth.url"
Name: "{group}\README.txt"; FileName: "{app}\README.txt"
Name: "{group}\COPYING.txt"; FileName: "{app}\COPYING.txt"
