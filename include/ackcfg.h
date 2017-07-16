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

#define RAD_TO_DEG 57.29580406904963
#define DEG_TO_RAD 0.0174533

#endif // _ACKNEXT_ACKCFG_
