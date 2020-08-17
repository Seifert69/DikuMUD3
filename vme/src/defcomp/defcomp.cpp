#define   FALSE 0
#define   TRUE   1
#include  <ctype.h>
using namespace std;
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <assert.h>

int string_to_file(const char *name,const char *s)
{
    int out;


    out = open(name, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

    if (!out)
    {
        cerr<<"Error in opening the "<<name<<" file."<<endl;
        return (FALSE);
    }

    ssize_t msout = write(out, s,strlen(s));
    assert(msout == (ssize_t) strlen(s));

    close (out);
    return (TRUE);
}


int checkcolor( char *cstr )
{
    return TRUE;
 /* deprecated   
    int bright;

    if ( (strlen(cstr) > 7) || (strlen(cstr) == 0 ) )
        return FALSE;

    if ( *cstr != '&' )
        return FALSE;

    cstr++;

    if ( tolower(*cstr) != 'c' )
        return FALSE;

    cstr++;

    bright = TRUE;
    while ( bright )
    {

        switch (*cstr)
        {
        case '+':
            bright = TRUE;
            cstr++;
            continue;
        case 'n':
            bright = FALSE;
            break;
        case 'r':
            bright = FALSE;
            break;
        case 'g':
            bright = FALSE;
            break;
        case 'y':
            bright = FALSE;
            break;
        case 'b':
            bright = FALSE;
            break;
        case 'm':
            bright = FALSE;
            break;
        case 'c':
            bright = FALSE;
            break;
        case 'w':
            bright = FALSE;
            break;
        default:
            return FALSE;
        }
    }

    cstr++;

    if ( *cstr != '&' )
        return FALSE;

    cstr++;

    if ( tolower(*cstr) != 'b' )
        return FALSE;

    cstr++;

    switch (*cstr)
    {
    case 'n':
        break;
    case 'r':
        break;
    case 'g':
        break;
    case 'y':
        break;
    case 'b':
        break;
    case 'm':
        break;
    case 'c':
        break;
    case 'w':
        break;
    default:
        return FALSE;
    }

    return TRUE;*/
}


int iscomment(char *b)
{
    while (!isgraph(*b))
        b++;

    if ((*b=='#')||
            (*b=='\0') ||
            (*b=='\r') ||
            (*b=='\n'))
        return (1);
    else
        return (0);
}

char * convert_line(char *temp,int ln,char *save_buff)
{
    int size=0;
    char *new_buff;
    char token[1024], strval[1024],*b=temp;
    int i=0;

    while (!isgraph(*b))
        b++;

    i=0;
    while ((isalnum(*b)) || (*b=='_'))
    {
        token[i]=*b;
        b++;
        i++;
    }
    token[i]='\0';

    if ( strlen(token)<1)
    {
        cerr<<"Erorr!  Line "<<ln<<":  No keyword value given."<<endl;
        return (NULL);
    }

    if (strlen(token)>20)
    {
        cerr<<"Erorr!  Line "<<ln<<":  Keyword to long."<<endl;
        return (NULL);
    }

    while (*b!='=')
    {
        if (!isspace(*b))
        {
            cerr<<"Erorr!  Line "<<ln<<":  Ilegal char in left hand value of '='"<<endl;
            return (NULL);
        }
        if ((*b=='\n') ||
                (*b=='\r') ||
                (*b=='\0'))
        {
            cerr <<"Erorr!  Line "<<ln<<":  Missing '='"<<endl;
            return (NULL);
        }
        b++;
    }

    while (*b!='"')
    {
        if ((!isspace(*b)) &&
                (*b!='='))
        {
            cerr<<"Erorr!  Line "<<ln<<":  Ilegal char in right hand value of '='"<<endl;
            return (NULL);
        }
        if ((*b=='\n') ||
                (*b=='\r') ||
                (*b=='\0'))
        {
            cerr <<"Erorr!  Line "<<ln<<":  Missing right hand side of '='"<<endl;
            return (NULL);
        }
        b++;
    }

    b++;
    i=0;
    while ((*b!='"') &&
            (*b!='\r') &&
            (*b!='\n') &&
            (*b!='\0'))
    {
        strval[i]=*b;
        b++;
        i++;
    }

    strval[i]='\0';

    if (*b!='"')
    {
        cerr <<"Erorr!  Line:  "<<ln<<":  Missing '\"'"<<endl;
        return (NULL);
    }

    if (strlen(strval)<1)
    {
        cerr<<"Erorr!  Line "<<ln<<":  No value inside '\"\"'"<<endl;
        return (NULL);
    }

    if (checkcolor(strval))
    {
        if (save_buff)
            size=strlen(save_buff)+1;
        size+=strlen(token)+1;
        size+=strlen(strval)+1;
        new_buff=new char[size];
        new_buff[0]='\0';
        if (save_buff)
        {
            strcat(new_buff,save_buff);
            strcat (new_buff,":");
        }
        strcat (new_buff,token);
        strcat (new_buff,":");
        strcat(new_buff,strval);
        delete save_buff;
    }
    else
    {
        cerr<<"Error!  Line "<<ln<<":  "<<strval<<" is not a legal color value."<<endl;
        return(NULL);
    }


    return (new_buff);
}

int main (int argc, char **argv)
{
    int ln=0,error=0;
    char buff[1024],*save_buff=NULL;
    char in_name[1024];
    FILE *in;
    int opt,p_opt;

    while((opt = getopt(argc, argv, "cf:")) != -1) {
        switch(opt) {
        case 'c':
            strcpy (in_name,"color.def");
            p_opt=opt;
            break;
        case 'f':
            strcpy (in_name,optarg);
            break;
        case ':':
            printf("option needs a value\n");
            exit(1);
        case '?':
            printf("unknown option: %c\n", optopt);
            break;
        }
    }


    switch (p_opt) {
    case 'c':
        in=fopen(in_name,"r");
        if (!in)
        {
            cerr<<"In file not opened."<<endl;
            exit (1);
        }

        while (fgets(buff,1024,in))
        {
            ln++;
            if (!iscomment(buff))
                if (!(save_buff=convert_line(buff,ln,save_buff)))
                    error++;
        }

        if (error<=0)
            string_to_file("color.dat",save_buff);
        fclose(in);
        exit (0);
    default:
        cerr<<"You must supply the type of Define file."<<endl;
        cerr<<"Example:"<<endl;
        cerr<<"         defcomp -c  (To convert the color.def file)"<<endl;
        break;
    }
    exit (0);
}
