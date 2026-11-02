#pragma once

// Minimal stand-in for libuuid's <uuid/uuid.h>, covering what libgamestream uses.
// libuuid has a TLS segment, and loading it breaks libhybris' bionic TLS on
// some devices, crashing the GPU driver on startup.

typedef unsigned char uuid_t[16];

void uuid_generate_random(uuid_t out);
void uuid_unparse(const uuid_t uu, char *out);
