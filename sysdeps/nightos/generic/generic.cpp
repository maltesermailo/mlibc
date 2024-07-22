#include <errno.h>
#include <nightos/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <stdlib.h>

namespace mlibc {
    extern "C" long syscall_wrapper(long syscall_number, ...);

    void sys_libc_log(const char *message) {

    }

    [[noreturn]] void sys_libc_panic() {
        while (1) {

        }
    }

    [[noreturn]] void sys_exit(int status) {

    }

    [[noreturn, gnu::weak]] void sys_thread_exit() {

    }

    int sys_clock_get(int clock, time_t *secs, long *nanos) {

    }

    int sys_tcb_set(void *pointer) {
        return -ENOSYS;
    }

    int sys_futex_tid() {
        return 0;
    }

    int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
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
        int read = syscall_wrapper(SYS_READ, buf, count);

        *bytes_read = read;

        return 0;
    }


    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
        return -ENOSYS;
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
        return -ENOSYS;
    }

    int sys_close(int fd) {
        return -ENOSYS;
    }

    int sys_anon_allocate(size_t size, void **pointer) {
        return -ENOSYS;
    }

    int sys_anon_free(void *pointer, size_t size) {
        return -ENOSYS;
    }

    [[gnu::weak]] int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags,
                               struct stat *statbuf) {
        return -ENOSYS;
    }
    int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
        return -ENOSYS;
    }
    int sys_vm_unmap(void *pointer, size_t size) {
        return -ENOSYS;
    }
    int sys_vm_protect(void *pointer, size_t size, int prot) {
        return -ENOSYS;
    }

    int sys_vm_readahead(void *pointer, size_t size) {
        return -ENOSYS;
    }

}