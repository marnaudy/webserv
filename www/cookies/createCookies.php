<!DOCTYPE html

    <head>
        <meta http-equiv="content-type" content="text/html;charset=utf-8"/>
        <title>Création de cookies</title>
    </head>

    <body>
        <h3>Création de <i>cookies</i></h3>
        <form action="writeCookies.php" method="post" id="FormName">
            <table width="154" border="0" cellspacing="2" cellpadding="0">
                <tr>
                    <td align="right">Nom</td>
                    <td><input type="text" name="nom" size="24"/></td>
                </tr>
                <tr>
                    <td align="right">Taille</td>
                    <td><input type="text" name="taille" size="24"/></td>
                </tr>
                <tr>
                    <td align="right">Couleur préférée</td>
                    <td><input type="text" name="couleur" size="24"/></td>
                </tr>
                <tr>
                    <td colspan="2" align="center"><input type="submit" name="submitButtonName" value="Créer le cookie"/></td>
                </tr>
            </table>
        </form>
        <p><a href="..">RETOUR</a></p>
        <p></p>
    </body>
</html>