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
	  	system("./bin/modelOO $file 0 0 0 0 0 0 >> $target");
		system("./bin/modelOO $file 0 1 1 0 0 0 >> $target");
		system("./bin/modelOO $file 0 0 0 1 1 0 >> $target");
		system("./bin/modelOO $file 0 0 0 0 0 1 >> $target");
		system("./bin/modelOO $file 1 0 0 0 0 0 >> $target");
		system("./bin/modelOO $file 0 1 1 1 1 1 >> $target");
		system("./bin/modelOO $file 1 1 1 1 1 0 >> $target");
		system("./bin/modelOO $file 1 1 1 0 0 1 >> $target");
		system("./bin/modelOO $file 1 0 0 1 1 1 >> $target");
		system("./bin/modelOO $file 1 1 1 1 1 1 >> $target");
	}
}


#system("mkdir -p $target");
system("echo 'nom_instance;config;root_obj;obj_sol1;time_sol1;gap_sol1;time;solved;feas;opt;obj;LB;gap;nb_nodes;nb_cuts;valid' >> $target");
explore $path;
