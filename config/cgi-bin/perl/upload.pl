#!/usr/bin/perl

# use strict;
# use warnings;

# use CGI;

# my $cgi = CGI->new;

# # Check if the "file" parameter is defined in the request
# if (my $filehandle = $cgi->upload("file")) {
#     my $filename = $cgi->param("file");

#     # Process the uploaded file
#     open my $output_file, '>', $filename or die "Failed to open file: $!";
#     while (my $chunk = <$filehandle>) {
#         print $output_file $chunk;
#     }
#     close $output_file;

#     print "Content-Type: text/html\n\n";
#     print "File '$filename' has been uploaded and saved.";
# } else {
#     print "Content-Type: text/html\n\n";
#     print "No file uploaded.";
# }

# use strict;
# use warnings;

# use CGI;

# my $cgi = CGI->new;

# if ($cgi->param("file")) {
#     print "Content-Type: text/html\n\n";
#     print "there is a file";
# } else {
#     print "Content-Type: text/html\n\n";
#     print "No file";
# }

#!/usr/bin/perl

use strict;
use warnings;

use CGI;

my $cgi = CGI->new;

eval {
    # Check if the "file" parameter is defined in the request
    if (my $filehandle = $cgi->upload("file")) {
        my $filename = $cgi->param("file");

        # Process the uploaded file
        open my $output_file, '>', $filename or die "Failed to open file: $!";
        while (my $chunk = <$filehandle>) {
            print $output_file $chunk;
        }
        close $output_file;

        print "Content-Type: text/html\n\n";
        print "File '$filename' has been uploaded and saved.";
    } else {
        print "Content-Type: text/html\n\n";
        print "No file uploaded.";
    }
};

if ($@) {
    # Handle the error here, e.g., log it or display a user-friendly message
    print "Content-Type: text/html\n\n";
    print "An error occurred during the file upload: $@";
}

