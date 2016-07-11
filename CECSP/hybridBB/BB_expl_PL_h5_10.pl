#!/usr/bin/perl

use strict;
use warnings;
use File::Basename;
use Cwd;

my $path = Cwd::realpath(shift);
#my $target = Cwd::realpath(shift);

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
 
	    system("echo '$name' >> /home/mnattaf/Dev2016/CECSP/res_CP/res_BB_ERFlow_h5_2_solPL.txt");
	    #system("echo '$name' >> $target2");
	    system("timeout --signal=9 4000s ./bin/hybridBB $file 5 4 2.5  2>&1 >> /home/mnattaf/Dev2016/CECSP/res_CP/res_BB_ERFlow_h5_2_solPL.txt; echo \"\$?\" >>/home/mnattaf/Dev2016/CECSP/res_CP/res_BB_ERFlow_h5_2_solPL.txt");

	    system("echo '     ***********************************************************************' >> /home/mnattaf/Dev2016/CECSP/res_CP/res_BB_ERFlow_h5_2_solPL.txt");

	    system("echo '$name' >> /home/mnattaf/Dev2016/CECSP/res_CP/res_BB_TTFlow_h5_2_solPL.txt");
	    #system("echo '$name' >> $target2");
	    system("timeout --signal=9 4000s ./bin/hybridBB $file 5 3 2.5  2>&1 >> /home/mnattaf/Dev2016/CECSP/res_CP/res_BB_TTFlow_h5_2_solPL.txt; echo \"\$?\" >>/home/mnattaf/Dev2016/CECSP/res_CP/res_BB_TTFlow_h5_2_solPL.txt");

	    system("echo '     ***********************************************************************' >> /home/mnattaf/Dev2016/CECSP/res_CP/res_BB_TTFlow_h5_2_solPL.txt");

	    system("echo '$name' >> /home/mnattaf/Dev2016/CECSP/res_CP/res_BB_ER_h5_2_solPL.txt");
	    #system("echo '$name' >> $target2");
	    system("timeout --signal=9 4000s ./bin/hybridBB $file 5 1 2.5  2>&1 >> /home/mnattaf/Dev2016/CECSP/res_CP/res_BB_ER_h5_2_solPL.txt; echo \"\$?\" >>/home/mnattaf/Dev2016/CECSP/res_CP/res_BB_ER_h5_2_solPL.txt");

	    system("echo '     ***********************************************************************' >> /home/mnattaf/Dev2016/CECSP/res_CP/res_BB_ER_h5_2_solPL.txt");
#	    system("echo '     ***********************************************************************' >> $target2");
    }
}

explore $path;

