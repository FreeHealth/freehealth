<?php

    // 0. recupere les arguments
    $userName = $_POST['user'];
    $message = $_POST['msg'];

    // 1. Verifie l'existence du fichier de sauvegarde des dosages
    $dosagesFileName = 'FMF_dosages.xml';
    if (!file_exists( $dosagesFileName ) )
        touch( $dosagesFileName );

    // 2. Ouvre le fichier et ajoute les donnees transmises
    $fp = fopen ( $dosagesFileName , "a" );
    fwrite( $fp , $message );
    fclose ( $fp );

    // 3. Envoie une information au developpeurs de FreeMedForms
    if ( mail( 'eric.maeker@gmail.com', '[FreeMedForms-FromApp]', $userName . ' -> send dosages' ) )
       echo 'OK';
    else
       echo 'An error occured.';
?>

