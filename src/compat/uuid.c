#include "uuid/uuid.h"

#include <openssl/rand.h>
#include <stdio.h>

void uuid_generate_random(uuid_t out)
{
    RAND_bytes(out, sizeof(uuid_t));
    out[6] = (out[6] & 0x0f) | 0x40; // version 4
    out[8] = (out[8] & 0x3f) | 0x80; // RFC 4122 variant
}

void uuid_unparse(const uuid_t uu, char *out)
{
    sprintf(out, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uu[0], uu[1], uu[2], uu[3], uu[4], uu[5], uu[6], uu[7],
            uu[8], uu[9], uu[10], uu[11], uu[12], uu[13], uu[14], uu[15]);
}
