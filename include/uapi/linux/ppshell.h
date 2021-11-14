#ifndef _UAPI_LINUX_PPSHELL_H
#define _UAPI_LINUX_PPSHELL_H

#include <linux/types.h>

#define PPS_SERVICE_NAME_MAX_LEN 100
#define PPS_SERVICE_DESCR_MAX_LEN 100
#define PPS_SERVICE_COMMAND_MAX_LEN 1000
#define PPS_SERVICE_PWD_MAX_LEN 100

#define PPSHELL_CREATE_PARAMS_SIZE_VER0 36

struct ppshell_create_params {
    __u32 size; // for fwd/bkw compatability
    char *name; // name for the service, unique per user
    char *description; // verbose description of the service, shown to other users
    char *command; // command to be executed
    char *auth_pwd;// password based auth (NULL = disabled)
};


#endif /* _UAPI_LINUX_PPSHELL_H */
