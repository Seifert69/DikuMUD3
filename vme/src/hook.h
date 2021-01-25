/*
 $Author: All $
 $RCSfile: select.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.3 $
 */

#ifndef SELECT_H
#define SELECT_H

#include "essential.h"
#include "queue.h"

#ifdef _WINDOWS
#include <winsock.h>
#else
#include <sys/select.h>
#endif

extern int bPipeSignal;

int PipeRead(int fd, char *buf, size_t count);
int PipeWrite(int fd, char *buf, size_t count);

#define SELECT_READ 0x01
#define SELECT_WRITE 0x02
#define SELECT_EXCEPT 0x04

class cCaptainHook;

// 2020
//
// This is really what the basic Hook class should have looked like in 1998.
// It's a bit of a mess. Now I'm unsing this "Navtive" variant only
// for wrapping my named pipe integration. Maybe someday I can inherit
// the Native hook class to the cHook which then becomes the buffering layer.
//
class cHookNative
{
public:
   cHookNative(void);
   virtual ~cHookNative();

   void Hook(int fd);
   int IsHooked(void);
   virtual void Unhook(void);

   int get_fd(void);

   int write(const void *buf, int count);
   int read(void *buf, int count);

   int fd;  // This should be private, but to not introduce bugs I'm just leaving it here for now
//private:
};


class cHook : public cHookNative
{
  friend class cCaptainHook;

public:
  cHook(void);
  virtual ~cHook(void);

  int tfd(void);
  //virtual int IsHooked(void);
  virtual void Unhook(void);

  virtual void Write(ubit8 *pData, ubit32 nLen, int bCopy = TRUE);
  int ReadToQueue(void);

  cQueue qRX;

protected:
  void PushWrite(void);
  virtual void Input(int nFlags) = 0;

  cQueue qTX;

private:
  //int fd;
  int id;
};

class cCaptainHook
{
  friend class cHook;

public:
  cCaptainHook(void);
  ~cCaptainHook(void);

  void Close(void);
  void Hook(int nHandle, cHook *hook);
  int Wait(struct timeval *timeout);
  void Unhook(cHook *hook);

private:

  fd_set read_set, write_set;

  cHook *pfHook[256];

  int nIdx[256];
  int nMax;
  int nTop;
};

extern cCaptainHook CaptainHook;

#endif
