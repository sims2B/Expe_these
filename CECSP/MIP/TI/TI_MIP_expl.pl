#!/usr/bin/perl

use strict;
use warnings;
use File::Basename;
use Cwd;

my $path = Cwd::realpath(shift);
#my $target = Cwd::realpath(shift);

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
#	system(" echo $name >> $target");
#   system("mv $file $path/$fichier\_f4.txt");
#	system("./bin/modelTI $file $target/$fichier\_LIN\_f5.txt  ");
	#		system("echo ********************************************************** >> $target");
    }
}

#system("mkdir -p $target");
explore $path;

