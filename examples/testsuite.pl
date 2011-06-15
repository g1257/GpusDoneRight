#!/usr/bin/perl -w
use strict;
use Class::Struct;

struct( MyTest => [
name => '$',
kernel => '$',
args => '$'
 ]);
my $filename = "descriptions.txt";

my @tests;
loadTests();


my ($n)=@ARGV;

defined($n) or $n= -1;

if ($n<0) {
	foreach my $testx (@tests) {
		run($testx);
	}
} else {
	run($tests[$n]);
}

sub run
{
	my ($thisTest)=@_;
	my $testname = $thisTest->name;
	print "Running $testname\n";
	system("make $testname");
	my $kernel = $thisTest->kernel;
	$kernel =~s/ //;
	system("nvcc --ptx $kernel.cu") 
		unless ($kernel eq "" or $kernel eq "none"
		or -r "$kernel.ptx");
	my $args = $thisTest->args;
	$args =~s/none//;
	system("./$testname $args");
}

sub loadTests
{
	open(FILE,$filename) or die "Cannot open $filename: $!\n";
	while(<FILE>) {
		next if (/^#/);
		if (/\[(.*)\]/) {
			my $name = $1;
			my $kernel = <FILE>;
			chomp($kernel);
			my $args = <FILE>;
			chomp($args);
			procEntry($name,$kernel,$args);
		}
	}
	close(FILE)
}

sub procEntry
{
	my ($name,$kernel,$args)=@_;
	my $thisTest = new MyTest;
	$thisTest->name($name);
	$thisTest->kernel($kernel);
	$thisTest->args($args);
	push @tests, $thisTest;
}
	
