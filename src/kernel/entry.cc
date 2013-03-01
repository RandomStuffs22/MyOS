#include "kutil.h"
#include "alloc/malloc.h"

#include <core/CPU.h>
#include <core/Debug.h>
#include <core/MQ.h>
#include <core/Process.h>
#include <core/Scheduler.h>
#include <core/Wait.h>
#include <hardware/keyboard/Keyboard.h>
#include <hardware/cmos/CMOS.h>
#include <hardware/pit/PIT.h>
#include <interrupts/IDT.h>
#include <interrupts/Interrupts.h>
#include <memory/AddressSpace.h>
#include <memory/Memory.h>
#include <tty/PhysicalTerminalManager.h>

#include <fs/devfs/PTY.h>
#include <fs/fat32/FAT32FS.h>
#include <fs/File.h>
#include <fs/Directory.h>
#include <fcntl.h>


int main() {}


void pit_handler(isrq_registers_t* regs) {
    static int counter = 0;
    counter++;
    microtrace();
}

void isrq80(isrq_registers_t* regs)  {
    klog('w', "SYSCALL #%x", regs->rax);
    klog_flush();
}


void repainterThread(void*) {
    for (;;) {
        Scheduler::get()->getActiveThread()->wait(new WaitForDelay(50));
        PhysicalTerminalManager::get()->render();
    }
}

void testThread(void*) {
    PTYSlave* p = PhysicalTerminalManager::get()->openPTY(0);
    for(;;) {
        Scheduler::get()->getActiveThread()->wait(new WaitForDelay(500));
        Scheduler::get()->forceThreadSwitchUserspace(NULL);
        //klog('i', "%i", PIT::get()->getTime());
        //KTRACEMEM
        char buf [1024];
        int c = p->read(buf, 1024);
        p->write(buf, c);
    }   
}


extern "C" void kmain () {
    CPU::enableSSE();
    CPU::CLI();
    CPU::CLTS();
    
    Memory::init();
    kalloc_switch_to_main_heap();
    klog_init();

    PhysicalTerminalManager::get()->init(5);
    klog_init_terminal();
    klog('w', "Kernel log started");
    PhysicalTerminalManager::get()->render();

    Debug::init();
    klog('i', "Setting IDT");
    IDT::get()->init();

    klog('i', "Time is %u", CMOS::get()->readTime());

    klog('i', "Configuring timer");
    PIT::get()->init();
    PIT::get()->setFrequency(250);
    PIT::MSG_TIMER.registerConsumer((MessageConsumer)&pit_handler);

    Keyboard::get()->init();
    Interrupts::get()->setHandler(IRQ(7),  INTERRUPT_MUTE);
    Interrupts::get()->setHandler(IRQ(15), INTERRUPT_MUTE);
    Interrupts::get()->setHandler(0x80, isrq80);

    AddressSpace::kernelSpace->dump();

    //MQ::post(Debug::MSG_DUMP_REGISTERS, NULL);

/*

    Directory* dir = fs->opendir("/");
    struct dirent* de;
    while (de = dir->read()) {
        klog('i', "%s%s", de->d_name, (de->d_type == DT_DIR) ? "/" : "");
    }
    dir->close();
*/


    FAT32FS* fs = new FAT32FS();
    File* f = fs->open("a.out", O_RDONLY);
    char* buf = (char*)kmalloc(1024*1024);
    int c =f->read(buf, 1024*1024);
    klog('i',"%i",c);
    f->close();

    KTRACEMEM

    klog('w', "Starting task scheduler");
    Scheduler::get()->init();
    Scheduler::get()->spawnKernelThread(&repainterThread, NULL, "Screen repainter thread");

    Scheduler::get()->spawnProcess((uint64_t)&testThread);
//    Scheduler::get()->spawnKernelThread(&testThread, NULL, "test thread");


    for (;;)
        CPU::halt();
}