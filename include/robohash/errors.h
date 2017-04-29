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
#define RH_ERR_OK                   0           // Everything is A-OK
#define RH_ERR_SUCCESS              0           // This is a triumph, I'm making a note here, huge success <3
#define RH_ERR_WARNING              1           // Can be ignored but might bite you in the ass later
#define RH_ERR_GENERIC              (1 << 2)    // Something generic went boom
#define RH_ERR_NOT_IMPL             (1 << 3)    // I haven't been bothered enough to implement it yet - woops

// Specific errors and return codes
#define RH_ERR_INVALID_MD           (1 << 4)    // This is not the message digest you're looking for
#define RH_ERR_INVALID_BG           (1 << 5)    //
#define RH_ERR_CTX_VOID             (1 << 6)    // Yo, the context you gave me was complete shit
#define RH_ERR_MALLOC               (1 << 7)    // Memory allocation failed miserably - http://downloadmoreram.com/
#define RH_ERR_RESOURCES            (1 << 8)    // Resources for robots couldn't be found. You need to set the path
#define RH_ERR_MD_FAILED            (1 << 9)    // Calculating the message digest (not MD5!) failed
#define RH_ERR_FILE_OPEN_FAILED     (1 << 10)   // Opening a file failed
#define RH_ERR_FILE_STORE_FAILED    (1 << 11)   // Writing to a file (most likely .png) failed

// Errors about images and image manipulation
#define RH_ERR_NULL_IMG             (1 << 20)
#define RH_ERR_RESOLUTION_MISMATCH  (1 << 21)

#endif // _ROBOHASH_ERR_H_
