#pragma once

#if defined(_ACKNEXT_INTERNAL_)
#define ACKCONST
#else
#define ACKCONST const
#endif

#if defined(__cplusplus)
#define ACKFUN extern "C"
#else
#define ACKFUN
#endif
