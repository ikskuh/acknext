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

#ifndef ACKNEXT_BITMAP_DETAIL
#define ACKNEXT_BITMAP_DETAIL void
#endif

#ifndef ACKNEXT_LEVEL_DETAIL
#define ACKNEXT_LEVEL_DETAIL void
#endif

#ifndef ACKNEXT_ENTITY_DETAIL
#define ACKNEXT_ENTITY_DETAIL void
#endif

#endif // _ACKNEXT_ACKCFG_
