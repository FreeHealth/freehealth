; Inno Setup compiler file for
; FreeAccount
; (C) Eric Maeker, MD, GPLv3, FreeMedForms project

[Setup]
AppName=FreeAccount
AppId=FreeAccount
AppVerName=FreeAccount,__version__
DefaultDirName={pf}\FreeAccount
DefaultGroupName=FreeAccount
UninstallDisplayIcon={app}\freeaccount.exe
VersionInfoDescription=FreeAccount __version__
VersionInfoProductName=FreeAccount
OutputDir=FreeAccount
;ShowLanguageDialog=no
;ShowUndisplayableLanguages=yes
AppCopyright=Copyright (C) 2008-2012 Eric Maeker, MD, {cm:FreeMedFormsProject}
AppPublisher={cm:FreeMedFormsProject}
AppPublisherURL=http://www.freemedforms.com/
ChangesAssociations=yes
;LicenseFile=
;OutputDir=SourceDir
;WizardSmallImageFile=mysmallimage.bmp
SourceDir=e:\freemedforms-__version__\packages\win\freeaccount
SetupIconFile=e:\freemedforms-__version__\freeaccount\freeaccount-src\freeaccount.ico
WizardSmallImageFile=e:\freemedforms-__version__\global_resources\pixmap\svg\freeaccount-128.bmp
WizardImageFile=e:\freemedforms-__version__\global_resources\pixmap\svg\freeaccount-128.bmp
;WizardImageStretch=no
Compression=lzma/normal

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"
Name: fr; MessagesFile: "compiler:Languages\French.isl"
Name: de; MessagesFile: "compiler:Languages\German.isl"

[Messages]
en.BeveledLabel=English
fr.BeveledLabel=FranÁais
de.BeveledLabel=Deutsch

[CustomMessages]
en.FreeAccountSetup=Free medical accountancy manager.
fr.FreeAccountSetup=Assistant de comptabilité médicale.
de.FreeAccountSetup=Freie medizinische rechnungswesen manager.

en.FreeMedFormsProject=The FreeMedForms project
fr.FreeMedFormsProject=Le projet FreeMedForms
de.FreeMedFormsProject=Das Projekt FreeMedForms

en.FullInstall=Full Install
de.FullInstall=Vollst‰ndige Installation
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

en.CompDbFr=Drugs database
de.CompDbFr=Drugs database
fr.CompDbFr=Base de données thérapeutique

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
de.CompHelpFr=Franzˆsisch Hilfe
fr.CompHelpFr=Aide française

en.StartApplication=Launch FreeAccount
de.StartApplication=Starten FreeAccount
fr.StartApplication=Démarrer FreeAccount

[Types]
Name: "full"; Description: {cm:FullInstall}
;Name: "onedb"; Description: {cm:OneDbInstall}
Name: "custom"; Description: {cm:CustomInstall}; Flags: iscustom

[Components]
Name: "program"; Description: "FreeAccount"; Types: full custom; Flags: fixed
Name: "dbFr"; Description: {cm:CompDbFr}; Types: full
Name: "helpEn"; Description: {cm:CompHelpEn}; Types: full custom
Name: "helpFr"; Description: {cm:CompHelpFr}; Types: full custom


[Files]
Source: "freeaccount.exe"; DestDir: "{app}"; Components: program
Source: "*.dll"; DestDir: "{app}"; Components: program
Source: "plugins/*"; DestDir: "{app}/plugins"; Components: program; Flags: recursesubdirs
Source: "Resources/pixmap/*"; DestDir: "{app}/Resources/pixmap"; Components: program; Flags: recursesubdirs
Source: "Resources/sql/*"; DestDir: "{app}/Resources/sql"; Components: program; Flags: recursesubdirs
Source: "Resources/textfiles/*"; DestDir: "{app}/Resources/textfiles"; Components: program; Flags: recursesubdirs
Source: "Resources/translations/*"; DestDir: "{app}/Resources/translations"; Components: program; Flags: recursesubdirs
;Source: "Resources/doc/freeaccount/en/*"; DestDir: "{app}/Resources/doc/freeaccount/en"; Components: helpEn; Flags: recursesubdirs
;Source: "Resources/doc/freeaccount/fr/*"; DestDir: "{app}/Resources/doc/freeaccount/fr"; Components: helpFr; Flags: recursesubdirs
Source: "Resources/package_helpers/freemedforms.url"; DestDir: "{app}"
Source: "README.txt"; DestDir: "{app}"; Components: program  ; Flags: isreadme
Source: "COPYING.txt"; DestDir: "{app}"; Components: program

[Icons]
Name: "{group}\FreeAccount"; Filename: "{app}\FreeAccount.exe"
Name: "{group}\{cm:UninstallProgram,FreeAccount}"; Filename: "{uninstallexe}"
Name: "{group}\{cm:FreeMedFormsProject}"; FileName: "{app}\freemedforms.url"
Name: "{group}\README.txt"; FileName: "{app}\README.txt"
Name: "{group}\COPYING.txt"; FileName: "{app}\COPYING.txt"

