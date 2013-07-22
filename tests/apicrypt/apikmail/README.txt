ApiKmail v0.0.1
Copyright (c) 2012, Laurent Masson
docteur.laurentmasson@gmail.com

What is ApiKmail ?


ApiKmail is a sub-projects of FreeMedForms. 
http://www.freemedforms.com/

This project is a free and open source project.

ApiKmail code is on License 3-clause BSD

ApiKmail is coded in C
ApiKmail is available for:
- Linux

ApiKmail is a program that allows:
- Receiving the data stream of mail client Kmail (Kmail pipe emails through ApiKmail),
- To send the decryption program apicrypt the message body,
- Redirect to the mail client the decrypted text
- And make a copy in a formatted file (for use via an integrator management software for patient records)

Apicrypt is a French encryption process of confidential medical data flowing through messaging.
http://www.apicrypt.org

---------------------------------------------------------------------

Qu'est-ce qu'ApiKmail ?

ApiKmail fait parti du projet FreeMedForms.
http://www.freemedforms.com/

Ce projet est un projet libre et open source.

Le code d'ApiKmail est sous licence BSD 3-clause

ApiKmail est codé en langage C
ApiKmail fonctionne sous :
- Linux

ApiKmail est un programme qui permet :
- de recevoir le flux de données du client mail Kmail, 
- d'envoyer au programme de décryptage apicrypt le corps du message, 
- de rediriger vers le client de messagerie le texte décrypté 
- et d'en faire une copie formatée dans un dossier (en vue d'une exploitation via un intégrateur pour logiciel de gestion de dossiers patients)

Utilisation :
1) Placer le fichier apikmail dans le dossier apicrypt
2) Dans Kmail => configuration => configurer les filtres
   Créer un filtre sur le corps du message qui doit contenir $apicrypt
   Action du filtre : Filtrer par /home/user/apicrypt/apiKmail
3) Eventuellement configuer le fichier apikmail.ini :
   la première ligne correspond au chemin absolu du dossier apicrypt
   la deuxième ligne correspond au chemin absolu du dossier temporaire de destination (apitemp pour MedinTux)
4) Tester en local depuis le dossier apicrypt et en ligne de commande, avec un fichier test : ./apikmail < mail.mbox

Apicrypt est un outil de sécurisation du courrier médical
http://www.apicrypt.org


