; Inno Setup compiler file for
; FreeMedForms
; (C) Eric Maeker, MD, GPLv3, FreeMedForms project

[Setup]
AppName=FreeMedForms
AppId=FreeMedForms
AppVerName=FreeMedForms,__version__
DefaultDirName={pf}\FreeMedForms
DefaultGroupName=FreeMedForms
UninstallDisplayIcon={app}\freemedforms.exe
VersionInfoDescription=FreeMedForms __version__
VersionInfoProductName=FreeMedForms
;OutputDir=
;ShowLanguageDialog=no
;ShowUndisplayableLanguages=yes
AppCopyright=Copyright (C) 2008-2016 Eric Maeker, MD, {cm:FreeMedFormsProject}
AppPublisher={cm:FreeMedFormsProject}
AppPublisherURL=https://www.freemedforms.com/
ChangesAssociations=yes
;LicenseFile=
;OutputDir=SourceDir
;WizardSmallImageFile=mysmallimage.bmp
;You may need to manually set SourceDir and SetupIconFile
;SourceDir=
SetupIconFile=freemedforms.ico
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

en.CompDbFr=Drugs database
de.CompDbFr=Drugs database
fr.CompDbFr=Base de données thérapeutique

en.CompHelpEn=English help
de.CompHelpEn=Englisch Hilfe
fr.CompHelpEn=Aide : traduction anglaise

en.CompHelpFr=French help translations
de.CompHelpFr=Französisch Hilfe
fr.CompHelpFr=Aide française

en.StartApplication=Launch FreeMedForms
de.StartApplication=Starten FreeMedForms
fr.StartApplication=Démarrer FreeMedForms

[Types]
Name: "full"; Description: {cm:FullInstall}
;Name: "onedb"; Description: {cm:OneDbInstall}
Name: "custom"; Description: {cm:CustomInstall}; Flags: iscustom

[Components]
Name: "program"; Description: "FreeMedForms"; Types: full custom; Flags: fixed
Name: "helpEn"; Description: {cm:CompHelpEn}; Types: full custom
Name: "helpFr"; Description: {cm:CompHelpFr}; Types: full custom


[Files]
Source: "freemedforms.exe"; DestDir: "{app}"; Components: program
Source: "*.dll"; DestDir: "{app}"; Components: program
Source: "plugins/*"; DestDir: "{app}/plugins"; Components: program; Flags: recursesubdirs
Source: "platforms/*"; DestDir: "{app}/platforms"; Components: program; Flags: recursesubdirs
Source: "Resources/datapacks/appinstalled/account/*"; DestDir: "{app}/Resources/datapacks/appinstalled/account"; Flags: recursesubdirs; Components: program
Source: "Resources/datapacks/appinstalled/drugs/*"; DestDir: "{app}/Resources/datapacks/appinstalled/drugs"; Components: program
Source: "Resources/datapacks/appinstalled/defaultservers.txt"; DestDir: "{app}/Resources/datapacks/appinstalled"; Components: program
Source: "Resources/forms/*"; DestDir: "{app}/Resources/forms/"; Components: program; Flags: recursesubdirs
Source: "Resources/pixmap/*"; DestDir: "{app}/Resources/pixmap"; Components: program; Flags: recursesubdirs
Source: "Resources/profiles/*"; DestDir: "{app}/Resources/profiles"; Components: program; Flags: recursesubdirs
Source: "Resources/sql/*"; DestDir: "{app}/Resources/sql"; Components: program; Flags: recursesubdirs
Source: "Resources/textfiles/*"; DestDir: "{app}/Resources/textfiles"; Components: program; Flags: recursesubdirs
Source: "Resources/translations/*"; DestDir: "{app}/Resources/translations"; Components: program; Flags: recursesubdirs
Source: "Resources/doc/freemedforms/en/*"; DestDir: "{app}/Resources/doc/freemedforms/en"; Components: helpEn; Flags: recursesubdirs
Source: "Resources/doc/freemedforms/fr/*"; DestDir: "{app}/Resources/doc/freemedforms/fr"; Components: helpFr; Flags: recursesubdirs
Source: "Resources/package_helpers/freemedforms.url"; DestDir: "{app}"
Source: "README.txt"; DestDir: "{app}"; Components: program  ; Flags: isreadme
Source: "COPYING.txt"; DestDir: "{app}"; Components: program

[Icons]
Name: "{group}\FreeMedForms"; Filename: "{app}\FreeMedForms.exe"
Name: "{group}\{cm:UninstallProgram,FreeMedForms}"; Filename: "{uninstallexe}"
Name: "{group}\{cm:FreeMedFormsProject}"; FileName: "{app}\freemedforms.url"
Name: "{group}\README.txt"; FileName: "{app}\README.txt"
Name: "{group}\COPYING.txt"; FileName: "{app}\COPYING.txt"
