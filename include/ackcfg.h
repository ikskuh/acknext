#pragma once

#ifndef _ACKNEXT_ACKCFG_
#define _ACKNEXT_ACKCFG_

#ifdef _ACKNEXT_INTERNAL_
#define ACKCONST
#else
#define ACKCONST const
#endif

#ifdef __cplusplus
#define ACKFUN extern "C"
#else
#define ACKFUN extern
#endif

#endif // _ACKNEXT_ACKCFG_
