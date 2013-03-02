#ifndef CORE_SCHEDULER_H
#define CORE_SCHEDULER_H

#include <lang/lang.h>
#include <lang/Pool.h>
#include <lang/Singleton.h>
#include <core/Thread.h>
#include <interrupts/Interrupts.h>


typedef void (*threadEntryPoint)(void*);


class Scheduler : public Singleton<Scheduler> {
public:
    void init();
    Process* spawnProcess();
    void spawnKernelThread(threadEntryPoint entry, void* argument, const char* name);
    void registerThread(Thread* t);
    void scheduleNextThread();
    void scheduleNextThread(Thread* t);
    void contextSwitch(isrq_registers_t*);
    
    void forceThreadSwitchUserspace(Thread* preferred);
    void forceThreadSwitchISRQContext(Thread* preferred, isrq_registers_t* regs);
    
    void saveKernelState(isrq_registers_t*);

    Thread* getActiveThread();
    
    Thread* kernelThread;
private:
    Thread* nextThread;
    Pool<Thread*, 1024> threads;
    Process* kernelProcess;
    Thread* activeThread;
};

#endif