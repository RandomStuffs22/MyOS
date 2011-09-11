#include <hardware/Keyboard.h>
#include <interrupts/Interrupts.h>
#include <kutils.h>


void Keyboard::handle(isrq_registers_t r) {
    u8int scancode = inb(0x60);

    if (_escaping)
        _buffer *= 256;
    else
        _buffer = 0;
    _escaping = false;
    _buffer += scancode;

    if (_buffer == 0xE0) _escaping = true;

    else if (_buffer == 0x1D) _modifiers |=  KBD_MOD_CTRL;
    else if (_buffer == 0x9D) _modifiers &= !KBD_MOD_CTRL;
    else if (_buffer == 0x2A) _modifiers |=  KBD_MOD_SHIFT;
    else if (_buffer == 0xAA) _modifiers &= !KBD_MOD_SHIFT;
    else if (_buffer == 0x38) _modifiers |=  KBD_MOD_ALT;
    else if (_buffer == 0xB8) _modifiers &= !KBD_MOD_ALT;
    else if (_buffer == 0xE05B) _modifiers |=  KBD_MOD_META;
    else if (_buffer == 0xE0DB) _modifiers &= !KBD_MOD_META;

    else if ((_handler != NULL) && scancode >= 0x80) _handler(_modifiers, scancode);
}

static void kbdh(isrq_registers_t r) {
    Keyboard::get()->handle(r);
}

void Keyboard::init() {
    Interrupts::get()->setHandler(IRQ(1), kbdh);
    _buffer = 0;
    _modifiers = 0;
    _escaping = false;
    _handler = NULL;
}

void Keyboard::setHandler(keyboard_handler h) {
    _handler = h;
}
