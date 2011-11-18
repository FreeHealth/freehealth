 <?php
  // Get args
  $userName = $_POST['user'];
  $message = base64_decode($_POST['msg']);
  $message = str_replace("$<%DATE%>$", date('l jS \of F Y h:i:s A'), $message);

  // saving posted base64 to a file in the server
  $today = date("Ymd") . date("Hms");
  $fileName = '../dokuwiki/data/pages/bugreport/bugreport_' . $today;
  if (!file_exists($dosagesFileName))
      touch($dosagesFileName);

  // save the bug file to dokuwiki bugreport namespace
  $fp = fopen($fileName , "a");
  fwrite($fp , $message);
  fclose ($fp);

  $dokuwikiUrl = 'http://www.freemedforms.com/bugreport/bugreport_' . $today;
  if (mail('eric.maeker@gmail.com', '[FreeMedForms - Bug report]', $dokuwikiUrl)) {
      echo 'Debug report @ ' . $dokuwikiUrl;
  } else {
      echo 'ERREUR';
  }
?>

