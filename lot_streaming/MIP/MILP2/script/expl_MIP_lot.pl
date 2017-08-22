#!/usr/bin/perl

use strict;
use warnings;
use File::Basename;
use Cwd;

my $path = Cwd::realpath(shift);
my $target2 = Cwd::realpath(shift);
my $target3 = Cwd::realpath(shift);
my $target4 = Cwd::realpath(shift);

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
	    system("echo '$name' >> $target2");
	    system("echo '$name' >> $target3");
	    system("echo '$name' >> $target4");
	    system("timeout --signal=9 7200s ./bin/modelLotSE $file 2 2>&1 >> $target2; echo \"\$?\" >>$target2");
	    system("timeout --signal=9 7200s ./bin/modelLotSE $file 3 2>&1 >> $target3; echo \"\$?\" >>$target3");
	    system("timeout --signal=9 7200s ./bin/modelLotSE $file 4 2>&1 >> $target4; echo \"\$?\" >>$target4");

	    system("echo '     ***********************************************************************' >> $target2");
	    system("echo '     ***********************************************************************' >> $target3");	
	    system("echo '     ***********************************************************************' >> $target4");

    }
}

explore $path;

