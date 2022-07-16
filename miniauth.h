#ifndef MINIAUTH_H
#define MINIAUTH_H

#include <uuid/uuid.h>
#include <stdbool.h>

typedef struct mdb      mdb;
typedef struct username username;

bool miniauth_issue_token (mdb *_m, time_t _c,              const username *_i_u, uuid_t    _o_t);
bool miniauth_check_token (mdb *_m, time_t _c, time_t _ttl, const uuid_t    _i_t, username *_o_u, bool *_logged);

#endif
