#ifndef _MY_SERVICE_H_
#define _MY_SERVICE_H_

struct conn_param{
	int logfd;
	int conn;
	pthread_mutex_t mutex;
};

void* service (void* parameters);
int set_status(char const * new_status,int sdc);
int set_header(char const * type,char const * extension,int sdc);
#endif
