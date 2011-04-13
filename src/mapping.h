#ifndef __mapping_h__
#define __mapping_h__

#include <sys/stat.h>
#include <errno.h>
#include "proto/rhizofs.pb-c.h"

// filetypes
#define RHI_FILETYPE_DIR   0040000 // Directory
#define RHI_FILETYPE_CHR   0020000 // Character device
#define RHI_FILETYPE_BLK   0060000 // Block device
#define RHI_FILETYPE_REG   0100000 // Regular file
#define RHI_FILETYPE_IFO   0010000 // FIFO
#define RHI_FILETYPE_LNK   0120000 // Symbolic link
#define RHI_FILETYPE_SOCK  0140000 // Socket

// permissions
#define RHI_PERM_RUSR 0400     // Read by owner
#define RHI_PERM_WUSR 0200     // Write by owner
#define RHI_PERM_XUSR 0100     // Execute by owner
#define RHI_PERM_RGRP (RHI_PERM_RUSR >> 3)  // Read by group
#define RHI_PERM_WGRP (RHI_PERM_WUSR >> 3)  // Write by group
#define RHI_PERM_XGRP (RHI_PERM_XUSR >> 3)  // Execute by group
#define RHI_PERM_ROTH (RHI_PERM_RGRP >> 3)  // Read by others
#define RHI_PERM_WOTH (RHI_PERM_WGRP >> 3)  // Write by others
#define RHI_PERM_XOTH (RHI_PERM_XGRP >> 3)  // Execute by others


typedef struct mode_pair {
    unsigned int protocol;
    mode_t local;
} mode_pair;

typedef struct errno_pair {
    int protocol;
    int local;
} errno_pair;


// l2p = local to protocol
// p2l = protocol to local

unsigned int mapping_mode_l2p(mode_t mode);
mode_t mapping_mode_p2l(unsigned int);

int mapping_errno_l2p(int lerrno);
int mapping_errno_p2l(int perrno);

#endif // __mapping_h__

