
#include <stdlib.h>
#include <sys/auxv.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>
#include <mlibc/posix-pipe.hpp>

#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

#include <xuniverse.frigg_pb.hpp>

// defined by the POSIX library
void __mlibc_initLocale();
// defined by the POSIX library
void __mlibc_initStdio();
// defined in file.cpp
void __mlibc_initFs();

// declared in posix-pipe.hpp
helx::EventHub eventHub = helx::EventHub::create();
helx::Pipe posixPipe;
helx::Pipe fsPipe;

// declared in posix-pipe.hpp
int64_t allocPosixRequest() {
	static int64_t next = 1;
	return next++;
}

// TODO: this function needs to be removed after we fix fork() semantics.
void __mlibc_reinitPosixPipe() {
	unsigned long fs_server;
	if(peekauxval(AT_FS_SERVER, &fs_server))
		__ensure(!"No AT_FS_SERVER specified");

	fsPipe = helx::Pipe(fs_server);
}

struct LibraryGuard {
	LibraryGuard();
};

static LibraryGuard guard;

LibraryGuard::LibraryGuard() {
	__mlibc_initLocale();
	__mlibc_initFs();
	__mlibc_initStdio();

	__mlibc_reinitPosixPipe();
}

// __dso_handle is usually defined in crtbeginS.o
// Since we link with -nostdlib we have to manually define it here
__attribute__ (( visibility("hidden") )) void *__dso_handle;

extern "C" int main(int argc, char *argv[], char *env[]);

// not declared in any header
extern char **environ;

extern "C" void __mlibc_entry() {
	char *empty_argv[] = { nullptr };

	int result = main(1, empty_argv, environ);
	exit(result);
}

