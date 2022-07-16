#include "miniauth.h"
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>
#include <mdb.h>
#include <types/username.h>
#include <types/uuid_ss.h>
#include <syslog.h>

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
    int      e      = 0;
    int      r      = 1;
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
    e = mdb_create(&mdb, NULL);
    if (!e/*err*/) goto cleanup;

    /* Map database. */
    e = mdb_map(mdb, "miniauth", "%s::miniauth", app);
    if (!e/*err*/) goto cleanup;
    
    /* Perform operation. */
    if (!strcmp(cmd, "i") || !strcmp(cmd, "issue")) {
        if (!arg1/*err*/) goto cleanup_missing_username;
        e = username_parse(&user, arg1, NULL);
        if (!e/*err*/) goto cleanup_invalid_username;
        e = miniauth_issue_token(mdb, ctime, &user, token);
        if (!e/*err*/) goto cleanup;
        printf("%s\n", uuid_str(token, UUID_SS_STORE));
    } else if (!strcmp(cmd, "c") || !strcmp(cmd, "check")) {
        if (!arg1/*err*/) goto cleanup_missing_token;
        e = uuid_parse_secure(token, arg1, false, NULL);
        if (!e/*err*/) goto cleanup;
        ttl_l = (arg2)?strtol(arg2, NULL, 10):3600;
        e = miniauth_check_token(mdb, ctime, ttl_l, token, &user, &logged);
        if (!e/*err*/) goto cleanup;
        if (logged) {
            printf("%s\n", user.s);
        } else {
            syslog(LOG_ERR, "Not logged in");
        }
    } else {
        goto cleanup_invalid_command;
    }
    
    /* Cleanup */
    r = 0;
    goto cleanup;
 cleanup_missing_app_name: syslog(LOG_ERR, "Missing application name."); goto cleanup;
 cleanup_missing_command:  syslog(LOG_ERR, "Missing command.");          goto cleanup;
 cleanup_missing_username: syslog(LOG_ERR, "Missing username.");         goto cleanup;
 cleanup_invalid_username: syslog(LOG_ERR, "Invalid username.");         goto cleanup;
 cleanup_missing_token:    syslog(LOG_ERR, "Missing token.");            goto cleanup;
 cleanup_invalid_command:  syslog(LOG_ERR, "Invalid command.");          goto cleanup;
 cleanup:
    if (mdb) mdb_destroy(mdb);
    return r;
}
