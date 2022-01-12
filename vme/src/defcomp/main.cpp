#define FALSE 0
#define TRUE 1
#include "defcomp.h"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../compile_defines.h"

int main(int argc, char **argv)
{
    std::cout << "Defcomp Compiled with [" << get_compiled_hash_defines() << "]\n";
    int ln = 0, error = 0;
    char buff[1024], *save_buff = NULL;
    char in_name[1024];
    FILE *in;
    int opt, p_opt;

    while ((opt = getopt(argc, argv, "cf:")) != -1)
    {
        switch (opt)
        {
            case 'c':
                strcpy(in_name, "color.def");
                p_opt = opt;
                break;
            case 'f':
                strcpy(in_name, optarg);
                break;
            case ':':
                printf("option needs a value\n");
                exit(1);
            case '?':
                printf("unknown option: %c\n", optopt);
                break;
        }
    }

    switch (p_opt)
    {
        case 'c':
            in = fopen(in_name, "r");
            if (!in)
            {
                std::cerr << "In file not opened." << std::endl;
                exit(1);
            }

            while (fgets(buff, 1024, in))
            {
                ln++;
                if (!iscomment(buff))
                    if (!(save_buff = convert_line(buff, ln, save_buff)))
                        error++;
            }

            if (error <= 0)
                string_to_file("color.dat", save_buff);
            fclose(in);
            exit(0);
        default:
            std::cerr << "You must supply the type of Define file." << std::endl;
            std::cerr << "Example:" << std::endl;
            std::cerr << "         defcomp -c  (To convert the color.def file)" << std::endl;
            break;
    }
    exit(0);
}
