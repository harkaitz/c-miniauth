#include "miniauth.h"
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>
#include <str/str2num.h>
#include <types/username.h>
#include <types/uuid_ss.h>
#include <mdb.h>

#define COPYRIGHT_LINE \
    "Bug reports, feature requests to gemini|https://harkadev.com/oss" "\n" \
    "Copyright (c) 2022 Harkaitz Agirre, harkaitz.aguirre@gmail.com" "\n" \
    ""

const char help[] =
    "Usage: %s ..."                                            "\n"
    ""                                                         "\n"
    "... APP i|issue USERNAME       : Issue login token."      "\n"
    "... APP c|check TOKEN    [TTL] : Get username for token." "\n"
    ""                                                         "\n"
    COPYRIGHT_LINE;

int main (int _argc, char *_argv[]) {

    mdb     *mdb    = NULL;
    int      res    = 0;
    int      retval = 1;
    time_t   ctime  = time(NULL);
    long     ttl_l  = 0;
    username user   = {0};
    uuid_t   token  = {0};
    bool     logged = false;
    char    *app, *cmd, *arg1, *arg2;
    
    /* Print help. */
    _argv[0] = basename(_argv[0]);
    if (_argc == 1 || !strcasecmp(_argv[1], "-h") || !strcasecmp(_argv[1], "--help")) {
        printf(help, _argv[0]);
        return 0;
    }

    /* Set logging. */
    openlog(_argv[0], LOG_PERROR, LOG_AUTH);

    /* Get arguments. */
    app  = _argv[1];
    cmd  = (_argc>2)?_argv[2]:NULL;
    arg1 = (_argc>3)?_argv[3]:NULL;
    arg2 = (_argc>4)?_argv[4]:NULL;
    if (!app/*err*/) goto cleanup_missing_app_name;
    if (!cmd/*err*/) goto cleanup_missing_command;
    
    /* Open database. */
    res = mdb_create(&mdb, NULL) && miniauth_open(mdb, app);
    if (!res/*err*/) goto cleanup;
    
    /* Perform operation. */
    switch(str2num(cmd, strcasecmp,
                   "i", 1, "issue", 1,
                   "c", 2, "check", 2,
                   NULL)) {
    case 1:
        if (!arg1/*err*/) goto cleanup_missing_username;
        res = username_parse(&user, arg1);
        if (!res/*err*/) goto cleanup_invalid_username;
        res = miniauth_issue_token(mdb, ctime, &user, token);
        if (!res/*err*/) goto cleanup;
        printf("%s\n", uuid_str(token, UUID_SS_STORE));
        break;
    case 2:
        if (!arg1/*err*/) goto cleanup_missing_token;
        res = uuid_parse_nn(arg1, token);
        if (!res/*err*/) goto cleanup_invalid_uuid;
        ttl_l = (arg2)?strtol(arg2, NULL, 10):3600;
        res = miniauth_check_token(mdb, ctime, ttl_l, token, &user, &logged);
        if (!res/*err*/) goto cleanup;
        if (logged) {
            printf("%s\n", user.s);
        } else {
            syslog(LOG_ERR, "Not logged in");
        }
        break;
    default:
        goto cleanup_invalid_command;
    }

    /* Cleanup */
    retval = 0;
    goto cleanup;
 cleanup_missing_app_name:
    syslog(LOG_ERR, "Missing application name.");
    goto cleanup;
 cleanup_missing_command:
    syslog(LOG_ERR, "Missing command.");
    goto cleanup;
 cleanup_missing_username:
    syslog(LOG_ERR, "Missing username.");
    goto cleanup;
 cleanup_invalid_username:
    syslog(LOG_ERR, "Invalid username.");
    goto cleanup;
 cleanup_missing_token:
    syslog(LOG_ERR, "Missing token.");
    goto cleanup;
 cleanup_invalid_uuid:
    syslog(LOG_ERR, "Invalid Token: Must be a valid UUID.");
    goto cleanup;
 cleanup_invalid_command:
    syslog(LOG_ERR, "Invalid command.");
    goto cleanup;
 cleanup:
    if (mdb) mdb_destroy(mdb);
    return retval;
}
