#!/usr/bin/perl

use strict;
use warnings;
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
    if ($file=~ /.*$/) {
    	for my $i1 (0..1){
    		for my $i2 (0..1){
    			for my $i3 (0..1){
    				for my $i4 (0..1){
   					 	for my $i5 (0..1){
  						  	for my $i6 (0..1){
  						  		system(" echo $i1 $i2 $i3 $i4 $i5 $i6 >> $target");
								system("./bin/modelOO $file $i1 $i2 $i3 $i4 $i5 $i6 >> $target");
								}
							}
						}
					}
				}
			}
        }
}

explore $path;


