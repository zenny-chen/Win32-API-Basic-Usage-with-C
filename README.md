# Win32 API Basic Usage with C
Win32 API Basic Usage with C


```masm
; 汇编源文件func.asm

.model flat
.code

_CFunc   proc public

    mov     eax, [esp + 4]    ; EAX存放第一个参数
    mov     ecx, [esp + 8]    ; ECX存放第二个参数
    sub     eax, ecx
    ret
```
