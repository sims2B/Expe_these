#!/usr/bin/perl

use strict;
use warnings;
use File::Basename;
use Cwd;

my $path = Cwd::realpath(shift);
my $target = Cwd::realpath(shift);

sub explore {
    my $p=shift;
    next if ($p =~ /.*\/\.[^\/]*$/);
    $p= Cwd::realpath $p;
    if (-d $p) {
	my $dir;
	opendir $dir,$p or die "Cannot open directory: $p";
	while (my $file=readdir($dir)) { explore("$p/$file"); }
    }
    elsif (-f $p) { handle_file($p); }
    else {warn "Neither file nor directory??? $p\n"; }
}

sub handle_file {
    my $file = shift;
    my $name = basename($file);
    if ($file=~ /.*$/) {
	for my $i (1){
	    for my $j (10,5){
		for my $k (1,3,4){
		    system("echo '$name' >> $target\_h$k\_$i\_$j.txt");
		    system("timeout --signal=9 7220s ./bin/hybridBB $file $k $i $j >> $target\_h$k\_$i\_$j.txt 2>&1 ");
		    
		    system("echo '     ***********************************************************************' >> $target\_h$k\_$i\_$j.txt");
		}}
	}
    }
}

explore $path;


