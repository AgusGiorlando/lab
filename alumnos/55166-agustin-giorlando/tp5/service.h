#ifndef _MY_SERVICE_H_
#define _MY_SERVICE_H_

void *service(void *arg);
void set_status(char const * new_status,int sdc);
void set_header(char const * type,char const * extension,int sdc);
#endif
