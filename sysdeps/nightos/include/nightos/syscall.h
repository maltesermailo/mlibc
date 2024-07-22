//
// Created by Jannik on 21.06.2024.
//

#ifndef NIGHTOS_SYSCALL_H
#define NIGHTOS_SYSCALL_H
#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_STAT 4
#define SYS_FSTAT 5
#define SYS_LSTAT 6
#define SYS_POLL 7
#define SYS_LSEEK 8
#define SYS_MMAP 9
#define SYS_MPROTECT 10
#define SYS_MUNMAP 11
#define SYS_BRK 12
#define SYS_IOCTL 16
#define SYS_TEMP_TCB_SET 29


#include <stdint.h>

static inline int
sc_error(uintptr_t ret)
{
    if (ret > -4096UL)
        return -ret;
    return 0;
}

#endif //NIGHTOS_SYSCALL_H
