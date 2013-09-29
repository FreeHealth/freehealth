; Inno Setup compiler file for
; FreeDRC
; (C) Eric Maeker, MD, GPLv3, FreeMedForms project

[Setup]
AppName=FreeDRC
AppId=FreeDRC
AppVerName=FreeDRC,__version__
DefaultDirName={pf}\FreeDRC
DefaultGroupName=FreeDRC
UninstallDisplayIcon={app}\freedrc.exe
VersionInfoDescription=FreeDRC __version__
VersionInfoProductName=FreeDRC
OutputDir=FreeDRC
;ShowLanguageDialog=no
;ShowUndisplayableLanguages=yes
AppCopyright=Copyright (C) 2008-2013 Eric Maeker, MD, {cm:FreeMedFormsProject}
AppPublisher={cm:FreeMedFormsProject}
AppPublisherURL=http://www.freemedforms.com/
ChangesAssociations=yes
;LicenseFile=
;OutputDir=SourceDir
;WizardSmallImageFile=mysmallimage.bmp
SourceDir=e:\freemedforms-__version__\packages\win\freedrc
SetupIconFile=e:\freemedforms-__version__\freedrc\freedrc-src\freedrc.ico
WizardSmallImageFile=e:\freemedforms-__version__\global_resources\pixmap\svg\freedrc-128.bmp
WizardImageFile=e:\freemedforms-__version__\global_resources\pixmap\svg\freedrc-128.bmp
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
en.FreeDRCSetup=Free eDRC (SFMG) coding assistant.
fr.FreeDRCSetup=Assistant de codage des eDRC (SFMG).
de.FreeDRCSetup=Free eDRC (SFMG) coding assistant.

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

en.StartApplication=Launch FreeDRC
de.StartApplication=Starten FreeDRC
fr.StartApplication=Démarrer FreeDRC

[Types]
Name: "full"; Description: {cm:FullInstall}
Name: "custom"; Description: {cm:CustomInstall}; Flags: iscustom

[Components]
Name: "program"; Description: "FreeDRC"; Types: full custom; Flags: fixed
Name: "helpEn"; Description: {cm:CompHelpEn}; Types: full custom
Name: "helpFr"; Description: {cm:CompHelpFr}; Types: full custom


[Files]
Source: "freedrc.exe"; DestDir: "{app}"; Components: program
Source: "*.dll"; DestDir: "{app}"; Components: program
Source: "plugins/*"; DestDir: "{app}/plugins"; Components: program; Flags: recursesubdirs
Source: "Resources/datapacks/appinstalled/edrc_ro/*"; DestDir: "{app}/Resources/datapacks/appinstalled/edrc_ro"; Components: program
Source: "Resources/datapacks/appinstalled/defaultservers.txt"; DestDir: "{app}/Resources/datapacks/appinstalled"; Components: program
Source: "Resources/pixmap/*"; DestDir: "{app}/Resources/pixmap"; Components: program; Flags: recursesubdirs
Source: "Resources/textfiles/*"; DestDir: "{app}/Resources/textfiles"; Components: program; Flags: recursesubdirs
Source: "Resources/translations/*"; DestDir: "{app}/Resources/translations"; Components: program; Flags: recursesubdirs
Source: "Resources/doc/freedrc/en/*"; DestDir: "{app}/Resources/doc/freedrc/en"; Components: helpEn; Flags: recursesubdirs
Source: "Resources/doc/freedrc/fr/*"; DestDir: "{app}/Resources/doc/freedrc/fr"; Components: helpFr; Flags: recursesubdirs
Source: "Resources/package_helpers/freemedforms.url"; DestDir: "{app}"
Source: "README.txt"; DestDir: "{app}"; Components: program  ; Flags: isreadme
Source: "COPYING.txt"; DestDir: "{app}"; Components: program

[Icons]
Name: "{group}\FreeDRC"; Filename: "{app}\FreeDRC.exe"
Name: "{group}\{cm:UninstallProgram,FreeDRC}"; Filename: "{uninstallexe}"
Name: "{group}\{cm:FreeMedFormsProject}"; FileName: "{app}\freemedforms.url"
Name: "{group}\README.txt"; FileName: "{app}\README.txt"
Name: "{group}\COPYING.txt"; FileName: "{app}\COPYING.txt"

