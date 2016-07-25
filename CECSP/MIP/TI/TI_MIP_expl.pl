#!/usr/bin/perl

use strict;
use warnings;
use File::Basename;
use Cwd;

my $path = Cwd::realpath(shift);
my $target = Cwd::realpath(shift);

sub explore {
	my $p = shift;
	next if ($p =~ /.*\/\.[^\/]*$/);
	$p = Cwd::realpath $p;
	if ( -d $p) {
		my $dir;
		opendir $dir,$p or die "Cannot open directory: $p";
		while (my $file = readdir($dir)) { explore("$p/$file"); }
	}
	elsif (-f $p) { handle_file($p); }
	else { warn "Neither file nor directory ??? $p\n"; }
}

sub handle_file {
	my $file = shift;
	my $name = basename($file);
	if ($file =~ /.*$/) {
	for my $i (2){
	system(" echo $name >> $target$i");
	system("./bin/modelTI $file 7200 $i >> $target$i ");
		}
}
}

#system("mkdir -p $target");
explore $path;

