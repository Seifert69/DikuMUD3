#!/bin/gawk -f
BEGIN {}

$1=="dilbegin"{
dilstart="";
dilrest="";
dilnew="";
found=0;


dilstart=$0;
while ($1!~/dilend.*/){
getline;
if ($1~/dilend.*/){
continue;
}
if (dilnew==""){
if ($0~/external/){
dilstart=dilstart"\n"$0;
            dilnew="\ninteger walk_room_u@function(u:unitptr);\n";
dilrest="\n";
}else if ($0~/var/){
            dilnew="\nexternal\ninteger walk_room_u@function(u:unitptr);\n";
dilrest=$0"\n";
}else if ($0~/code/){
dilnew="\nexternal\ninteger walk_room_u@function(u:unitptr);\n";
dilrest=$0"\n";
}else {

dilstart=dilstart"\n"$0;
}}else{
 if ($0~/walkto/){
sub (/walkto/,"walk_room_u@function",$0);
found=1;
}
dilrest=dilrest"\n"$0;
}
}
dilrest=dilrest""$0"\n";
if (found==1){
$0=dilstart""dilnew""dilrest;
}else{
$0=dilstart""dilrest;
}
}


$2=="SFUN_TEAMWORK"{
$1="";
$2="";
$0="dilcopy teamwork@function("$0");\n";
}

$2=="SFUN_HIDEAWAY"{
                                      $0="dilcopy hideaway@function("$4");\n";
}

	$2=="SFUN_RESCUE"{
$1="";
$2="";
$0="dilcopy rescue@function("$0");\n";
	}

	$2=="SFUN_SCAVENGER"{
$0="dilcopy fido@function(\"$1n slowly devours $2n, crunching the bones.\",\n\"$1n grabs $2n and hungrily munches it.\");\n";
	}

	$2=="SFUN_AGGRESSIVE"{
$0="dilcopy aggressive@function (0, -1, 0, 0, 3, PULSE_SEC*60, 0, \"ANY\", {\"\",\"\"});\n";
	}

	$2=="SFUN_OBEY"{
$0="dilcopy obey@function();\n";
	}

	$2=="SFUN_RANDOM_ZONE_MOVE"{
$0="dilcopy wander_zones@function(\"@loadzone\", 60, 1, 0);\n";
	}

	$2=="SFUN_RANDOM_GLOBAL_MOVE"{
$0="dilcopy global_wander@function(60,0,0);\n";
	}

	$2=="SFUN_COMBAT_POISON"{
$0="dilcopy comb_poison@function(\"You feel very ill as $1n bites you!\",\"$1n sinks $1s fangs deep into $3n!\");\n";
	}

	$2=="SFUN_COMBAT_MAGIC_HEAL"{
split ($0,a,"\"cast ");
$0="dilcopy combat_mag@function(\"\",\""a[2]", 25, 1);\n";
}


	$2=="SFUN_COMBAT_MAGIC"{
split ($0,a,"\"cast ");
$0="dilcopy combat_mag@function(\""a[2]",\"\", 25, 1);\n";
}


	$2=="SFUN_GUARD_WAY"{
split ($0,a,"+");
$0="dilcopy guard_dir@function("a[2]", null, {\"\"}, \"\");\n";
}

	$2=="SFUN_GUARD_DOOR"{
split ($0,a,"@");
split(a[2],b," ");
b[1]=toupper(b[1]);
$0="dilcopy guard_door@function("b[1]",\""a[4]"\",\""a[5]");\n" ;
}

                          	$2=="SFUN_MERCENARY_HIRE"{
$0="dilcopy mercenary_hire@function();\n";
}

$2=="SFUN_DEATH_ROOM"{
gsub(/\"/,"",$0);
$0="dilcopy death_room@function("$4","$5",\"\");\n";
}

{
print $0;
}	

