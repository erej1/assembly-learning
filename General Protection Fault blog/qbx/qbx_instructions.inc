; macros for the qbx vm https://gpfault.net/posts/asm-tut-2.txt.html

;
macro qbx_insns m, [arg*] {
common
        m arg, \
        noop, \
        halt
}