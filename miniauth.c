#include "miniauth.h"
#include <mdb.h>
#include <syslog.h>
#include <types/username.h>
#include <syslog.h>



typedef struct miniauth {
    time_t   itime;
    username user;
} miniauth;

bool miniauth_issue_token(mdb *_m, time_t _c, const username *_i_u, uuid_t _o_t) {
    miniauth a;
    uuid_generate_random(_o_t);
    a.itime = _c;
    strcpy(a.user.s, _i_u->s);
    return mdb_insert(_m, "miniauth", mdb_k_uuid(_o_t), &a, sizeof(a));
}

bool miniauth_check_token(mdb *_m, time_t _c, time_t _ttl, const uuid_t _i_t, username *_o_u, bool *_logged) {

    miniauth  a;
    bool      a_found;
    int       r;
    mdb_k     k = mdb_k_uuid(_i_t);

    /* Set unlogged by default. */
    if (_logged) *_logged = false;
    if (_o_u)    _o_u->s[0] = '\0';

    /* Search in database. */
    r = mdb_search_cp(_m, "miniauth", k, &a, sizeof(a), &a_found);
    if (!r/*err*/) return false;

    /* Not found. */
    if (!a_found) {
        if (_logged) {
            return true;
        } else {
            syslog(LOG_ERR, "Invalid token");
            return false;
        }
    }

    /* Expired. */
    if (_c > (a.itime + _ttl)) {
        if (_logged) {
            return true;
        } else {
            syslog(LOG_ERR, "Expired token");
            return false;
        }
    }
    
    /* Update. */
    if (_ttl > 0) {
        a.itime = _c + _ttl;
        r = mdb_replace(_m, "miniauth", k, &a, sizeof(a));
        if (!r/*err*/) return false;
    }
    
    /* Copy username. */
    if (_o_u) strcpy(_o_u->s, a.user.s);
    if (_logged) *_logged = true;
    
    return true;
}

