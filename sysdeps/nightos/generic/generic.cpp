#include <errno.h>
#include <nightos/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>

#include <bits/ensure.h>
#include <asm/ioctls.h>

namespace mlibc {
    extern "C" long syscall_wrapper(long syscall_number, ...);

    [[gnu::weak]] int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
        syscall_wrapper(SYS_IOCTL, fd, request, arg);

        return 0;
    }

    int sys_ppoll(struct pollfd *fds, int nfds, const struct timespec *timeout,
                  const sigset_t *sigmask, int *num_events) {
        int ret = syscall_wrapper(SYS_PPOLL, fds, nfds, timeout, sigmask);

        if (int e = sc_error(ret); e)
            return e;

        *num_events = ret;

        return 0;
    }

#ifndef MLIBC_BUILDING_RTLD
    int
    sys_pselect(int nfds, fd_set *read_set, fd_set *write_set, fd_set *except_set,
                const struct timespec *timeout, const sigset_t *sigmask, int *num_events)
    {
        struct pollfd *fds = (struct pollfd *)malloc(
                nfds * sizeof(struct pollfd));

        for (int i = 0; i < nfds; i++) {
            struct pollfd *fd = &fds[i];
            memset(fd, 0, sizeof(struct pollfd));

            if (read_set && FD_ISSET(i, read_set))
                fd->events |= POLLIN;
            if (write_set && FD_ISSET(i, write_set))
                fd->events |= POLLOUT;
            if (except_set && FD_ISSET(i, except_set))
                fd->events |= POLLPRI;

            if (!fd->events) {
                fd->fd = -1;
                continue;
            }

            fd->fd = i;
        }

        int e = sys_ppoll(fds, nfds, timeout, sigmask, num_events);

        if (e != 0) {
            free(fds);
            return e;
        }

        fd_set res_read_set;
        fd_set res_write_set;
        fd_set res_except_set;
        FD_ZERO(&res_read_set);
        FD_ZERO(&res_write_set);
        FD_ZERO(&res_except_set);

        for (int i = 0; i < nfds; i++) {
            struct pollfd *fd = &fds[i];

            if (read_set && FD_ISSET(i, read_set) &&
                fd->revents & (POLLIN | POLLERR | POLLHUP)) {
                FD_SET(i, &res_read_set);
            }

            if (write_set && FD_ISSET(i, write_set) &&
                fd->revents & (POLLOUT | POLLERR | POLLHUP)) {
                FD_SET(i, &res_write_set);
            }

            if (except_set && FD_ISSET(i, except_set) &&
                fd->revents & POLLPRI) {
                FD_SET(i, &res_except_set);
            }
        }

        free(fds);

        if (read_set)
            memcpy(read_set, &res_read_set, sizeof(fd_set));
        if (write_set)
            memcpy(write_set, &res_write_set, sizeof(fd_set));
        if (except_set)
            memcpy(except_set, &res_except_set, sizeof(fd_set));

        return 0;
    }
#endif

    int sys_fcntl(int fd, int request, va_list args, int* result){
        if(request == F_DUPFD){
            return sys_dup(fd, 0, result);
        } else if (request == F_DUPFD_CLOEXEC) {
            return sys_dup(fd, O_CLOEXEC, result);
        } else if(request == F_GETFD){
            *result = 0;
            return 0;
        } else if(request == F_SETFD){
            /*if(va_arg(args, int) & FD_CLOEXEC) {
                return sys_ioctl(fd, FIOCLEX, NULL, result);
            } else {
                return sys_ioctl(fd, FIONCLEX, NULL, result);
            }*/
            return ENOSYS;
        } else if(request == F_GETFL){
            return ENOSYS;

            /*int ret = syscall(SYS_GET_FILE_STATUS_FLAGS, fd);
            if(ret < 0){
                return -ret;
            }

            *result = ret;
            return 0;*/
        } else if(request == F_SETFL){
            /*int ret = syscall(SYS_SET_FILE_STATUS_FLAGS, fd, va_arg(args, int));
            return -ret;*/

            return ENOSYS;
        } else {
            infoLogger() << "mlibc: sys_fcntl unsupported request (" << request << ")" << frg::endlog;
            return EINVAL;
        }
    }

    int sys_tcgetattr(int fd, struct termios *attr) {
        int ret = syscall_wrapper(SYS_IOCTL, fd, TCGETS, attr);
        if (int e = sc_error(ret); e)
            return e;
        return 0;
    }

    int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
        int req;

        switch (optional_action) {
            case TCSANOW: req = TCSETS; break;
            case TCSADRAIN: req = TCSETSW; break;
            case TCSAFLUSH: req = TCSETSF; break;
            default: return EINVAL;
        }

        int ret = syscall_wrapper(SYS_IOCTL, fd, req, attr);
        if (int e = sc_error(ret); e)
            return e;
        return 0;
    }


    int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
        int ret = syscall_wrapper(SYS_POLL, fds, count, timeout);

        if (int e = sc_error(ret); e)
            return e;

        *num_events = ret;

        return 0;
    }

    int sys_dup(int fd, int flags, int *newfd) {
        __ensure(!flags);
        int ret = syscall_wrapper(SYS_DUP, fd);
        if (int e = sc_error(ret); e)
            return e;
        *newfd = ret;
        return 0;
    }

    uid_t sys_getuid(){
        return syscall_wrapper(SYS_GETUID);
    }

    gid_t sys_getgid(){
        return syscall_wrapper(SYS_GETGID);
    }

    uid_t sys_geteuid(){
        return syscall_wrapper(SYS_GETUID);
    }

    gid_t sys_getegid(){
        return syscall_wrapper(SYS_GETGID);
    }

    pid_t sys_getppid(void) {
        return syscall_wrapper(SYS_GETPPID);
    }

    pid_t sys_getpgid(pid_t pid, pid_t* out) {
        auto ret = syscall_wrapper(SYS_GETPGRP, pid);
        if(int e = sc_error(ret); e)
            return e;

        *out = ret;

        return 0;
    }

#ifndef MLIBC_BUILDING_RTLD
    int sys_ttyname(int fd, char *buf, size_t size) {
        if(!mlibc::sys_isatty(fd)) {
            return ENOTTY;
        }

        //TODO: Once PTYs are added, this should be changed.
        strcpy(buf, "/dev/console0");
        size = strlen("/dev/console0");

        return 0;
    }
#endif

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

    int sys_getcwd(char *buffer, size_t size){
        return syscall_wrapper(SYS_GETCWD, buffer, size);
    }

    int sys_isatty(int fd) {
        //Standard output is connected directly to the console
        //TODO: Change that when switching to graphic output
        if(fd <= 3) {
            return 0;
        }

        return ENOTTY;
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
        auto ret = syscall_wrapper(SYS_WAIT4, pid, status, flags, ru);
        if(int e = sc_error(ret); e)
            return e;

        *ret_pid = ret;

        return 0;
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
        syscall_wrapper(SYS_WRITE, 0, "MLIBC: ", strlen("MLIBC: "));
        syscall_wrapper(SYS_WRITE, 0, message, strlen(message));
        syscall_wrapper(SYS_WRITE, 0, "\n", 1);
    }

    [[noreturn]] void sys_libc_panic() {
        sys_libc_log("PANIC EXIT!\n");
        while (1) {
            sys_exit(-1);
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