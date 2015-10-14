#ifndef MAINTASK___H
#define MAINTASK___H
#ifdef __cplusplus
extern "C"
{
#endif
  
void mainTask(void *context);
void sleepJ();
void initExti();
void deinitExti();

#ifdef __cplusplus
}
#endif
#endif
