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
	$name =~ s/(.+)\.[^.]+/$1/;
	if ($file =~ /.*$/) {
		my $fichier = $name;
		$fichier =~ s/(.+)\.[^.]+/$1/;
		system("echo $name >> $target");
		system("./bin/modelTI $file >> $target");
		#system("./bin/modelTI $file /home/mnattaf/instance_CECSP/instance_CECSP_function/convex_piecewise_linear_function/inst_first_run/inst_conv_id/TW\_ERint/$fichier\_TW\_ERint.txt >> $target 2>&1");
		#system("echo ********************************************************** >> $target");
	}
}

#system("mkdir -p $target");
explore $path;

