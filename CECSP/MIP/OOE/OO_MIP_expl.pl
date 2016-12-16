#!/usr/bin/perl

use strict;
use warnings;
use File::Basename;
use Cwd;

my $path = Cwd::realpath(shift);
my  $target = Cwd::realpath(shift);
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
		system("echo '$name' >> $target\_0\_0\_0\_1\_1\_0\_0\_0\_0.txt");
		system("./bin/modelOO $file 0 0 0 1 1 0 0 0 0 >> $target\_0\_0\_0\_1\_1\_0\_0\_0\_0.txt");
		system("echo '$name' >> $target\_0\_1\_1\_0\_1\_0\_0\_0\_0.txt");
		system("./bin/modelOO $file 0 1 1 0 1 0 0 0 0 >> $target\_0\_1\_1\_0\_1\_0\_0\_0\_0.txt");
		system("echo '$name' >> $target\_0\_0\_0\_0\_1\_1\_1\_0\_0.txt");
		system("./bin/modelOO $file 0 0 0 0 1 1 1 0 0 >> $target\_0\_0\_0\_0\_1\_1\_1\_0\_0.txt");
		system("echo '$name' >> $target\_0\_0\_0\_1\_1\_0\_0\_1\_0.txt");
		system("./bin/modelOO $file 0 0 0 1 1 0 0 1 0 >> $target\_0\_0\_0\_1\_1\_0\_0\_1\_0.txt");
		system("echo '$name' >> $target\_1\_0\_0\_1\_1\_0\_0\_0\_0.txt");
		system("./bin/modelOO $file 1 0 0 1 1 0 0 0 0 >> $target\_1\_0\_0\_1\_1\_0\_0\_0\_0.txt");
		system("echo '$name' >> $target\_0\_1\_1\_0\_1\_1\_1\_1\_0.txt");
		system("./bin/modelOO $file 0 1 1 0 1 1 1 1 0 >> $target\_0\_1\_1\_0\_1\_1\_1\_1\_0.txt");
		system("echo '$name' >> $target\_1\_1\_1\_0\_1\_1\_1\_0\_0.txt");
		system("./bin/modelOO $file 1 1 1 0 1 1 1 0 0 >> $target\_1\_1\_1\_0\_1\_1\_1\_0\_0.txt");
		system("echo '$name' >> $target\_1\_1\_1\_0\_1\_0\_0\_1\_0.txt");
		system("./bin/modelOO $file 1 1 1 0 1 0 0 1 0 >> $target\_1\_1\_1\_0\_1\_0\_0\_1\_0.txt");
		system("echo '$name' >> $target\_1\_0\_0\_1\_1\_1\_1\_1\_0.txt");
		system("./bin/modelOO $file 1 0 0 1 1 1 1 1 0 >> $target\_1\_0\_0\_1\_1\_1\_1\_1\_0.txt");
		system("echo '$name' >> $target\_1\_1\_1\_0\_1\_1\_1\_1\_0.txt");
		system("./bin/modelOO $file 1 1 1 0 1 1 1 1 0 >> $target\_1\_1\_1\_0\_1\_1\_1\_1\_0.txt");
	}
}


#system("mkdir -p $target");
explore $path;
