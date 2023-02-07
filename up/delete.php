<?php
$fileName = substr(getenv("PATH_INFO"), 1);
$deleted = false;
if (file_exists($fileName)) {
	if (is_dir($fileName)) {
		http_response_code(403);
	} else {
		$deleted = unlink($fileName);
	}
	if (!$deleted) {
		http_response_code(403);
	}
} else {
	http_response_code(404);
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
				echo $fileName . " couldn't be deleted.";
			?></p>
	</body>

</html>
