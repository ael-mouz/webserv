<?php
header('Content-type: text/html');

echo "<html>\n";
echo "<head><title>Environment Variables</title></head>\n";
echo "<body>\n";
echo "<h1>Environment Variables</h1>\n";
echo "<ul>\n";

foreach ($_SERVER as $key => $value) {
    echo "<li><strong>{$key}:</strong> {$value}</li>\n";
}

echo "</ul>\n";
echo "</body>\n";
echo "</html>\n";
?>