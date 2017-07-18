#!/usr/bin/perl -w


use Net::SMTP::SSL;
$account='forgefifteen@gmx.net';
$password="pinkfloyd";
$recipient=$ARGV[0];
$subject=$ARGV[1];
$smtp = Net::SMTP::SSL->new(
Host => 'mail.gmx.net',
Port => 465,
Timeout => 120
); # connect to an SMTP server
die "Couldn't open connection: $!" if (!defined $smtp );
$smtp->auth($account,$password);
$smtp->mail( $account ); # use the sender's address here
$smtp->to( $recipient ); # recipient's address
$smtp->data(); # Start the mail
# Send the header.
$smtp->datasend("To: $recipient\n");
$smtp->datasend("From: $account\n");
$smtp->datasend("Subject: ".$subject."\n");
$smtp->datasend("\n");
# Send the body.
$smtp->datasend($ARGV[1]."\n");
$smtp->dataend(); # Finish sending the mail
$smtp->quit; # Close the SMTP connection
print "Done!";
