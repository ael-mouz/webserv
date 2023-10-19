#!/usr/bin/php
<?php
header('HTTP/1.1 200 OK');
header('Content-type: text/html');

echo "<html>";
echo "<head><title>Environment Variables</title></head>";
echo "<body>";
echo "<h1>Environment Variables</h1>";
echo "<ul>";

foreach ($_SERVER as $key => $value) {
    echo "<li><strong>{$key}:</strong> {$value}</li>";
}

echo "</ul>";
echo "</body>";
echo "</html>";
?>