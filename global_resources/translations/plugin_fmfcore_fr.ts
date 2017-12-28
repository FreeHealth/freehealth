<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr_FR">
<context>
    <name>Core::AppConfigWizard</name>
    <message>
        <source>Application Configurator Wizard</source>
        <translation>Assistant de configuration</translation>
    </message>
</context>
<context>
    <name>Core::ClientConfigPage</name>
    <message>
        <source>Network client configuration</source>
        <translation>Configuration réseau en tant que client</translation>
    </message>
    <message>
        <source>You must configure the server using your personnal login and password to connect the database.</source>
        <translation>Vous devez configurer le serveur en utilisant vos identifiant et mot de passe personnels que vous utilisez pour vous connecter à la base de données.</translation>
    </message>
    <message>
        <source>Database global prefix</source>
        <translation>Préfixe global de base de données</translation>
    </message>
    <message>
        <source>Please specify the database prefix to use (please ask your software administrator)</source>
        <translation>Merci d&apos;indiquer le préfixe de base de données à utiliser (merci de demander à l&apos;adminstrateur de votre base de données)</translation>
    </message>
    <message>
        <source>No server configuration detected</source>
        <translation>Aucune configuration serveur détectée</translation>
    </message>
    <message>
        <source>You are trying to configure a network client. It is manadatory to connect to a network server.
While the host connection is valid, no configuration was found on this host.

Please check that this host contains a server configuration.</source>
        <translation>Vous essayez de configurer le logiciel pour qu&apos;il se connecte à des bases de données existantes.
Le nom d&apos;hôte est valide mais aucune configuration correcte n&apos;a été trouvée sur cet hôte.
Merci de vérifier que cet hôte contient bien une configuration correcte.</translation>
    </message>
</context>
<context>
    <name>Core::CoreConfigPage</name>
    <message>
        <source>Welcome to %1</source>
        <translation>Bienvenue, %1</translation>
    </message>
    <message>
        <source>Select your language</source>
        <translation>Sélectionnez votre langue</translation>
    </message>
    <message>
        <source>Select the type of installation</source>
        <translation>Sélectionnez le type d&apos;installation</translation>
    </message>
    <message>
        <source>This wizard will help you configure the base parameters of the application.
Select your preferred language and the installation type.</source>
        <translation>Cet installateur va vous aider à configurer les paramètres de base de l&apos;application.
Sélectionnez votre langue et votre type d&apos;installation.</translation>
    </message>
    <message>
        <source>Create MariaDB databases</source>
        <translation>Créer les bases de données MariaDB</translation>
    </message>
    <message>
        <source>Connect to MariaDB databases</source>
        <translation>Se connecter aux bases de données MariaDB</translation>
    </message>
    <message>
        <source>Test without MariaDB</source>
        <translation>Tester sans MariaDB</translation>
    </message>
    <message>
        <source>&lt;b&gt;This simplified installation procedure is for testing purposes only.&lt;/b&gt; 
It is using SQLite as a temporary database. We do not recommend that you use SQLite to store medical data because  FreeHealth support of SQLite will end soon. If you want to install FreeHealth for professional use, please choose &lt;b&gt;Create MariaDB databases&lt;/b&gt; instead.
Click Ok to test FreeHealth, or click Cancel to install FreeHealth with MariaDB.</source>
        <translation>&lt;b&gt;Cette procédure d&apos;installation simplifiée sert uniquement à tester l&apos;application.&lt;/b&gt; 
Elle utilise une base de données temporaire SQLite. Nous vous recommandons de ne pas utiliser SQLite pour stocker des données médicales car le support de SQLite sera bientôt abandonné. Si vous voulez installer FreeHealth pour un usage professionnel, merci d&apos;installer MariaDB puis de choisir  &lt;b&gt;Créer les bases de données MariaDB&lt;/b&gt; dans le menu déroulant.
Cliquer sur &lt;b&gt;Ok&lt;/b&gt; pour tester FreeHealth, ou cliquer sur &lt;b&gt;Annuler&lt;/b&gt; pour installer FreeHealth avec MariaDB.</translation>
    </message>
</context>
<context>
    <name>Core::CoreDatabaseCreationPage</name>
    <message>
        <source>Preparing databases</source>
        <translation>Préparation des bases de données</translation>
    </message>
    <message>
        <source>Preparing databases. Please wait...</source>
        <translation>Préparation des bases de données. Veuillez patienter...</translation>
    </message>
    <message>
        <source>Use this prefix for all databases&lt;br&gt;&lt;i&gt;&amp;nbsp;&amp;nbsp;Optional, you can safely leave this editor empty&lt;/i&gt;</source>
        <translation>Utiliser ce préfixe pour toutes les bases de données&lt;br&gt;&lt;i&gt;&amp;nbsp;&amp;nbsp;Optionnel, vous pouvez laisser ce paramètre vide&lt;/i&gt;</translation>
    </message>
    <message>
        <source>If you define a global prefix, all database will be named {YourPrefix}{DatabaseName} for all configuration.</source>
        <translation>Si vous définissez un préfixe global, toutes les bases de données seront nommées {Préfixe}{NomBaseDeDonnées} pour toutes les configurations.</translation>
    </message>
    <message>
        <source>Create all database</source>
        <translation>Créer les bases de données</translation>
    </message>
    <message>
        <source>Select the path where to store your personal databases&lt;br&gt;&lt;i&gt;&amp;nbsp;&amp;nbsp;Optional, you can safely leave this editor empty&lt;/i&gt;</source>
        <translation>Sélectionnez le chemin où vous souhaitez sauvegarder vos bases de données personnelles&lt;br&gt;&lt;i&gt;&amp;nbsp;&amp;nbsp;Optionnel, vous pouvez laisser ce paramètre vide&lt;/i&gt;</translation>
    </message>
    <message>
        <source>You can store your local personal database anywhere you want, just select a path.</source>
        <translation>Vous pouvez sauvegardeer vos bases de données locales là où vous le souhaitez, vous n&apos;avez qu&apos;à sélectionner le chemin.</translation>
    </message>
    <message>
        <source>Configure the server and create all database</source>
        <translation>Configurer le serveur et créer toutes les bases de données</translation>
    </message>
</context>
<context>
    <name>Core::EndConfigPage</name>
    <message>
        <source>%1 is now configured</source>
        <translation>%1 est maintenant configuré</translation>
    </message>
    <message>
        <source>Don&apos;t clean databases</source>
        <translation>Ne pas nettoyer les bases</translation>
    </message>
    <message>
        <source>Clean and recreate database</source>
        <translation>Nettoyer et recréer les bases</translation>
    </message>
    <message>
        <source>You can create virtual data to test the application. Select the option above.</source>
        <translation>Vous pouvez créer des données virtuelles pour tester l&apos;application. Sélectionnez l&apos;option ci-dessous.</translation>
    </message>
    <message>
        <source>Don&apos;t create virtual data</source>
        <translation>Ne pas créer les données virtuelles</translation>
    </message>
    <message>
        <source>Create virtual data</source>
        <translation>Créer les données virtuelles</translation>
    </message>
    <message>
        <source>You can clean and recreate all your databases. Select the option above. If you select the clean option, all databases will be erased with &lt;b&gt;definitive data loss&lt;/b&gt;.</source>
        <translation>Vous pouvez effacer et recréer toutes vos bases de données. Sélectionnez l&apos;option ci-dessous. Si vous sélectionnez cette option, toutes vos bases de données seront effacées avec &lt;b&gt;une perte définitive des données&lt;/b&gt;.</translation>
    </message>
    <message>
        <source>Please read the online user guide.</source>
        <translation>Merci de lire le manuel de l&apos;utilisateur en ligne.</translation>
    </message>
    <message>
        <source>Help, support and information:</source>
        <translation>Aide, assistance et information:</translation>
    </message>
</context>
<context>
    <name>Core::HelpDialog</name>
    <message>
        <source>&lt;center&gt;Documentation is available on line only.&lt;/center&gt;</source>
        <translation>&lt;center&gt;La documentation est uniquement disponible en ligne.&lt;/center&gt;</translation>
    </message>
</context>
<context>
    <name>Core::ICore</name>
    <message>
        <source>You are running an alpha version of %1. This version can be heavily buggy and is only provided for testing purpose.</source>
        <translation>Vous utilisez une version alpha de %1. Cette version peut être sérieusement bogguée et n&apos;est proposée que pour des fins de test.</translation>
    </message>
    <message>
        <source>If you found any problem with %1, please report it on our mailing list: &lt;a href=&quot;mailto:freemedforms@googlegroups.com&gt;freemedforms@googlegroups.com&lt;/a&gt;</source>
        <translation>Si vous rencontrez un problème quelconque avec %1, veuillez le notifier sur la liste de diffusion : &lt;a href=&quot;mailto:freemedforms@googlegroups.com&gt;freemedforms@googlegroups.com&lt;/a&gt;</translation>
    </message>
    <message>
        <source>Warning alpha version</source>
        <translation>Attention version alpha</translation>
    </message>
</context>
<context>
    <name>Core::Internal::AboutDialog</name>
    <message>
        <source>Application name</source>
        <translation>Nom de l&apos;application</translation>
    </message>
    <message>
        <source>Dialog</source>
        <translation></translation>
    </message>
    <message>
        <source>About FreeMedForms</source>
        <translation>À propos de FreeMedForms</translation>
    </message>
</context>
<context>
    <name>Core::Internal::AppAboutPage</name>
    <message>
        <source>Application</source>
        <translation></translation>
    </message>
    <message>
        <source>General</source>
        <translation>Général</translation>
    </message>
</context>
<context>
    <name>Core::Internal::ApplicationGeneralPreferencesWidget</name>
    <message>
        <source>Update engine</source>
        <translation>Moteur de mise à jour</translation>
    </message>
    <message>
        <source>Update checking</source>
        <translation>Vérification des mises à jour</translation>
    </message>
    <message>
        <source>Database Management</source>
        <translation>Pilotage des bases de données</translation>
    </message>
    <message>
        <source>Automatically save data without prompting users</source>
        <translation>Toujours sauvegarder automatiquement les données</translation>
    </message>
    <message>
        <source>Allow and use virtual data</source>
        <translation>Autoriser l&apos;utilisation des données virtuelles</translation>
    </message>
    <message>
        <source>Form</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>Core::Internal::BuildAboutPage</name>
    <message>
        <source>Build information</source>
        <translation>Information de compilation</translation>
    </message>
    <message>
        <source>Current Qt version: %1</source>
        <translation>Version courante de Qt : %1</translation>
    </message>
    <message>
        <source>Application</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>Core::Internal::CoreImpl</name>
    <message>
        <source>Core intialization finished...</source>
        <translation>Initialisation du Core terminée...</translation>
    </message>
</context>
<context>
    <name>Core::Internal::CorePlugin</name>
    <message>
        <source>Initializing core plugin...</source>
        <translation>Initialisation du plugin central...</translation>
    </message>
</context>
<context>
    <name>Core::Internal::DebugDialog</name>
    <message>
        <source>Save log</source>
        <translation>Enregistrer le journal</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <source>Dialog</source>
        <translation></translation>
    </message>
    <message>
        <source>Ok</source>
        <translation></translation>
    </message>
    <message>
        <source>Save log to...</source>
        <translation>Sauvegarder le journal dans...</translation>
    </message>
    <message>
        <source>Log correctly saved</source>
        <translation>Journal correctement sauvegardé</translation>
    </message>
    <message>
        <source>The log was correctly saved into&lt;br /&gt;%1</source>
        <translation>Le journale a été correctement sauvegardé dans &lt;br /&gt;%1</translation>
    </message>
    <message>
        <source>Saving log</source>
        <translation>Sauvegarde du journal</translation>
    </message>
</context>
<context>
    <name>Core::Internal::LicenseAboutPage</name>
    <message>
        <source>License</source>
        <translation>Licence</translation>
    </message>
    <message>
        <source>Application</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>Core::Internal::PageWidget</name>
    <message>
        <source>Options</source>
        <translation></translation>
    </message>
    <message>
        <source>0</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>Core::Internal::ProxyPreferencesWidget</name>
    <message>
        <source>Proxy settings</source>
        <translation>Paramètres du proxy</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Hôte</translation>
    </message>
    <message>
        <source>Port</source>
        <translation>Port</translation>
    </message>
    <message>
        <source>User name</source>
        <translation>Nom d&apos;utilisateur</translation>
    </message>
    <message>
        <source>User password</source>
        <translation>Mot de passe</translation>
    </message>
    <message>
        <source>Auto-detect</source>
        <translation>Détecter</translation>
    </message>
    <message>
        <source>Form</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>Core::Internal::ServerPreferencesWidget</name>
    <message>
        <source>Test connection</source>
        <translation>Tester la connection</translation>
    </message>
    <message>
        <source>Database server</source>
        <translation>Serveur de données</translation>
    </message>
    <message>
        <source>Server type</source>
        <translation>Type de serveur</translation>
    </message>
    <message>
        <source>Host name</source>
        <translation>Nom de l&apos;hôte</translation>
    </message>
    <message>
        <source>Port</source>
        <translation>Port</translation>
    </message>
    <message>
        <source>Test host connection</source>
        <translation>Tester la connexion à l&apos;hôte</translation>
    </message>
    <message>
        <source>User identification</source>
        <translation>Identification de l&apos;utilisateur</translation>
    </message>
    <message>
        <source>Login</source>
        <translation>Identifiant</translation>
    </message>
    <message>
        <source>Password</source>
        <translation>Mot de passe</translation>
    </message>
    <message>
        <source>Use the default FreeMedForms administrator</source>
        <translation>Utiliser l&apos;administrateur par défaut de FreeMedForms</translation>
    </message>
    <message>
        <source>Form</source>
        <translation></translation>
    </message>
    <message>
        <source>MySQL</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>Core::Internal::ServerPreferencesWidgetPrivate</name>
    <message>
        <source>Enter host name...</source>
        <translation>Entrer le nom d&apos;hôte...</translation>
    </message>
</context>
<context>
    <name>Core::Internal::SettingDebugPage</name>
    <message>
        <source>Settings</source>
        <translation>Préférences</translation>
    </message>
</context>
<context>
    <name>Core::Internal::SettingsDialog</name>
    <message>
        <source>Options</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>Core::Internal::SettingsPrivate</name>
    <message>
        <source>Using ini file %1.</source>
        <translation>Utilise le fichier INI %1.</translation>
    </message>
    <message>
        <source>Ini file %1 is not writable. Can not use it.</source>
        <translation>Le fichier %1 n&apos;est pas accessible en écriture. Impossible de l&apos;utiliser.</translation>
    </message>
    <message>
        <source>Using ini file %1</source>
        <translation>Utilise le fichier INI %1</translation>
    </message>
    <message>
        <source>Found a configuration file next to the binary. Reading file.</source>
        <translation>Fichier de configuration trouvé à côté du binaire. Fichier en cours de lecture.</translation>
    </message>
    <message>
        <source>File does not exists and can not be created: %1</source>
        <translation>Le fichier n&apos;existe pas et ne peut pas être créé : %1</translation>
    </message>
    <message>
        <source>Trying ini file %1</source>
        <translation>Essaie le fichier INI %1</translation>
    </message>
    <message>
        <source>Name</source>
        <translation>Nom</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Valeur</translation>
    </message>
    <message>
        <source>Operating System</source>
        <translation>Système d&apos;exploitation</translation>
    </message>
    <message>
        <source>uname output</source>
        <translation>Sortie de uname</translation>
    </message>
    <message>
        <source>Compile Qt version</source>
        <translation>Version de Qt (compilation)</translation>
    </message>
    <message>
        <source>Actual Qt version</source>
        <translation>Version actuelle de Qt</translation>
    </message>
    <message>
        <source>Compile mode</source>
        <translation>Mode de compilation</translation>
    </message>
    <message>
        <source>GIT revision</source>
        <translation>Révision git</translation>
    </message>
    <message>
        <source>Binary</source>
        <translation>Binaire</translation>
    </message>
    <message>
        <source>Read only Databases</source>
        <translation>Bases de données en lecture seule</translation>
    </message>
    <message>
        <source>Writable databases</source>
        <translation>Bases de données en écriture</translation>
    </message>
    <message>
        <source>Bundle root path</source>
        <translation>Répertoire du bundle</translation>
    </message>
    <message>
        <source>Bundle resources path</source>
        <translation>Répertoire resources du paquet</translation>
    </message>
    <message>
        <source>Translations path</source>
        <translation>Répertoire des traductions</translation>
    </message>
    <message>
        <source>Qt Plugins path</source>
        <translation>Répertoire des plugins de Qt</translation>
    </message>
    <message>
        <source>Qt FrameWorks path</source>
        <translation>Répertoire des bibliothèques Qt</translation>
    </message>
    <message>
        <source>Default installed datapack path</source>
        <translation>Répertoire d&apos;installation des paquets de données distribués par défaut</translation>
    </message>
    <message>
        <source>Passing command line ini file: %1</source>
        <translation>Passe les paramètres en ligne de commande : %1</translation>
    </message>
    <message>
        <source>WARNING: Ini file %1 can not be used.</source>
        <translation>Attention : fichier ini %1 ne peut pas être utiliser.</translation>
    </message>
    <message>
        <source>Unable to create dir: %1, no Ini File can be used.</source>
        <translation>Impossible de créer le répertoire %1, pas de fichier ini trouvé.</translation>
    </message>
    <message>
        <source>Datapack persistent temporary path</source>
        <translation>Répertoire temporaire persistant des paquets de données</translation>
    </message>
    <message>
        <source>Datapack installation path</source>
        <translation>Répertoire d&apos;installation des paquets de données</translation>
    </message>
    <message>
        <source>System information</source>
        <translation>Informations système</translation>
    </message>
    <message>
        <source>Datapack Complete Forms installation path</source>
        <translation>Répertoire d&apos;installation des paquets de données des formulaires complets</translation>
    </message>
    <message>
        <source>Datapack SubForms installation path</source>
        <translation>Répertoire d&apos;installation des paquets de données des sous-formulaires</translation>
    </message>
    <message>
        <source>Absolute Paths</source>
        <translation>Chemins absolus</translation>
    </message>
    <message>
        <source>Using Ini File</source>
        <translation>Fichier ini</translation>
    </message>
    <message>
        <source>Relative Paths</source>
        <translation>Chemins relatifs</translation>
    </message>
    <message>
        <source>Settings values</source>
        <translation>Valeurs des paramètres</translation>
    </message>
    <message>
        <source>Orphan settings</source>
        <translation>Paramètres orphelins</translation>
    </message>
    <message>
        <source>Running version: %1</source>
        <translation>Version courante : %1</translation>
    </message>
    <message>
        <source>Qt Build version: %1</source>
        <translation>Version de Qt pour la compilation : %1</translation>
    </message>
    <message>
        <source>Qt running version: %1</source>
        <translation>Version de Qt en cours d&apos;utilisation : %1</translation>
    </message>
    <message>
        <source>Application Version: %1</source>
        <translation>Version de l&apos;application : %1</translation>
    </message>
    <message>
        <source>Actual build: Debug</source>
        <translation>Compilation de débogage</translation>
    </message>
    <message>
        <source>Actual build: Release</source>
        <translation>Compilation pour utilisation en production</translation>
    </message>
    <message>
        <source>GIT revision: %1</source>
        <translation>Révision git : %1</translation>
    </message>
    <message>
        <source>Application path: %1</source>
        <translation>Chemin de l&apos;application : %1</translation>
    </message>
    <message>
        <source>Running on Linux</source>
        <translation>Utilisation sur Linux</translation>
    </message>
    <message>
        <source>uname returns: %1</source>
        <translation>retour de uname : %1</translation>
    </message>
    <message>
        <source>Running on MacOs</source>
        <translation>Utilisation sur MacOs</translation>
    </message>
    <message>
        <source>Running on Windows</source>
        <translation>Utilisation sur Windows</translation>
    </message>
    <message>
        <source>UserResourcesPath</source>
        <translation></translation>
    </message>
    <message>
        <source>SmallPixmapPath</source>
        <translation></translation>
    </message>
    <message>
        <source>MediumPixmapPath</source>
        <translation></translation>
    </message>
    <message>
        <source>BigPixmapPath</source>
        <translation></translation>
    </message>
    <message>
        <source>SystemTempPath</source>
        <translation></translation>
    </message>
    <message>
        <source>ApplicationTempPath</source>
        <translation></translation>
    </message>
    <message>
        <source>CompleteFormsPath</source>
        <translation></translation>
    </message>
    <message>
        <source>SubFormsPath</source>
        <translation></translation>
    </message>
    <message>
        <source>UserCompleteFormsPath</source>
        <translation></translation>
    </message>
    <message>
        <source>UserSubFormsPath</source>
        <translation></translation>
    </message>
    <message>
        <source>WebSiteUrl</source>
        <translation></translation>
    </message>
    <message>
        <source>FormsPath</source>
        <translation></translation>
    </message>
    <message>
        <source>DocumentationPath</source>
        <translation></translation>
    </message>
    <message>
        <source>We found a FreeMedForms user directory.</source>
        <translation>Nous avons trouvé un dossier utilisateur de FreeMedForms.</translation>
    </message>
    <message>
        <source>Do you want to use this directory with FreeHealth?</source>
        <translation>Souhaitez-vous utiliser ce dossier avec FreeHealth?</translation>
    </message>
    <message>
        <source>We found a freemedforms directory inside your user directory. If you want to use it with FreeHealth, click Yes and we will make a copy of it named freehealth. If you want to start a new installation from scratch, click No.</source>
        <translation>Nous avons trouvé un dossier freemedforms à l&apos;intérieur de votre dossier utilisateur. Si vous souhaitez utiliser ce dossier avec FreeHealth, cliquez sur Oui et nous en ferons une copie nommée freehealth. Si vous voulez procéder à une nouvelle installation en partant de zéro, cliquez sur Non.</translation>
    </message>
    <message>
        <source>Application Plugins path</source>
        <translation>Chemin vers les plugins de l&apos;application</translation>
    </message>
    <message>
        <source>We found a FreeMedforms parameter directory.</source>
        <translation>Nous avons trouvé un dossier contenant les paramètres de FreeMedForms.</translation>
    </message>
    <message>
        <source>We found a FreeMedForms parameter directory inside your user directory. It contains the connection parameters for your existing databases. If you want to use these databases with FreeHealth, click Yes. If you want to start a new installation from scratch, click No.</source>
        <translation>Nous avons trouvé un dossier contenant des paramètres de connexion à des bases de données FreeMedForms dans votre répertoire utilisateur. Si vous souhaitez continuer à utiliser ces bases de données avec FreeHealth, cliquez sur Oui. Si vous souhaitez effectuer une installation de FreeHealth en partant de zéro (avec de nouvelles bases de données), cliquez sur Non.</translation>
    </message>
    <message>
        <source>LocalCompleteFormsPath</source>
        <translation></translation>
    </message>
    <message>
        <source>LocalSubFormsPath</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>Core::Internal::SimpleTextDialog</name>
    <message>
        <source>Dialog</source>
        <translation></translation>
    </message>
    <message>
        <source>title</source>
        <translation>titre</translation>
    </message>
</context>
<context>
    <name>Core::Internal::SqliteDatabasePathPage</name>
    <message>
        <source>Database path</source>
        <translation>Chemin vers les bases de données</translation>
    </message>
</context>
<context>
    <name>Core::Internal::SqliteDatabasePathWidget</name>
    <message>
        <source>Form</source>
        <translation>Formulaire</translation>
    </message>
    <message>
        <source>When configured as single computer mode, you can move your personal database anywhere you want. If you change this value, you will have to restart the application.</source>
        <translation>Lorsque vous utilisez la configuration mono-poste, vous pouvez placer vos bases de données là où vous le souhaitez. Si vous modifiez cette valeur, vous devrez redémarrer l&apos;application.</translation>
    </message>
    <message>
        <source>Move database</source>
        <translation>Déplacer les bases de données</translation>
    </message>
    <message>
        <source>Select a path</source>
        <translation>Sélectionnez un chemin</translation>
    </message>
    <message>
        <source>Database moved</source>
        <translation>Bases de données déplacées</translation>
    </message>
    <message>
        <source>All your database are now moved to:
%1

&lt;b&gt;You must restart the application.&lt;/b&gt;</source>
        <translation>Toutes vos bases de données sont maintenant déplacée vers :
%1

&lt;b&gt;Vous devez redémarrer l&apos;application.&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>Core::Internal::TeamAboutPage</name>
    <message>
        <source>Team</source>
        <translation>Équipe</translation>
    </message>
    <message>
        <source>Application</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>Core::Internal::ThemePrivate</name>
    <message>
        <source>Using splash: %1</source>
        <translation>Utilise le spash : %1</translation>
    </message>
</context>
<context>
    <name>Core::ModeManager</name>
    <message>
        <source>Switch to &lt;b&gt;%1&lt;/b&gt; mode</source>
        <translation>Passer au mode &lt;b&gt;%1&lt;/b&gt;</translation>
    </message>
</context>
<context>
    <name>Core::PageWidget</name>
    <message>
        <source>Full view</source>
        <translation>Vue étendue</translation>
    </message>
</context>
<context>
    <name>Core::PluginDialog</name>
    <message>
        <source>Details</source>
        <translation>Détails</translation>
    </message>
    <message>
        <source>Error Details</source>
        <translation>Détails des erreurs</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Fermer</translation>
    </message>
    <message>
        <source>Installed Plugins</source>
        <translation>Plugins installés</translation>
    </message>
    <message>
        <source>Plugin Details of %1</source>
        <translation>Détails du plugin %1</translation>
    </message>
    <message>
        <source>Plugin Errors of %1</source>
        <translation>Détail des erreurs du plugin %1</translation>
    </message>
</context>
<context>
    <name>Core::ProxyPage</name>
    <message>
        <source>Proxy definition</source>
        <translation>Définition du pare-feu</translation>
    </message>
    <message>
        <source>%1 has detected a network proxy on your computer. 
Please check and validate the proxy settings.</source>
        <translation>%1 a détecté un pare-feu réseau sur votre ordinateur.
Veuillez vérifier et valider les données de paramétrage.</translation>
    </message>
</context>
<context>
    <name>Core::ServerConfigPage</name>
    <message>
        <source>Network server configuration</source>
        <translation>Configuration réseau : serveur</translation>
    </message>
    <message>
        <source>Missing files.</source>
        <translation>Fichiers manquants.</translation>
    </message>
    <message>
        <source>An error occured...</source>
        <translation>Une erreur est survenue...</translation>
    </message>
    <message>
        <source>An error occured when trying to execute the script configuration script.
Please check out the log files and contact your administrator.</source>
        <translation>Une erreur est survenue lors de la tentative d&apos;exécution du script de configuration.
Veuillez vérifier les fichiers journaux et contacter votre administrateur.</translation>
    </message>
    <message>
        <source>Server already configured</source>
        <translation>Serveur déjà configuré</translation>
    </message>
    <message>
        <source>Server configured</source>
        <translation>Serveur configuré</translation>
    </message>
    <message>
        <source>The server was successfully configured.</source>
        <translation>Le serveur a été correctement configuré.</translation>
    </message>
    <message>
        <source>You must configure the server using the server super-administrator login and password to connect the database.</source>
        <translation>Vous devez configurer le serveur en utilisant les identifiants et mot de passe du super-administrateur de la base de données.</translation>
    </message>
    <message>
        <source>The configuration script is missing. You can not configure the server without this script.&lt;br&gt;&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;</source>
        <translation>Impossible de trouver le script de configuration. Vous ne pouvez pas configurer le serveur sans ce script.&lt;br&gt;&lt;br&gt;&lt;b&gt;%1&lt;/b&gt;</translation>
    </message>
    <message>
        <source>Connection to the server: User privileges inadequate</source>
        <translation>Connexion au serveur: privilèges utilisateur incorrects</translation>
    </message>
    <message>
        <source>You need to connect with another user that has the following global privileges: select, udpate, delete, insert, create, drop, alter and create user.
Please contact your server administrator.</source>
        <translation>Vous devez vous connecter avec un autre utilisateur qui a les privilèges globaux suivants: select, update, delete, insert, create, drop, alter and create user.
Merci de contacter l&apos;administrateur de votre serveur de bases de données.</translation>
    </message>
    <message>
        <source>The server is already configured for FreeHealth.</source>
        <translation>Le serveur est déjà configuré pour FreeHealth.</translation>
    </message>
</context>
<context>
    <name>Core::ServerPreferencesWidget</name>
    <message>
        <source>Host name error (%1:%2)</source>
        <translation>Erreur de l&apos;hôte (%1: %2)</translation>
    </message>
    <message>
        <source>Host not reachable...</source>
        <translation>Hôte non joignable...</translation>
    </message>
    <message>
        <source>Host available...</source>
        <translation>Hôte joignable...</translation>
    </message>
    <message>
        <source>No anonymous connection allowed</source>
        <translation>Les connexion anonymes sont interdites</translation>
    </message>
    <message>
        <source>Test in progress...</source>
        <translation>Test en cours...</translation>
    </message>
    <message>
        <source>Connection error: %1</source>
        <translation>Erreur de connexion : %1</translation>
    </message>
    <message>
        <source>Connected</source>
        <translation>Connecté</translation>
    </message>
    <message>
        <source>Enter hostname...</source>
        <translation>Entrer le nom d&apos;hôte...</translation>
    </message>
    <message>
        <source>Host name is valid...</source>
        <translation>Le nom d&apos;hôte est valide...</translation>
    </message>
    <message>
        <source>Host name is not valid...</source>
        <translation>Le nom d&apos;hôte n&apos;est pas valide...</translation>
    </message>
</context>
<context>
    <name>Core::SettingsDialog</name>
    <message>
        <source>Apply all changes (all pages)</source>
        <translation>Appliquer tous les changements (toutes les pages)</translation>
    </message>
    <message>
        <source>Reset the current page to defaults</source>
        <translation>Réinitialiser la page courante aux valeurs par défaut</translation>
    </message>
</context>
<context>
    <name>Core::Translators</name>
    <message>
        <source>Can not load %1, path: %2</source>
        <translation>Impossible de charger %1, chemin %2</translation>
    </message>
    <message>
        <source>Add Translator %1.</source>
        <translation>Ajout du dictionnaire de traductions %1.</translation>
    </message>
    <message>
        <source>WARNING: %1 can not be loaded or is already loaded.</source>
        <translation>Attention : impossible de charger %1 ou est déjà chargé.</translation>
    </message>
</context>
<context>
    <name>tkConstants</name>
    <message>
        <source>FreeDiams Prescriptions (*.di)</source>
        <translation>Prescriptions FreeDiams (*.di)</translation>
    </message>
    <message>
        <source>FreeMedForms Forms File (*.xml)</source>
        <translation>Fichier Formulaire FreeMedForms (*.xml)</translation>
    </message>
    <message>
        <source>This token namespace contains all patient&apos;s related tokens.
It does include the form&apos;s extracted token (when forms use &apos;patientDataRepresentation&apos;),
but does not contains all other form items.</source>
        <translation>Ce nom d&apos;espace d&apos;étiquette contient toutes les informations relatives aux patients.</translation>
    </message>
    <message>
        <source>This token namespace contains all user&apos;s related tokens.</source>
        <translation>Ce nom d&apos;espace d&apos;étiquette contient toutes les informations relatives aux utilisateurs.</translation>
    </message>
    <message>
        <source>FreeDRC Consultation files (*.cr)</source>
        <translation>FreeDRC fichier de consultation (*.cr)</translation>
    </message>
    <message>
        <source>FreeDDIManager files (*.ddi)</source>
        <translation>Fichiers FreeDDIManager (*.ddi)</translation>
    </message>
    <message>
        <source>FreeMedForms database files (*.db)</source>
        <translation>Bases de données FreeMedForms (*.db)</translation>
    </message>
</context>
</TS>
