<?php
if (isset($_GET['source'])) { die(highlight_file(FILE, 1)); }
?>
<?php

$expected=array('nom', 'taille', 'couleur', 'expiration'); 
foreach ($expected as $formvar)
      $$formvar = (isset($_POST[$formvar]))?$_POST[$formvar]:NULL;
 
$expiration = time()+606024*5 ; // dans 5 jours
setcookie("Nom",$nom,$expiration);
setcookie("Taille",$taille,$expiration);
setcookie("Couleur",$couleur,$expiration);
?>
<!DOCTYPE html
    <head>
        <meta http-equiv="content-type" content="text/html;charset=ISO-8859-1"/>
        <title>&Eacute;criture des cookies</title>
    </head>

    <body bgcolor="#ffffff">
        <p>Le cookie a &eacute;t&eacute; sauv&eacute; avec les valeurs suivantes:</p>
        <table width="122" border="1" cellspacing="2" cellpadding="0">
            <tr>
                <td>Nom</td>
                <td><?php print ($nom);?></td>
            </tr>
            <tr>
                <td>Taille</td>
                <td><?php print ($taille);?></td>
            </tr>
            <tr>
                <td>Couleur</td>
                <td><?php print ($couleur);?></td>
            </tr>
        </table>
        <p>Cliquer <a href="readCookies.php">ici</a> pour lire le cookie sauv&eacute;</p>
        <p></p>
        <p><a href="..">RETOUR</a></p>
        <p></p>
    </body>

</html>