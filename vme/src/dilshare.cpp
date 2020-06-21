/*
 $Author: All $
 $RCSfile: dilshare.cpp,v $
 $Date: 2003/01/03 04:08:47 $
 $Revision: 1.2 $
 */

#include "namelist.h"
#include "intlist.h"
#include "dil.h"
/* free generated temporary values */
dilval::~dilval (void)
{
    switch (type)
    {
    case DILV_SP:
        /* Only free if temporary allocated expression */
        if (val.ptr == NULL)
        {
            /*
            	slog (LOG_ALL, 0, "DIL: NULL string pointer to FREE().");
            	*/
        }
        else if (atyp == DILA_EXP)
        {
            FREE (val.ptr);
            val.ptr = NULL;
        }
        break;

    case DILV_SLP:
        /* Only free if temporary allocated expression */
        if (val.ptr == NULL)
        {
            /*
            	slog (LOG_ALL, 0, "DIL: NULL string list pointer to FREE().");
            	*/
        }
        else if (atyp == DILA_EXP)
        {
            delete ((class cNamelist *) val.ptr);
            val.ptr = NULL;
        }
        break;

    case DILV_ILP:
        /* Only free if temporary allocated expression */
        if (val.ptr == NULL)
        {
            /*
            				slog (LOG_ALL, 0, "DIL: NULL intlist pointer to FREE().");
            				*/
        }
        else if (atyp == DILA_EXP)
        {
            delete ((class cintlist *) val.ptr);
            val.ptr = NULL;
        }
        break;
    }
}
