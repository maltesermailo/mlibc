#include <errno.h>
#include <nightos/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <stdlib.h>
#include <stdint.h>

namespace mlibc {
    extern "C" long syscall_wrapper(long syscall_number, ...);

    //==========================================================================//
    //                        ANSI C SYSDEPS                                    //
    //==========================================================================//

    [[noreturn]] void sys_exit(int status) {

    }

    [[noreturn, gnu::weak]] void sys_thread_exit() {

    }

    int sys_clock_get(int clock, time_t *secs, long *nanos) {

    }

    //==========================================================================//
    //                        INTERNAL SYSDEPS except write                     //
    //==========================================================================//

    void sys_libc_log(const char *message) {
        syscall_wrapper(SYS_WRITE, 0, message, strlen(message));
        syscall_wrapper(SYS_WRITE, 0, "\n", strlen(message));
    }

    [[noreturn]] void sys_libc_panic() {
        sys_libc_log("PANIC!\n");
        while (1) {

        }
    }

    int sys_tcb_set(void *pointer) {
        syscall_wrapper(SYS_TEMP_TCB_SET, (uintptr_t) pointer);

        return 0;
    }

    int sys_futex_tid() {
        return 0;
    }

    int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
        if(*pointer == expected) {

        }

        return -ENOSYS;
    }

    int sys_futex_wake(int *pointer) {
        return -ENOSYS;
    }


    int sys_open(const char *pathname, int flags, mode_t mode, int *fd) {
        int out = syscall_wrapper(SYS_OPEN, pathname, mode);

        *fd = out;

        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
        int read = syscall_wrapper(SYS_READ, fd, buf, count);

        *bytes_read = read;

        return 0;
    }


    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
        int written = syscall_wrapper(SYS_WRITE, fd, buf, count);

        *bytes_written = written;

        return 0;
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
        return -ENOSYS;
    }

    int sys_close(int fd) {
        return 0;
    }

    int sys_anon_allocate(size_t size, void **pointer) {
        return sys_vm_map(0, size, 0, 0, 0, 0, pointer);
    }

    int sys_anon_free(void *pointer, size_t size) {
        return sys_vm_unmap(pointer, size);
    }

    [[gnu::weak]] int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags,
                               struct stat *statbuf) {
        return -ENOSYS;
    }
    int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
        long result = syscall_wrapper(SYS_MMAP, (uintptr_t) hint, size, prot, flags, fd, offset);

        if(result == 0) {
            return ENOMEM;
        }

        *window = (void*)result;

        return 0;
    }
    int sys_vm_unmap(void *pointer, size_t size) {
        long result = syscall_wrapper(SYS_MUNMAP, (uintptr_t) pointer, size);

        if(result != 0) {
            return sc_error(result);
        }

        return 0;
    }
    int sys_vm_protect(void *pointer, size_t size, int prot) {
        return 0; //NOT IMPLEMENTED
    }

    int sys_vm_readahead(void *pointer, size_t size) {
        return -ENOSYS;
    }

}