# NAME

miniauth_open(), miniauth_close(), miniauth_issue_token(),
miniauth_check_token()

# SYNOPSIS

    #include <miniauth.h>
    
    bool miniauth_open  (mdb *_m, const char _app[]);
    void miniauth_close (mdb *_m);
    
    bool miniauth_issue_token (mdb            *_m,
                               time_t          _c,
                               const username *_i_u,
                               uuid_t          _o_t);
    bool miniauth_check_token (mdb         *_m,
                               time_t       _c,
                               time_t       _ttl,
                               const uuid_t  _i_t,
                               username     *_o_u,
                               bool         *_logged);

# DESCRIPTION

Issue authentication tokens and check.

# RETURN VALUE

True on success false on error.

# COLLABORATING

For making bug reports, feature requests and donations visit
one of the following links:

1. [gemini://harkadev.com/oss/](gemini://harkadev.com/oss/)
2. [https://harkadev.com/oss/](https://harkadev.com/oss/)

# SEE ALSO

**SYS_AUTHORIZATION(3)**
