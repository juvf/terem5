#ifndef MAINTASK___H
#define MAINTASK___H
#ifdef __cplusplus
extern "C"
{
#endif
  
void mainTask(void *context);
void stopJ();
void initExti();
void deinitExti();
void initUartDeinitExti3();

#ifdef __cplusplus
}
#endif
#endif
