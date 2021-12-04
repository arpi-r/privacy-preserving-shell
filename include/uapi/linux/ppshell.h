#ifndef _UAPI_LINUX_PPSHELL_H
#define _UAPI_LINUX_PPSHELL_H

#include <linux/types.h>

#define PPS_SERVICE_NAME_MAX_LEN 100
#define PPS_SERVICE_DESCR_MAX_LEN 1000
#define PPS_SERVICE_COMMAND_MAX_LEN 800
#define PPS_SERVICE_PWD_MAX_LEN 100
#define PPS_SERVICE_AUTH_UID_LIST_MAX_LEN 100
#define PPS_SERVICE_ENV_MAX_LEN 100
#define PPS_SERVICE_ENV_VAR_MAX_LEN 1000

#define PPSHELL_CREATE_PARAMS_SIZE_VER0 72 // 4 bytes paddings for __u32
#define PPSHELL_CALL_PARAMS_SIZE_VER0 32 // 4 bytes paddings for __u32

struct ppshell_create_params {
    __u32 size; // for fwd/bkw compatability
    char *name; // name for the service, unique per user
    char *description; // verbose description of the service, shown to other users
    char *command; // command to be executed // will be passed as -c arg to /bin/bash
    char *auth_pwd;// password based auth (NULL = disabled)
    uid_t *auth_uid_list; // list of uids (NULL = disabled)
    __u32 auth_uid_len; // alleged length of the auth uid list
    char **environ; // list of environment vars to be passed to service
    __u32 env_len;
};

struct ppshell_call_params {
    __u32 size; // for fwd/bkw compatability
    char *name; // name for the service to be called
    uid_t owner_euid; // effective user id of the owner
    char *auth_pwd;// password based auth (NULL = not checked)
};

struct ppshell_list_values {
	char *name;
	char *desc;
};

#endif /* _UAPI_LINUX_PPSHELL_H */
