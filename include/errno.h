
#ifndef _ERRNO_H_
#define _ERRNO_H_

extern int errno;

/* Error Codes: */
#define EOK         0
#define EPERM       1
#define ENOENT      2
#define EIO         5
#define EBADF       9     
#define EOPNOTSUPP  95 

#ifndef __ERRNO_T_DEFINED
#define __ERRNO_T_DEFINED
typedef int errno_t;
#endif

#endif  /* _ERRNO_H_ */

