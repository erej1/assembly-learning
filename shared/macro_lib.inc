; useful macros for fasm
; basically entirely based on this tutorial https://gpfault.net/posts/asm-tut-1.txt.html

; import directory table (NEEDS TO BE USED TOGETHER WITH import_functions)
macro import_directory_table [lib] {
; IDT entry
forward
        dd rva IAT__#lib
        dd 0
        dd 0
        dd rva NAME__#lib
        dd rva IAT__#lib

; termination happens after all the entries
common
        dd 5 dup(0)

; name strings
forward
        NAME__#lib db `lib, ".DLL", 0
}

; import functions from imported libs (NEEDS TO BE USED TOGETHER WITH import_directory_table)
macro import_functions libname, [funcnames] {
; hint/name table
forward
        ; proper alignment
        if $ & 1
           db 0
        end if
        IMPORTNAME__#funcnames dw 0
                               db `funcnames, 0
; IAT definition
common
        IAT__#libname:
; these point to the hint/name table, and are then overwritten at runtime
forward
        funcnames dq rva IMPORTNAME__#funcnames
; terminate whole IAT
common
        dq 0
}

; put argument into proper register, used by 'call64' macro
macro call64_putreg param*, reg* {
      if ~ (reg eqtype rax)
         display "target must be a register"
         err
      end if
      if ~ param eq reg
         mov reg, param
      end if
}

; call with Win64 calling convention, for GP arguments
macro call64gp fn*, [arg] {
common
      ; assembly time variables for handling different amounts of arguments, values set later
      local nargs, arg_idx, stack_space

      ; reserve stack space and align, assuming the stack alignment is 16+8 when called
      if nargs <= 4
         ; shadow space and padding accounting the caller return addres
         stack_space = 5 * 8
      else if nargs & 1
         ; odd amount of  arguments, reserve just nargs * 8
         ; this automatically accounts for the caller return address padding
         stack_space = nargs * 8
      else
         ; even amount of arguments, need the + 1 to account for caller return address padding
         stack_space = (nargs + 1) * 8
      end if

      if stack_space
         ; the actual allocation using the just calculated stack_space
         sub rsp, stack_space
      end if

      arg_idx = 0

forward
      ; loop through arguments and push them to registers or stack
      match ,arg \{
         ; this skips the next block if no arguments were given,
         ; for some reason forward wouldn't do that otherwise
      \}
      match any,arg \{
         ; passes first 4 args to registers and the rest are pushed to stack
         ; keeps track of the argument number with the arg_idx variable
         arg_idx = arg_idx + 1
         if arg_idx = 1
            call64_putreg arg, rcx
         else if arg_idx = 2
            call64_putreg arg, rdx
         else if arg_idx = 3
            call64_putreg arg, r8
         else if arg_idx = 4
            call64_putreg arg, r9
         else
            mov qword [rsp + (arg_idx-1)*8], arg
         end if
      \}
common
      ; and finally set the nargs, calculated with arg_idx
      nargs = arg_idx

      ; call the function
      call fn

      ; clean up stack after function has returned
      if stack_space
         add rsp, stack_space
      end if
}