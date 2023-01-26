<?php
$fileName = substr(getenv("PATH_INFO"), 1);
$deleted = false;
if (file_exists($fileName)) {
	$deleted = true;
	unlink($fileName);
}
?>
<!DOCTYPE html
    <head>
        <meta http-equiv="content-type" content="text/html;charset=ISO-8859-1"/>
        <title>Delete file</title>
    </head>

    <body bgcolor="#ffffff">
        <p><?php
			if ($deleted)
				echo $fileName . " has been deleted.";
			else
				echo $fileName . " doesn't exist.";
			?></p>
	</body>

</html>
