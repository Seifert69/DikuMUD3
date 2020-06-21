#! /bin/awk -f
BEGIN{RS="}"
buf=""
buf2="";
}
{
split($0,a,"{");
buf=buf""a[1];
}

END{
ret=split (buf,b,"*/");
for (i=0;i<=ret;i++){
split(b[i],c,"/*");
buf2=buf2""c[1];
}

buf="";
ret=split (buf2,a,"\n");
for (i=0;i<ret;i++){
if (a[i]~/^\#/){
a[i]="";
}

if (a[i]~/.*=.*/){
a[i]="";
}

if (a[i]~/.*if.*/){
a[i]="";
}

if (a[i]~/.*\".*/){
a[i]="";
}


if (a[i]~/.*\(LOG_.*/){
a[i]="";
}

if (a[i]~/.*&&.*/){
a[i]="";
}

if (a[i]~/.*\|\|.*/){
a[i]="";
}



if (a[i]~/.*switch.*/){
a[i]="";
}


if (a[i]~/.*;.*/){
a[i]="";
}

if (a[i]~/.*->.*/){
a[i]="";
}






if (a[i]~/[a-zA-Z0-9].*if.*\(/){
a[i]="";
}

if (a[i]~/.*\(.*\(.*/){
a[i]="";
}

if (a[i]~/.*\).*\).*/){
a[i]="";
}



gsub(/\r/,"",a[i]);
gsub(/\n/,"",a[i]);
if (a[i]~/.*\(.*\).*/){
if (a[i]!=""){
buf=buf"\n"a[i];}}
}




print buf;
}

