#!/usr/bin/perl

use strict;
use warnings;

use CGI;
my $cgi = CGI->new;
my $name = $cgi->param('name');
my $nickname = $cgi->param('nickname');

print $cgi->header(-type =>'text/html');
print "<!DOCTYPE html>\n";
print "<html>\n";
print "<head>\n";
print "<title>Perl Example</title>\n";
print "</head>\n";
print "<body>\n";
print "<h1>name:$name</h1>\n";
print "<h1>nickname:$nickname</h1>\n";
print "</body>\n";
print "</html>\n";
