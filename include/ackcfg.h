#pragma once

#if defined(_ACKNEXT_INTERNAL_)
#define ACKCONST const
#else
#define ACKCONST
#endif

#if defined(__cplusplus)
#define ACKFUN extern "C"
#else
#define ACKFUN
#endif
