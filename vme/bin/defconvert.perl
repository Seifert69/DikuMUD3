#!/usr/bin/perl
# File   : defconvert[.perl]	                                   -*-Perl-*-
# Version: 1.00
# Author : bakka@blinksoft.com & seifert@diku.dk
# Purpose: To convert the *.def into *.dat by running through cpp, etc.
#
$libdir = "../lib/";
$bindir = "../bin/";
$definf = "@ARGV[0]";
$defouf = "@ARGV[1]";
$deftmp = `mktemp --tmpdir=../log --suffix=.cpp`;
chomp $deftmp;
$tmplin = `mktemp --tmpdir=../log --suffix=.lin`;
chomp $tmplin;
$cpp = "../bin/vmc -q -p -I../include/ ";

system "$cpp $definf | grep -v \\# > $deftmp";
if (!open(TMPIN, $deftmp))
{
    die " echo ERROR30: Could not open file: $deftmp";
}

if (!open(TMPOUT, ">$defouf"))
{
    die " echo ERROR30: Could not create file: $defouf";
}

if (!open(MYOUT, ">$tmplin"))
{
    die " echo ERROR30: Could not create file: $tmplin";
}

close(MYOUT);

#print "Processing...\n";

while (<TMPIN>)
{
    if (/^\S/)
    {
if 	($_ =~ /^(.*)\=(.*)/){
       ($F1,$Etc) = ($_ =~ /^(.*)\=(.*)/);
	          $res = calc($Etc);
       print TMPOUT "$F1 = $res\n";
	   }else{
	          print TMPOUT "$_";
	   }

   }
}

close(TMPIN);
close(TMPOUT);

system "rm -f $tmplin $deftmp";

#system "echo Complete.";

sub calc {
  local ($str) = @_;
  local ($mstr, $ret, $temp);
  
  if ($str !~ /[0-9]/) {
    return ($str);
  }
  if ($str =~ /\$/) {
    return ($str);
  } 
  if ($str =~ /\@/) {
    return ($str);
  } 
  if ($str =~ /\'/) {
    return ($str);
  } 
  if ($str =~ /\{/) {
    $mstr = $str;
	 $mstr =~ s/\s+//g;
    @chars = split(//, $mstr);
    if ((@chars[0] eq "{") && (@chars[(length($mstr)-1)] eq "}")) {
      $str =~ s/{//g;
      $str =~ s/}//g;
	   @words = split(/,/, $str);
      $ret = join(" ", @words);
      return ($ret);
    }
  }	
  $mstr = $str;
  $mstr = "\$tmp = " . $mstr;
  eval($mstr);
  
  if ($val eq "") {
    return( $tmp );
  } else {
    die ("EVALED ERRORED");
  }
}
