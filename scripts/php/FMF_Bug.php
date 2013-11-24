<?php
  // Get args

  $userName = $_POST['user'];
  $messageBase64 = $_POST['msg'];
  $messageBase64 = str_replace(' ', '+', $messageBase64);

  $message = base64_decode($messageBase64);
  $message = str_replace("$<%DATE%>$", date('l jS \of F Y h:i:s A'), $message);

  // saving posted base64 to a file in the server
  $today = date("Ymd") . date("His");
  $fileName = getcwd() . '/../data/pages/en/bugreport/bugreport' . $today . '.txt';
  if (!file_exists($fileName)) {
      touch($fileName);
  }

  // save the bug file to dokuwiki bugreport namespace
  $fp = fopen($fileName , "a");
  fwrite($fp , $message);
  fclose ($fp);

  $dokuwikiUrl = 'http://www.freemedforms.com/en/bugreport/bugreport' . $today;
  if (mail('eric.maeker@gmail.com', '[FreeMedForms - Bug report]', $dokuwikiUrl)) {
      echo 'Debug report ' . $dokuwikiUrl;
  } else {
      echo 'ERREUR';
  }
?>

