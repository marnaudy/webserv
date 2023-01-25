<?php
if (isset($_GET['source'])) { die(highlight_file(FILE, 1)); }
?>
<!DOCTYPE html
    <head>
        <meta http-equiv="content-type" content="text/html;charset=utf-8"/>
        <title>Lecture de cookies</title>
    </head>

    <body>
        <p>L'information suivante a &eacute;t&eacute; sauv&eacute;e dans un cookie sur votre machine</p>
        <p><?php print ($_COOKIE)?></p>
        <table width="200" border="1" cellspacing="2" cellpadding="0">
            <?php 
                foreach($_COOKIE as $key => $value)
                    print ("<tr> <td>$key</td><td>$value</td></tr>");
            ?>
        </table>
        <p><a href="..">RETOUR</a></p>
        <p></p>
    </body>

</html>