#!/usr/bin/perl

use strict;
use warnings;

use CGI;
my $cgi = CGI->new;
my $name = $cgi->param('name');
my $nickname = $cgi->param('nickname');


# Print the HTTP headers
print "HTTP/1.1 200 OK\r\n";
print "Content-Type: text/html\r\n";
print "\r\n";

# Print the HTML content
print "<!DOCTYPE html>\n";
print "<html>\n";
print "<head>\n";
print "<title>Perl Example</title>\n";
print "</head>\n";
print "<body>\n";
print "<h1>CGI scripts are working fine on our server!</h1>\n";
print "<h1>name:$name</h1>\n";
print "<h1>nickname:$nickname</h1>\n";
print "</body>\n";
print "</html>\n";
