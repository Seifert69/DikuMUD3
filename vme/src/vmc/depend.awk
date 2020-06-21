#!/usr/bin/gawk -f
BEGIN{}
{
if ($1~/\.o:/){
	print "../../build//vmc/"$0;
	}else{
	print $0}
	}
