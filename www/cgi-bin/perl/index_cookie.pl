#!/usr/bin/perl

use warnings;
use strict;

use CGI;
use CGI::Cookie;
my $cgi = CGI->new;
my %cookies = CGI::Cookie->fetch;
my $name_cookie = $cookies{'name'} if exists $cookies{'name'};
my $token_cookie = $cookies{'token'} if exists $cookies{'token'};
my $name_value = $name_cookie ? $name_cookie->value : 'User';
my $token_value = $token_cookie ? $token_cookie->value :'Token';
my $token = $token_cookie ? $token_cookie->value : generate_token() ;

sub generate_token {
	my @chars = ('a'..'z', 'A'..'Z', '0'..'9');
	my $token = '';
	for (1..16) {
		$token .= $chars[rand @chars];
	}
	return $token;
}

$token_cookie = CGI::Cookie->new(
	-name    => 'token',
	-value   => $token,
	# -expires => '0',
	-expires => '+5s',
);

$name_cookie = CGI::Cookie->new(
	-name    => 'name',
	-value   => 'ayman',
	# -expires => '0',
	-expires => '+5s',
);

print $cgi->header(-cookie => [$name_cookie, $token_cookie]);
print "<!DOCTYPE html>\n";
print "<html lang='en-US'>\n";
print "<head>\n";
print "<title>Cookie Example</title>\n";
print "<meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1' />\n";
print "</head>\n";
print "<body style='font-family: Arial, sans-serif;'>\n";
print "<h1>Hello, $name_value!</h1>\n";
print "<h4>Token : [$token_value]</h4>\n";
print "<button style=\"background-color: rgb(27, 252, 132);\" onClick=\"window.location.reload();\">Generate cookeis</button>\n";
print "</body>\n";
print "</html>\n";
