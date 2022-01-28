#define FALSE 0
#define TRUE 1
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>
#include <cctype>
#include <cstring>
#include <iostream>

int string_to_file(const char *name, const char *s)
{
    int out = 0;

    out = open(name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (!out)
    {
        std::cerr << "Error in opening the " << name << " file." << std::endl;
        return (FALSE);
    }

    ssize_t msout = write(out, s, strlen(s));
    assert(msout == (ssize_t)strlen(s));

    close(out);
    return (TRUE);
}

int checkcolor(char *cstr)
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
    {
        b++;
    }

    if ((*b == '#') || (*b == '\0') || (*b == '\r') || (*b == '\n'))
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

char *convert_line(char *temp, int ln, char *save_buff)
{
    int size = 0;
    char *new_buff = nullptr;
    char token[1024];
    char strval[1024];
    char *b = temp;
    int i = 0;

    while (!isgraph(*b))
    {
        b++;
    }

    i = 0;
    while ((isalnum(*b)) || (*b == '_'))
    {
        token[i] = *b;
        b++;
        i++;
    }
    token[i] = '\0';

    if (strlen(token) < 1)
    {
        std::cerr << "Erorr!  Line " << ln << ":  No keyword value given." << std::endl;
        return (nullptr);
    }

    if (strlen(token) > 20)
    {
        std::cerr << "Erorr!  Line " << ln << ":  Keyword to long." << std::endl;
        return (nullptr);
    }

    while (*b != '=')
    {
        if (!isspace(*b))
        {
            std::cerr << "Erorr!  Line " << ln << ":  Ilegal char in left hand value of '='" << std::endl;
            return (nullptr);
        }
        if ((*b == '\n') || (*b == '\r') || (*b == '\0'))
        {
            std::cerr << "Erorr!  Line " << ln << ":  Missing '='" << std::endl;
            return (nullptr);
        }
        b++;
    }

    while (*b != '"')
    {
        if ((!isspace(*b)) && (*b != '='))
        {
            std::cerr << "Erorr!  Line " << ln << ":  Ilegal char in right hand value of '='" << std::endl;
            return (nullptr);
        }
        if ((*b == '\n') || (*b == '\r') || (*b == '\0'))
        {
            std::cerr << "Erorr!  Line " << ln << ":  Missing right hand side of '='" << std::endl;
            return (nullptr);
        }
        b++;
    }

    b++;
    i = 0;
    while ((*b != '"') && (*b != '\r') && (*b != '\n') && (*b != '\0'))
    {
        strval[i] = *b;
        b++;
        i++;
    }

    strval[i] = '\0';

    if (*b != '"')
    {
        std::cerr << "Erorr!  Line:  " << ln << ":  Missing '\"'" << std::endl;
        return (nullptr);
    }

    if (strlen(strval) < 1)
    {
        std::cerr << "Erorr!  Line " << ln << ":  No value inside '\"\"'" << std::endl;
        return (nullptr);
    }

    if (checkcolor(strval))
    {
        if (save_buff)
        {
            size = strlen(save_buff) + 1;
        }
        size += strlen(token) + 1;
        size += strlen(strval) + 1;
        new_buff = new char[size];
        new_buff[0] = '\0';
        if (save_buff)
        {
            strcat(new_buff, save_buff);
            strcat(new_buff, ":");
        }
        strcat(new_buff, token);
        strcat(new_buff, ":");
        strcat(new_buff, strval);
        delete save_buff;
    }
    else
    {
        std::cerr << "Error!  Line " << ln << ":  " << strval << " is not a legal color value." << std::endl;
        return (nullptr);
    }

    return (new_buff);
}
