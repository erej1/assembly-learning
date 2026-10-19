format PE64 NX GUI  6.0
entry start

include '../shared/macro_lib_gpf.inc'

; import data
section '.idata' import readable writeable
        import_directory_table KERNEL32, USER32
        import_functions KERNEL32, ExitProcess
        import_functions USER32, MessageBoxA

; read only data
section '.rdata' data readable
        HelloStr db "Hello, World!", 0

; the actual code
section '.text' code readable executable
start:
        int3
        call64gp [MessageBoxA], 0, HelloStr, HelloStr, 0
        int3
        call64gp [ExitProcess], 0
