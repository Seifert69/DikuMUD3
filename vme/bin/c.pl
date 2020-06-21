#!/usr/bin/perl
use IO::Handle;
# If this does not execute correctly make sure the $perlloc is set to
#the full path of your perl installation 

$perlloc="/usr/bin/perl";
$zonedir="../zonenew";


open(CFILE, "cmd2str");

while (<CFILE>)
{  
  chop $_;
  @val = split(/\s+/,$_);
  print "$val[0], $val[1]\n";
  push (@rep, "\$str =~ s/$val[0]/\"$val[1]\"/g;\n");
}
close (CFILE);
`rm $zonedir/*.data`;
`rm $zonedir/*.err`;
`rm $zonedir/*.reset`;

opendir (ZONDIR, $zonedir) || die "Can not open ../zone/";
while ($dir = readdir(ZONDIR))
{
    if ($dir =~ /\.zon$/) 
	 {
      print "Processing $dir\n"; 
		
	   `mv $zonedir/$dir $zonedir/$dir.bak`;
	   sysopen (ZONE, "$zonedir/$dir.bak", O_RDONLY,0700);
 	   open (OZONE, ">$zonedir/$dir");
	     $x=0;
		  @stat = stat(ZONE);
	     sysread(ZONE,$str,$stat[7]) ;
	     while ($x<@rep)
		  {
		     eval("$rep[$x]");
		  $x++;
	     }
		  print OZONE $str;
	 close(ZONE);
	 close(OZONE);
  }	 
}
