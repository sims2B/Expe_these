#!/usr/bin/perl

use strict;
use warnings;
use File::Basename;
use Cwd;

my $path = Cwd::realpath(shift);
my $target = Cwd::realpath(shift);
#my $target2 = Cwd::realpath(shift);
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
		for my $i (0..1){
	 		for my $j (0..1){
				for my $k (0..1){
					for my $l (0..1){
						if ($k + $l <= 1){
						for my $m (0..1){
							for my $n (0..1){
								for my $o (0..1){	    	
									if ($j+$k<=1){							
										system("echo '$name' >> $target\_$i\_$j\_$k\_$l\_$m\_$n\_$o.txt");
   									system("./bin/modelSE $file $i $j $k $l $m $n $o >> $target\_$i\_$j\_$k\_$l\_$m\_$n\_$o.txt");
system(" echo '$name' >> traite.txt");
	}}}}}}}} #fermeture de tous les for
	}
}
}

#system("mkdir -p $target");
explore $path;
