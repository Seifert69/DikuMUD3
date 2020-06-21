#!/usr/bin/perl

$servercfg=$ENV{VME_SERVER_CFG};
if ( -f $servercfg )
{  
   print "Starting VME executer using the server config file $servercfg\n";
}  else {
   print "Environment not correctly setup, VME_SERVER_CFG not defined\n";
   exit 2;
}

load_servercfg();
$datfile = "$cfgvals{libdir}/execute.dat";

for (;;)
{
  sleep(30);
  if (!(-r "$datfile")) {
    next;
  }

  open(DAT, "$datfile");
  @lines = <DAT>;
  close(DAT);
  unlink("$datfile");
  $x = 0;
  $pram = "";
  while($x < @lines) {
    @vars = split(/\s+/, $lines[$x]);
    $fname = $vars[0];
    splice(@vars,0,1);
    foreach $var (@vars) {
	   $var =~ s/\@/\\@/g;
	   $var =~ s/\$/\$/g;
      $pram = $pram . "\"$var\",";
    }
	 chop($pram);
	 $fcall = "$fname($pram);";
	 eval($fcall);
	 $x ++;
	 $pram = "";
  }

}

sub restore {
    local ($name, $day, $opt, $mail) = @_;
    $bpath = "/home/val/Backup";
    $plib = "/home/val/vme/lib/ply";

    if ($name eq "") {
	return;
    }

    if ($day == 0) {
	$day =1 ;
    }

    if ($opt eq "") {
	$opt = "full";
    }
    $c = substr($name,0,1);


    if ($opt =~ /inv/) {
	chdir("$bpath");
	`tar zxvf $day.ply.tgz ply/$c/$name.inv >/dev/null 2>&1`;
	if (-s "$bpath/ply/$c/$name.inv") {
           `mv $bpath/ply/$c/$name* $plib/$c >/dev/null 2>&1`;
if ($mail ne "") {
  `mail -s \"PLAYER $name INVENTORY RESTORED\" $mail </dev/null`;
}
         } else {
if ($mail ne "") {
  `mail -s \"ERROR RESTORING PLAYER $name INVENTORY\" $mail </dev/null`;
}
return;
        }
    }
    if ($opt =~ /ful/) {
	chdir("$bpath");
	`tar zxvf $day.ply.tgz ply/$c/$name.inv >/dev/null 2>&1`;
	`tar zxvf $day.ply.tgz ply/$c/$name >/dev/null 2>&1`;
	if (-s "$bpath/ply/$c/$name") {
           `mv $bpath/ply/$c/$name* $plib/$c >/dev/null 2>&1`;
if ($mail ne "") {
  `mail -s \"PLAYER $name FULLY RESTORED\" $mail </dev/null`;
}
         } else {
        if ($mail ne "") {
  `mail -s \"ERROR RESTORING PLAYER $name\" $mail </dev/null`;
         }
           return;
        }
    }

    chdir($bpath);
    `rm -rf ply`;
    return;
}

sub mail {
  local ($request, $zone, $address) = @_;
  local ($zon) = "/home/val/vme/zone";
                            local ($nxtzon) = "/home/val/vme/zone/nxtzon";
  if ($request ne "zone") {
    return;
  } elsif ($zone eq "zonelist") {
    `mail -s \"Valhalla ZONELIST transmission\" $address < ../etc/zonelist`;
  } elsif (-r "$nxtzon/$zone.zon") {
      `mail -s \"Valhalla ZONE $zone transmission\" $address < $nxtzon/$zone.zon `;
    } elsif (-r "$zon/$zone.zon") {
      `mail -s \"Valhalla ZONE $zone transmission\" $address < $zon/$zone.zon `;
    } else {
	   `mail -s \"Valhalla ZONE transmission FAILURE (NO ZONE $zone)\" $address </dev/null`;
    }
	}
  
sub syslog {
  local ($num, $address) = @_;
  local ($logs)="/home/val/vme/log";

  if ($num < 0) {
  $num = 0
  } elsif ($zone > 8) {
  $num = 8;
  }
if ($num == 0 ) {
      `mail -s \"Valhalla Syslog transmission\" $address < $logs/vme.log`;
      }else{
      `gzip -d $logs/$num.vme.log.gz`;
      `mail -s \"Valhalla Syslog transmission\" $address < $logs/$num.vme.log`;
      `gzip $logs/$num.vme.log`;
      }
                                                                            }

sub load_servercfg
{

open(CFG, "$servercfg");
while(<CFG>) 
{  
   if (!(($_ =~ /^#/) || ($_ =~ /^ *$/)))
   {
	   s/[\s\t]+=[\s\t]+/=/g;
	   s/~//g;
	   s/\"//g;
		chomp($_);
		@val=split("=",$_);
		$val[0] =~ tr/[A-Z]/[a-z]/;
		$val[0] =~ s/\s+/_/;
                if ( /mplex*\[/ )
                {  
                   $val[0] =~ s/\]//g;
                   @p = split(/\[/, $val[0]);
                   $mplex{$p[1]}=$val[1];
                } else {
		  $cfgvals{$val[0]}=$val[1];
                }
   }

}

close(CFG);

if ( $cfgvals{debug} > 0)
{
  for $key (sort keys %cfgvals)
  {
   print "$key = $cfgvals{$key}\n";
  }
  for $key (sort keys %mplex)
  {
   print "mplex $key = $mplex{$key}\n";
  }
}

}

                                                                          
