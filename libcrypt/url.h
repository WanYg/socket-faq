#ifndef __URL_H_
#define __URL_H_

#include <stdint.h>
#include <sys/types.h>

/*url*/
int url_decode(char *str, int len);
char *url_encode(char const *s, int len, int *new_length);

#endif //__URL_H_
