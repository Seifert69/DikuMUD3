#include <assert.h>
#include <stdlib.h>
#include <string.h>

#undef malloc
#undef calloc
#undef realloc
#undef free

// This pattern used for traditional C malloc/free
char creg_pattern[8] = {42, 43, 44, 45, 46, 47, 48, 49};
char creg_free_pattern[8] = {50, 51, 52, 53, 54, 55, 56, 57};

// This pattern used for C++ new/delete
char cpp_pattern[8] = {21, 22, 23, 24, 25, 26, 27, 28};
char cpp_free_pattern[8] = {7, 8, 9, 10, 11, 12, 13, 14};

// ==============================================================

int membug_bread_ubit32(char **b)
{
   int i;

   assert(sizeof(int) == 4);  // If this isn't true, pick the type that is 4 bytes long

   memcpy((char *)&i, *b, sizeof(int));
   *b += sizeof(int);

   return i;
}

void membug_bwrite_ubit32(char **b, int i)
{
   assert(sizeof(int) == 4);  // If this isn't true, pick the type that is 4 bytes long

   memcpy(*b, (char *)&i, sizeof(int));
    *b += sizeof(int);
}

// Call only if you dont know if a piece of memory is allocated via new or malloc
void membug_verify(void *ptr)
{
   size_t size;
   char *p = (char *)ptr;

   if (!ptr)
      return;

   p -= 12;
   p += 8;
   size = membug_bread_ubit32(&p);
   assert(size > 16 + 4);

   p = (char *)ptr;
   p -= 12;

   for (size_t i = 0; i < 8; i++)
   {
      assert((p[i] == cpp_pattern[i]) || (p[i] == creg_pattern[i]));
   }

   for (size_t i = 0; i < 8; i++)
   {
      assert((p[size - 8 + i] == cpp_pattern[i]) || (p[size - 8 + i] == creg_pattern[i]));
   }
}

// ==============================================================

void membug_setfree_class(void *ptr)
{
   size_t size;
   char *p = (char *)ptr;

   p -= 12;
   p += 8;
   size = membug_bread_ubit32(&p);
   assert(size > 16 + 4);

   p = (char *)ptr;
   p -= 12;

   for (size_t i = 0; i < 8; i++)
   {
      assert(p[i] == cpp_pattern[i]);
      p[i] = cpp_free_pattern[i];
   }

   for (size_t i = 0; i < 8; i++)
   {
      assert(p[size - 8 + i] == cpp_pattern[i]);
      p[size - 8 + i] = cpp_free_pattern[i];
   }
}

// Call to verify C++ object
void membug_verify_class(void *ptr)
{
   size_t size;
   char *p = (char *)ptr;

   if (!ptr)
      return;

   p -= 12;
   p += 8;
   size = membug_bread_ubit32(&p);
   assert(size > 16 + 4);

   p = (char *)ptr;
   p -= 12;

   for (size_t i = 0; i < 8; i++)
   {
      assert(p[i] == cpp_pattern[i]);
   }

   for (size_t i = 0; i < 8; i++)
   {
      assert(p[size - 8 + i] == cpp_pattern[i]);
   }
}

//
// Call to allocate new for C++ (use emplacement, see docs).
void *membug_new(size_t size)
{
   char *p;

   assert(size > 0);

   size += 16 + 4;
   p = (char *)malloc(size);
   assert(p);

   // The first and last 8 bytes are 42 per byte
   memcpy(p, cpp_pattern, sizeof(creg_pattern));
   char *u = p + 8;
   membug_bwrite_ubit32(&u, size);
   memcpy(p + size - 8, cpp_pattern, sizeof(creg_pattern));
   membug_verify(p + 12);

   return p + 12;
}

//
// Call to delete C++ class
//
void membug_delete(void *ptr)
{
   membug_verify_class(ptr);
   membug_setfree_class(ptr);
   char *p = (char *)ptr;
   p -= 12;
   free(p);
}

// ==============================================================

void membug_setfree_reg(void *ptr)
{
   size_t size;
   char *p = (char *)ptr;

   p -= 12;
   p += 8;
   size = membug_bread_ubit32(&p);
   assert(size > 16 + 4);

   p = (char *)ptr;
   p -= 12;

   for (size_t i = 0; i < 8; i++)
   {
      assert(p[i] == creg_pattern[i]);
      p[i] = creg_free_pattern[i];
   }

   for (size_t i = 0; i < 8; i++)
   {
      assert(p[size - 8 + i] == creg_pattern[i]);
      p[size - 8 + i] = creg_free_pattern[i];
   }
}

// Call to verify C regular object
void membug_verify_reg(void *ptr)
{
   size_t size;
   char *p = (char *)ptr;

   if (!ptr)
      return;

   p -= 12;
   p += 8;
   size = membug_bread_ubit32(&p);
   assert(size > 16 + 4);

   p = (char *)ptr;
   p -= 12;

   for (size_t i = 0; i < 8; i++)
   {
      assert(p[i] == creg_pattern[i]);
   }

   for (size_t i = 0; i < 8; i++)
   {
      assert(p[size - 8 + i] == creg_pattern[i]);
   }
}

// Call to malloc
void *membug_malloc(size_t size)
{
   char *p;

   assert(size > 0);

   size += 16 + 4;
   p = (char *)malloc(size);
   assert(p);

   // The first and last 8 bytes are 42 per byte
   memcpy(p, creg_pattern, sizeof(creg_pattern));
   char *u = p + 8;
   membug_bwrite_ubit32(&u, size);
   memcpy(p + size - 8, creg_pattern, sizeof(creg_pattern));
   membug_verify_reg(p + 12);

   return p + 12;
}

// Call to calloc
void *membug_calloc(size_t nmemb, size_t size)
{
   char *p;

   assert(size * nmemb > 0);

   size = size * nmemb + 16 + 4;
   p = (char *)calloc(size, 1);
   assert(p);

   // The first and last 8 bytes are 42 per byte
   memcpy(p, creg_pattern, sizeof(creg_pattern));
   char *u = p + 8;
   membug_bwrite_ubit32(&u, size);
   memcpy(p + size - 8, creg_pattern, sizeof(creg_pattern));

   membug_verify_reg(p + 12);

   return p + 12;
}

// Call to realloc
void *membug_realloc(void *ptr, size_t size)
{
   char *p;

   membug_verify_reg(ptr);

   assert(size > 0);

   size += 16 + 4;

   p = (char *)ptr;
   p -= 12;
   p = (char *)realloc(p, size);
   assert(p);

   // The first and last 8 bytes are 42 per byte
   memcpy(p, creg_pattern, sizeof(creg_pattern));
   char *u = p + 8;
   membug_bwrite_ubit32(&u, size);
   memcpy(p + size - 8, creg_pattern, sizeof(creg_pattern));

   membug_verify_reg(p + 12);

   return p + 12;
}

void membug_free(void *ptr)
{
   membug_verify_reg(ptr);
   membug_setfree_reg(ptr);
   char *p = (char *)ptr;
   p -= 12;
   free(p);
}
