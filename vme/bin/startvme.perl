#!/usr/bin/perl 
use POSIX ":sys_wait_h";
$SIG{CHLD} = \&REAPER;                  # install *after* calling waitpid
$SIG{TERM} = \&EXIT;
$SIG{KILL} = \&EXIT;

$servercfg=$ENV{VME_SERVER_CFG};
if ( -f $servercfg )
{  
   print "Starting VME using the server config file $servercfg\n";
}  else {
   print "Environment not correctly setup, VME_SERVER_CFG not defined\n";
   exit 2;
}

#Directories
$nextzon="$vmehome/zone/nxtzon";

load_servercfg();

chdir ("$cfgvals{vmehome}/bin") || die ("Can not vme home bin directory $cfgvals{vmehome},\n update vmehome in your server config file $servercfg\n");

# Set inital status of multiplexors, vme, and other programs to 0 meaning 
# not running.  This status is checked before respawning.

$uname=`whoami`;
chomp($uname);
open(PROC, "ps -elf | grep $uname | egrep 'startvme|mplex|vme|ftpcompiler|executer' | grep -v grep |" ); 

$running = 0;
while (<PROC>)
{
  @vals =split(/\s+/, $_);
  if (@vals[14] =~ m/vme/)
  {
     if (@vals[15] =~ m/-s$servercfg/)
     {   
       if ( $running == 0)
         { print "WARNING:  Can not start VME, some VME processes are still runing\n"; }
       $running = 1;
       print "The vme process is still running PID = @vals[3]\n";
     }
  } elsif (@vals[15] =~ m/executer.perl/) {
       if ( $running == 0)
         { print "WARNING:  Can not start VME, some VME processes are still runing\n"; }
       $running = 1;
       print "The executer.perl process is still running PID = @vals[3]\n";
   } elsif (@vals[15] =~ m/ftpcompiler/) {
       if ( $running == 0)
         { print "WARNING:  Can not start VME, some VME processes are still runing\n"; }
       $running = 1;
       print "The ftpcompiler process is still running PID = @vals[3]\n";
  } elsif (@vals[14] =~ m/mplex/) {
     for $p (keys %mplex)
     {
        if (@vals[16] =~ m/$p/)
        {
       if ( $running == 0)
         { print "WARNING:  Can not start VME, some VME processes are still runing\n"; }
       $running = 1;
       print "The mplex process is still running PID = @vals[3]\n";
        }
     }
  }
}
close PROC;

if ($running == 1) {  exit 1; }


if ($mainpid = fork)
{
  print "Starting VME Processes\n";
  exit 0;
} else 
{

start_all_mplex();
start_vme();
start_executer();
start_ftpcompiler();

while ( 1 )
{
  sleep 10;
}

}

sub start_vme
{
  my ($pid);
  $pid = fork;
  if ($pid == 0)
  {
     close STDERR;
     close STDOUT;
     open (STDOUT,">>$cfgvals{logdir}/startvme.log");
     open (STDERR,">>$cfgvals{logdir}/startvme.log");
     $date = date();
     print "$date Starting VME server $cfgvals{port}\n";
     cycle_logs($cfgvals{logdir},"vme.log",$cfgvals{logdir});
     cycle_logs(".","core",".");
     system("mv $cfgvals{nextzon}/* $cfgvals{zonedir} >/dev/null 2>&1");
     exec("./vme -s$servercfg -l$cfgvals{logdir}/vme.log") ;
     exit 0;
  } else {
     $forked{$pid}="vme";
  }
}

sub start_all_mplex
{
  my ($port , $pid);
  for $port (keys %mplex)
  { 
     start_mplex($port);
  }
}

sub start_mplex
{
  my ($port,$mpid) = @_;
  $mpid = fork();
  if ( $mpid == 0 )
  {
     close STDERR;
     close STDOUT;
     open (STDOUT,">>$cfgvals{logdir}/startvme.log");
     open (STDERR,">>$cfgvals{logdir}/startvme.log");
     $date = date();
     print "$date Starting mplex on port $port\n";
     cycle_logs($cfgvals{logdir},"mplex.$port.log",$cfgvals{logdir});
     exec("./mplex -p $port -a localhost -s $cfgvals{port} $mplex{$port} -l $cfgvals{logdir}mplex.$port.log") ;
     exit 0;
  } else {
    $forked{$mpid} = $port;
  }
}

sub start_executer 
{
  my ($pid);
  if ($cfgvals{executer} > 0 )
  {
  $pid = fork;
  if ($pid == 0)
  {
     close STDERR;
     close STDOUT;
     open (STDOUT,">>$cfgvals{logdir}/startvme.log");
     open (STDERR,">>$cfgvals{logdir}/startvme.log");
     $date = date();
     print "$date Starting VME executer.perl\n";
     exec("./executer.perl") ;
     exit 0;
  } else {
     $forked{$pid}="executer";
  }
  }
}

sub start_ftpcompiler 
{
  my ($pid);
  if ($cfgvals{ftpcompiler} > 0 )
  {
  $pid = fork;
  if ($pid == 0)
  {
     close STDERR;
     close STDOUT;
     open (STDOUT,">>$cfgvals{logdir}/startvme.log");
     open (STDERR,">>$cfgvals{logdir}/startvme.log");
     $date = date();
     print "$date Starting VME ftpcompiler";
     exec("./ftpcompiler") ;
     exit 0;
  } else {
     $forked{$pid}="ftpcompiler";
  }
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


}  #load_servercfg


sub cycle_logs
{
  my($srcdir, $srcfile, $dstdir)=@_;
  if ( -f "$srcdir/$srcfile" ) 
  {
    $x=$cfgvals{log_num_keep}-1;
	 while ($x >= 0)
	 {
	   $next=$x+1;
		if ( -f "$dstdir/$x.$srcfile$cfgvals{compress_extention}" )
		{
	     system("mv -f $dstdir/$x.$srcfile$cfgvals{compress_extention} $dstdir/$next.$srcfile$cfgvals{compress_extention} >/dev/null 2>&1");
		}
      $x--;
    }

    system("mv -f $srcdir/$srcfile $dstdir/1.$srcfile");
    if (exists $cfgvals{compress_program} )
	 {
	   system("$cfgvals{compress_program} $dstdir/1.$srcfile >/dev/null 2>&1");
	 }

  }
}  #cycle_logs


sub REAPER {
  my $pid;
  while (($pid = waitpid(-1, &WNOHANG)) > 0) {
  if ( defined($forked{$pid}) ) { 
     if ($forked{$pid} =~ m/^[0-9]+$/)
      { 
         start_mplex($forked{$pid});
         undef $forked{$pid};
      } elsif ($forked{$pid} =~ m/vme/) {
         start_vme();
         undef $forked{$pid};
      } elsif ($forked{$pid} =~ m/executer/) {
         start_executer();
         undef $forked{$pid};
      } elsif ($forked{$pid} =~ m/ftpcompiler/) {
         start_ftpcompiler();
         undef $forked{$pid};
      }
}
  }
  $SIG{CHLD} = \&REAPER;                  # install *after* calling waitpid
}

sub EXIT {
my ($pid);
  for $pid (keys %forked)
  {
    $dead = kill (15, $pid);
    if ($dead != 1)
    {
       kill (9, $pid);
    }
  }
 exit 0;
}

sub date
{
     my $date;
     open (DATE, "date '+%m/%d/%y %H:%M:%S' |"); $date=<DATE>;chomp($date); close DATE;
     return $date;
}

