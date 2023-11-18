#!/usr/bin/perl

use warnings;
use strict;

use CGI;
my $cgi = CGI->new();

print $cgi->header(-type=>'text/html');
print "<!DOCTYPE html>\n";
print "<html lang='en-US'>\n";
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
