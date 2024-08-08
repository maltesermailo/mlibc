#include <errno.h>
#include <nightos/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <stdlib.h>
#include <stdint.h>

namespace mlibc {
    extern "C" long syscall_wrapper(long syscall_number, ...);

    [[gnu::weak]] int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
        syscall_wrapper(SYS_IOCTL, fd, request);

        return 0;
    }

    //==========================================================================//
    //                        ANSI C SYSDEPS                                    //
    //==========================================================================//

    [[noreturn]] void sys_exit(int status) {
        syscall_wrapper(SYS_EXIT_GROUP);
    }

    [[noreturn, gnu::weak]] void sys_thread_exit() {
        syscall_wrapper(SYS_EXIT);
    }

    int sys_clock_get(int clock, time_t *secs, long *nanos) {
        return 0;
    }

    int sys_clock_getres(int clock, time_t *secs, long *nanos) {
        return 0;
    }

    int sys_flock(int fd, int options) {
        return ENOSYS;
    }

    int sys_open_dir(const char *path, int *handle) {
        int out = syscall_wrapper(SYS_OPEN, path, 0);

        *handle = out;

        return 0;
    }

    int sys_read_entries(int handle, void *buffer, size_t max_size,
                         size_t *bytes_read) {
        return 0;
    }

    int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
        long read = syscall_wrapper(SYS_PREAD64, fd, buf, n, off);
        *bytes_read = read;

        return 0;
    }

    int sys_sleep(time_t *secs, long *nanos) {
        struct timespec spec = {
                .tv_sec = *secs,
                .tv_nsec = *nanos
        };
        syscall_wrapper(SYS_NANOSLEEP, &spec);

        return 0;
    }

    int sys_isatty(int fd) {
        return 0;
    }

    [[gnu::weak]] int sys_rmdir(const char *path) {
        return ENOSYS;
    }
    [[gnu::weak]] int sys_unlinkat(int dirfd, const char *path, int flags) {
        return ENOSYS;
    }

    [[gnu::weak]] int sys_rename(const char *path, const char *new_path) {
        return ENOSYS;
    }
    [[gnu::weak]] int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
        return ENOSYS;
    }

    [[gnu::weak]] int sys_sigprocmask(int how, const sigset_t *__restrict set,
                                      sigset_t *__restrict retrieve) {
        return ENOSYS;
    }
    [[gnu::weak]] int sys_sigaction(int, const struct sigaction *__restrict,
                                    struct sigaction *__restrict) {
        return ENOSYS;
    }

    [[gnu::weak]] int sys_fork(pid_t *child) {
        pid_t pid = syscall_wrapper(SYS_FORK);

        *child = pid;
        return 0;
    }

    int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
        return ENOSYS;
    }

    [[gnu::weak]] pid_t sys_getpid() {
        return syscall_wrapper(SYS_GETPID);
    }

    [[gnu::weak]] int sys_kill(int, int) {
        return ENOSYS;
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

    /*int sys_futex_tid() {
        return 0;
    }*/

    int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
        if(__sync_fetch_and_add(pointer, 0) == expected) {
            return syscall_wrapper(SYS_FUTEX, pointer, expected, time);
        }

        return -EAGAIN;
    }

    int sys_futex_wake(int *pointer) {
        return syscall_wrapper(SYS_FUTEX, pointer, 0, 0);
    }


    int sys_open(const char *pathname, int flags, mode_t mode, int *fd) {
        int out = syscall_wrapper(SYS_OPEN, pathname, mode);

        *fd = out;

        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
        int read = syscall_wrapper(SYS_READ, fd, buf, count);

        *bytes_read = 0;
        if(read >= 0) {
            *bytes_read = read;
        }

        return read < 0 ? sc_error(read) : 0;
    }


    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
        int written = syscall_wrapper(SYS_WRITE, fd, buf, count);

        *bytes_written = 0;
        if(written >= 0) {
            *bytes_written = written;
        }

        return written < 0 ? sc_error(written) : 0;
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
        long result = syscall_wrapper(SYS_LSEEK, fd, offset, whence);

        if(result < 0) {
            return sc_error(result);
        }

        *new_offset = result;

        return 0;
    }

    int sys_close(int fd) {
        return syscall_wrapper(SYS_CLOSE, fd);
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