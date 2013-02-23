#include <interrupts/Interrupts.h>
#include <kutils.h>
#include <util/cpp.h>


#define IDT_SIZE 256

static interrupt_handler interrupt_handlers[IDT_SIZE];


void Interrupts::removeAllHandlers() {
    memset(interrupt_handlers, 0, sizeof(interrupt_handler)*IDT_SIZE);
}

void Interrupts::setHandler(int n, interrupt_handler h) {
    interrupt_handlers[n] = h;
}

void Interrupts::removeHandler(int n) {
    interrupt_handlers[n] = NULL;
}

static void default_interrupt_handler(isrq_registers_t* regs) {
    // Enable FPU
    u32int cr0;
    asm volatile(" mov %%cr0, %0": "=r"(cr0));
    cr0 &=0xFFFFFFF7;
    regs->err_code=cr0;
    asm volatile(" mov %0, %%cr0":: "r"(cr0));

    if (regs->int_no == 7) {
        // Mute INT7
        return;
    }
    klogn("INT ");
    klogn(to_dec(regs->int_no));
    klogn(" ");
    klog(to_hex(regs->err_code));
}

static void default_irq_handler(isrq_registers_t* regs) {
    char s[] = "  xx  xxxxxx";

    klogn("IRQ ");

    int n = regs->int_no;

    s[2] = (char)((int)'0' + n/100);
    s[3] = (char)((int)'0' + n/10%10);
    s[4] = (char)((int)'0' + n%10);

    s[6] = 0;
    klog(s);
}

extern "C" void isr_handler(isrq_registers_t regs) {
    regs.int_no %= 256;
    bool irq = (regs.int_no >= 32 && regs.int_no <= 47);
    int ino = regs.int_no;
    if (irq)
        regs.int_no -= 32;

    if (irq) {
        if (regs.int_no >= 8)
        {
            outb(0xA0, 0x20);
        }
        outb(0x20, 0x20);
    }


    if (interrupt_handlers[ino] != 0) {
        interrupt_handlers[ino](&regs);
    } else {
        if (!irq) {
            default_interrupt_handler(&regs);
        } else {
            default_irq_handler(&regs);
        }
    }
}