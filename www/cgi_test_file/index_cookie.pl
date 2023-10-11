#!/usr/bin/perl

use strict;
use warnings;

use CGI;
use CGI::Cookie;

my $cgi = CGI->new;

# Check if the 'name' cookie already exists
my %cookies = CGI::Cookie->fetch;
my $name_cookie = $cookies{'name'} if exists $cookies{'name'};
my $token_cookie = $cookies{'token'} if exists $cookies{'token'};

# my $name = $name_cookie ? $name_cookie->value : '';
my $name_value = $name_cookie ? $name_cookie->value : '';
my $token = $token_cookie ? $token_cookie->value : generate_token();

# Create and set the 'name' cookie
$name_cookie = CGI::Cookie->new(
	-name    => 'name',
	-value   => 'ayman',
	-expires => '+30d',
);

# Create and set the 'token' cookie
$token_cookie = CGI::Cookie->new(
	-name    => 'token',
	-value   => $token,
	-expires => '+30d',
);

# Print the HTTP headers
print "HTTP/1.1 200 OK\r\n";
print $cgi->header(-cookie => [$name_cookie, $token_cookie]);

# Print the HTML content
print "<!DOCTYPE html>\n";
print "<html lang='en-US'>\n";
print "<head>\n";
print "<title>Cookie Example</title>\n";
print "<meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1' />\n";
print "</head>\n";
print "<body>\n";
# print "<h1>Hello, $name!</h1>\n";
print "<h1>Hello, $name_value!</h1>\n";
print "</body>\n";
print "</html>\n";

# Function to generate a random token
sub generate_token {
	my @chars = ('a'..'z', 'A'..'Z', '0'..'9');
	my $token = '';
	for (1..16) {
		$token .= $chars[rand @chars];
	}
	return $token;
}
