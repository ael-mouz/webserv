#!/usr/bin/perl
use warnings;
use strict;
use CGI;
my $cgi = CGI->new();

my $content = "<!DOCTYPE html>\n";
$content .="<html>\n";
$content .="<head>\n";
$content .="<title>Perl Example</title>\n";
$content .="</head>\n";
$content .="<body>\n";
$content .="<h1>CGI scripts!</h1>\n";
$content .="</body>\n";
$content .="</html>\n";

my $content_length = length($content);

print $cgi->header(-type => 'text/plain');
print $content
