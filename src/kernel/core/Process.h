#ifndef CORE_PROCESS_H
#define CORE_PROCESS_H

#include <lang/lang.h>
#include <lang/Pool.h>
#include <fs/File.h>
#include <core/Scheduler.h>
#include <memory/AddressSpace.h>
#include <interrupts/Interrupts.h>


class Thread;

class Process {
public:
    Process();
    ~Process();

    Thread* spawnThread(threadEntryPoint entry, void* arg, const char* name);
    int attachFile(File* f);
    void closeFile(int fd);

    void* sbrk(uint64_t size);

    uint64_t brk;
    uint64_t pid;
    char* name;

    AddressSpace* addressSpace;
    bool isKernel;
    
    Pool<Thread*, 1024> threads;
    Pool<File*, 1024> files;

private:
};
#endif