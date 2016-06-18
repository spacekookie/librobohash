////////////////////////////////
//
// This file is part of librobohash
//    and thus free software under the
//    GNU Public License 2.0
//
///////////

#ifndef _ROBOHASH_ERR_H_
#define _ROBOHASH_ERR_H

// Vague errors and return codes
#define RH_ERR_OK           0
#define RH_ERR_WARN         1
#define RH_ERR_GENERIC      2
#define RH_ERR_NOT_IMPL     7

// Specific errors and return codes
#define RH_ERR_INVALID_MD   16
#define RH_ERR_INVALID_BG   17
#define RH_ERR_CTX_VOID     18
#define RH_ERR_MALLOC       20
#define RH_ERR_RESOURCES    22
#define RH_ERR_MD_FAILED    24

#endif // _ROBOHASH_ERR_H_
