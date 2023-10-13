#!/usr/bin/perl
print "HTTP/1.1 200 OK\r\n";
print "Content-type: text/html\r\n\r\n";

print "<html>\n";
print "<head>\n";
print "<title>Environment Information</title>\n";
print "</head>\n";

print "<body style='font-family: Arial, sans-serif;'>\n";
print "<h1 style='color: #336699;'>Environment Information</h1>\n";
print "<ul style='list-style-type: none; padding: 0;'>\n";

foreach (sort keys %ENV) {
   print "<li><strong>$_:</strong> $ENV{$_}</li>\n";
}

print "</ul>\n";
print "</body>\n";
print "</html>\n";
1;
