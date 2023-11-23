<?php
session_start();
?>
<!DOCTYPE html>
<html>
<body>

<?php
$_SESSION["cgi"] = "php";
$_SESSION["cgi1"] = "py";
$_SESSION["cgi2"] = "pl";
print_r($_SESSION);
session_unset();
session_destroy();
?>

</body>
</html>
