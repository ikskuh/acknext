#ifndef ACKCPP_H
#define ACKCPP_H
#ifdef __cplusplus

#include "acktypes.h"

static inline bool operator == (HANDLE const & rhs, HANDLE const & lhs)
{
    return (rhs.id == lhs.id) && (rhs.type == lhs.type);
}

static inline bool operator != (HANDLE const & rhs, HANDLE const & lhs)
{
    return (rhs.id != lhs.id) || (rhs.type != lhs.type);
}

#endif // __cplusplus
#endif // ACKCPP_H
