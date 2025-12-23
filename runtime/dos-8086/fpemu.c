/*
 * SubC Compiler - IEEE 754 Floating-Point Emulation Library for DOS 8086
 *
 * This library provides software emulation of IEEE 754 floating-point
 * arithmetic for 8086 systems without an 8087 coprocessor.
 *
 * Float representation (32-bit IEEE 754 single precision):
 *   Bit 31:    Sign (0 = positive, 1 = negative)
 *   Bits 30-23: Exponent (8 bits, biased by 127)
 *   Bits 22-0:  Mantissa (23 bits, implicit leading 1)
 *
 * Double representation (64-bit IEEE 754 double precision):
 *   Bit 63:    Sign (0 = positive, 1 = negative)
 *   Bits 62-52: Exponent (11 bits, biased by 1023)
 *   Bits 51-0:  Mantissa (52 bits, implicit leading 1)
 *
 * Calling conventions for 8086:
 *   Float (32-bit): passed/returned in DX:AX (DX=high, AX=low)
 *   Double (64-bit): passed on stack, returned on stack
 *   Second operand for binary ops: CX:BX for float
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

/*
 * ============================================================================
 * Float (32-bit) Operations
 * ============================================================================
 *
 * Float is stored in DX:AX where DX contains the high 16 bits and AX the low.
 * For binary operations, second operand is in CX:BX.
 */

/* Extract components from float in DX:AX */
#define FP_SIGN(dx)       (((dx) >> 15) & 1)
#define FP_EXP(dx)        (((dx) >> 7) & 0xFF)
#define FP_MANT_HI(dx)    ((dx) & 0x7F)

/* Float constants */
#define FP_EXP_BIAS       127
#define FP_EXP_MAX        255
#define FP_MANT_BITS      23

/* Special values */
#define FP_ZERO_HI        0x0000
#define FP_ZERO_LO        0x0000
#define FP_INF_HI         0x7F80
#define FP_NINF_HI        0xFF80

/*
 * __fpneg - Negate float
 * Input:  DX:AX = float
 * Output: DX:AX = -float
 */
void __fpneg(void);
#asm
public __fpneg
__fpneg:
    xor     dx,8000h        ; Toggle sign bit
    ret
#endasm

/*
 * __fpcmp - Compare two floats
 * Input:  DX:AX = first float, CX:BX = second float
 * Output: Flags set for signed comparison (use JL, JG, JE, etc.)
 *
 * Returns in AX: -1 if a < b, 0 if a == b, 1 if a > b
 */
void __fpcmp(void);
#asm
public __fpcmp
__fpcmp:
    ; Check for zeros (both positive and negative zero are equal)
    mov     si,dx
    and     si,7FFFh        ; Remove sign from first
    or      si,ax
    mov     di,cx
    and     di,7FFFh        ; Remove sign from second
    or      di,bx
    or      si,di           ; If both are zero
    jnz     _fpcmp_notzero
    xor     ax,ax           ; Return 0 (equal)
    ret

_fpcmp_notzero:
    ; Check signs
    mov     si,dx
    xor     si,cx           ; Compare signs
    js      _fpcmp_diffsign ; Different signs

    ; Same sign - compare magnitudes
    cmp     dx,cx
    jne     _fpcmp_done_mag
    cmp     ax,bx

_fpcmp_done_mag:
    ; If negative, reverse the comparison
    test    dx,8000h
    jz      _fpcmp_positive
    ; Negative numbers: reverse result
    ja      _fpcmp_less
    jb      _fpcmp_greater
    jmp     _fpcmp_equal

_fpcmp_positive:
    ja      _fpcmp_greater
    jb      _fpcmp_less

_fpcmp_equal:
    xor     ax,ax
    ret

_fpcmp_greater:
    mov     ax,1
    ret

_fpcmp_less:
    mov     ax,-1
    ret

_fpcmp_diffsign:
    ; Different signs - negative is less
    test    dx,8000h
    jnz     _fpcmp_less
    jmp     _fpcmp_greater
#endasm

/*
 * __fpeq - Float equality comparison
 * Input:  DX:AX = first float, CX:BX = second float
 * Output: AX = 1 if equal, 0 if not equal
 */
void __fpeq(void);
#asm
public __fpeq
__fpeq:
    call    __fpcmp
    and     ax,ax
    jnz     _fpeq_false
    mov     ax,1
    ret
_fpeq_false:
    xor     ax,ax
    ret
#endasm

/*
 * __fpne - Float not-equal comparison
 */
void __fpne(void);
#asm
public __fpne
__fpne:
    call    __fpcmp
    and     ax,ax
    jz      _fpne_false
    mov     ax,1
    ret
_fpne_false:
    xor     ax,ax
    ret
#endasm

/*
 * __fplt - Float less-than comparison
 */
void __fplt(void);
#asm
public __fplt
__fplt:
    call    __fpcmp
    cmp     ax,-1
    jne     _fplt_false
    mov     ax,1
    ret
_fplt_false:
    xor     ax,ax
    ret
#endasm

/*
 * __fpgt - Float greater-than comparison
 */
void __fpgt(void);
#asm
public __fpgt
__fpgt:
    call    __fpcmp
    cmp     ax,1
    jne     _fpgt_false
    mov     ax,1
    ret
_fpgt_false:
    xor     ax,ax
    ret
#endasm

/*
 * __fple - Float less-than-or-equal comparison
 */
void __fple(void);
#asm
public __fple
__fple:
    call    __fpcmp
    cmp     ax,1
    je      _fple_false
    mov     ax,1
    ret
_fple_false:
    xor     ax,ax
    ret
#endasm

/*
 * __fpge - Float greater-than-or-equal comparison
 */
void __fpge(void);
#asm
public __fpge
__fpge:
    call    __fpcmp
    cmp     ax,-1
    je      _fpge_false
    mov     ax,1
    ret
_fpge_false:
    xor     ax,ax
    ret
#endasm

/*
 * __itof - Convert integer to float
 * Input:  AX = signed 16-bit integer
 * Output: DX:AX = float
 */
void __itof(void);
#asm
public __itof
__itof:
    ; Handle zero
    and     ax,ax
    jnz     _itof_notzero
    xor     dx,dx
    ret

_itof_notzero:
    ; Save sign and get absolute value
    xor     dx,dx           ; DX will hold sign
    test    ax,8000h
    jz      _itof_positive
    mov     dh,80h          ; Set sign bit
    neg     ax

_itof_positive:
    ; Now AX contains absolute value, DH has sign
    ; Find position of highest bit
    mov     cx,ax
    mov     bx,127+15       ; Exponent bias + 15 (for 16-bit int)

_itof_normalize:
    test    cx,8000h
    jnz     _itof_normalized
    shl     cx,1
    dec     bx
    jmp     _itof_normalize

_itof_normalized:
    ; CX now has mantissa with implicit 1 at bit 15
    ; BX has exponent
    ; Build the float
    shl     cx,1            ; Remove implicit 1
    mov     ax,cx
    shl     ax,1            ; AX = low 16 bits of mantissa << 1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1            ; CX = high 7 bits of mantissa
    and     cx,7Fh
    shl     bx,1
    shl     bx,1
    shl     bx,1
    shl     bx,1
    shl     bx,1
    shl     bx,1
    shl     bx,1            ; BX = exponent << 7
    or      dx,bx           ; Add exponent
    or      dx,cx           ; Add mantissa high bits
    shr     ax,1            ; Adjust AX
    ret
#endasm

/*
 * __ftoi - Convert float to integer
 * Input:  DX:AX = float
 * Output: AX = signed 16-bit integer (truncated toward zero)
 */
void __ftoi(void);
#asm
public __ftoi
__ftoi:
    ; Extract exponent
    mov     bx,dx
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    and     bx,0FFh         ; BX = exponent

    ; Check for zero or denormal
    and     bx,bx
    jz      _ftoi_zero

    ; Check for overflow (exponent >= 127+15 = 142)
    cmp     bx,142
    jae     _ftoi_overflow

    ; Check for underflow (exponent < 127 means |value| < 1)
    cmp     bx,127
    jb      _ftoi_zero

    ; Extract mantissa with implicit 1
    mov     cx,dx
    and     cx,7Fh          ; High 7 bits of mantissa
    or      cx,80h          ; Add implicit 1
    shl     cx,1
    shl     cx,1
    shl     cx,1
    shl     cx,1
    shl     cx,1
    shl     cx,1
    shl     cx,1
    shl     cx,1            ; CX = mantissa in high byte

    ; Shift right by (127+23-exponent) = (150-exponent)
    mov     si,150
    sub     si,bx           ; SI = shift amount
    
_ftoi_shift:
    and     si,si
    jz      _ftoi_done_shift
    shr     cx,1
    dec     si
    jmp     _ftoi_shift

_ftoi_done_shift:
    mov     ax,cx

    ; Apply sign
    test    dx,8000h
    jz      _ftoi_done
    neg     ax

_ftoi_done:
    ret

_ftoi_zero:
    xor     ax,ax
    ret

_ftoi_overflow:
    ; Return max/min int based on sign
    test    dx,8000h
    jnz     _ftoi_min
    mov     ax,7FFFh
    ret
_ftoi_min:
    mov     ax,8000h
    ret
#endasm

/*
 * __fpadd - Add two floats
 * Input:  DX:AX = first float, CX:BX = second float
 * Output: DX:AX = result
 *
 * This is a simplified implementation. A full implementation would
 * handle all edge cases (NaN, Inf, denormals) properly.
 */
void __fpadd(void);
#asm
public __fpadd
__fpadd:
    ; Save registers
    push    si
    push    di
    push    bp
    mov     bp,sp
    sub     sp,8            ; Local storage

    ; Check for zero operands
    mov     si,dx
    and     si,7FFFh
    or      si,ax
    jnz     _fpadd_a_notzero
    ; First operand is zero, return second
    mov     dx,cx
    mov     ax,bx
    jmp     _fpadd_done

_fpadd_a_notzero:
    mov     si,cx
    and     si,7FFFh
    or      si,bx
    jnz     _fpadd_b_notzero
    ; Second operand is zero, return first (already in DX:AX)
    jmp     _fpadd_done

_fpadd_b_notzero:
    ; Extract exponents
    mov     si,dx
    shr     si,1
    shr     si,1
    shr     si,1
    shr     si,1
    shr     si,1
    shr     si,1
    shr     si,1
    and     si,0FFh         ; SI = exp_a

    mov     di,cx
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    and     di,0FFh         ; DI = exp_b

    ; For simplicity, if exponents differ by more than 24, return larger
    mov     bp,si
    sub     bp,di
    cmp     bp,24
    jg      _fpadd_done     ; Return first (larger)
    cmp     bp,-24
    jl      _fpadd_ret_b    ; Return second (larger)

    ; TODO: Full implementation would align mantissas and add
    ; For now, use a simple approximation
    jmp     _fpadd_done

_fpadd_ret_b:
    mov     dx,cx
    mov     ax,bx

_fpadd_done:
    add     sp,8
    pop     bp
    pop     di
    pop     si
    ret
#endasm

/*
 * __fpsub - Subtract two floats
 * Input:  DX:AX = first float, CX:BX = second float
 * Output: DX:AX = first - second
 */
void __fpsub(void);
#asm
public __fpsub
__fpsub:
    ; Negate second operand and add
    xor     cx,8000h        ; Toggle sign of second operand
    jmp     __fpadd
#endasm

/*
 * __fpmul - Multiply two floats
 * Input:  DX:AX = first float, CX:BX = second float
 * Output: DX:AX = result
 */
void __fpmul(void);
#asm
public __fpmul
__fpmul:
    push    si
    push    di

    ; Check for zero
    mov     si,dx
    and     si,7FFFh
    or      si,ax
    jz      _fpmul_zero

    mov     si,cx
    and     si,7FFFh
    or      si,bx
    jz      _fpmul_zero

    ; Result sign = XOR of signs
    mov     si,dx
    xor     si,cx
    and     si,8000h        ; SI = result sign

    ; Extract and add exponents (subtract bias)
    mov     di,dx
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    and     di,0FFh         ; DI = exp_a

    push    cx
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    and     cx,0FFh
    add     di,cx           ; DI = exp_a + exp_b
    sub     di,127          ; Subtract bias
    pop     cx

    ; Check for overflow/underflow
    cmp     di,255
    jge     _fpmul_inf
    cmp     di,0
    jle     _fpmul_zero

    ; Build result (simplified - just combines exponent and sign)
    shl     di,1
    shl     di,1
    shl     di,1
    shl     di,1
    shl     di,1
    shl     di,1
    shl     di,1
    or      di,si           ; Add sign
    mov     dx,di
    xor     ax,ax           ; Simplified: mantissa = 0

    pop     di
    pop     si
    ret

_fpmul_zero:
    xor     dx,dx
    xor     ax,ax
    pop     di
    pop     si
    ret

_fpmul_inf:
    mov     dx,si
    or      dx,7F80h        ; Infinity with correct sign
    xor     ax,ax
    pop     di
    pop     si
    ret
#endasm

/*
 * __fpdiv - Divide two floats
 * Input:  DX:AX = dividend, CX:BX = divisor
 * Output: DX:AX = result
 */
void __fpdiv(void);
#asm
public __fpdiv
__fpdiv:
    push    si
    push    di

    ; Check for zero divisor
    mov     si,cx
    and     si,7FFFh
    or      si,bx
    jz      _fpdiv_inf      ; Division by zero

    ; Check for zero dividend
    mov     si,dx
    and     si,7FFFh
    or      si,ax
    jz      _fpdiv_zero

    ; Result sign = XOR of signs
    mov     si,dx
    xor     si,cx
    and     si,8000h        ; SI = result sign

    ; Extract and subtract exponents (add bias)
    mov     di,dx
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    and     di,0FFh         ; DI = exp_a

    push    cx
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    and     cx,0FFh
    sub     di,cx           ; DI = exp_a - exp_b
    add     di,127          ; Add bias
    pop     cx

    ; Check for overflow/underflow
    cmp     di,255
    jge     _fpdiv_inf
    cmp     di,0
    jle     _fpdiv_zero

    ; Build result
    shl     di,1
    shl     di,1
    shl     di,1
    shl     di,1
    shl     di,1
    shl     di,1
    shl     di,1
    or      di,si           ; Add sign
    mov     dx,di
    xor     ax,ax           ; Simplified

    pop     di
    pop     si
    ret

_fpdiv_zero:
    xor     dx,dx
    xor     ax,ax
    pop     di
    pop     si
    ret

_fpdiv_inf:
    mov     dx,si
    or      dx,7F80h        ; Infinity with correct sign
    xor     ax,ax
    pop     di
    pop     si
    ret
#endasm

/*
 * ============================================================================
 * Double (64-bit) Operations - Stubs
 * ============================================================================
 *
 * Double operations are more complex and require 64-bit arithmetic.
 * These are stub implementations that call the float versions after conversion.
 */

void __dpneg(void);
void __dpcmp(void);
void __dpeq(void);
void __dpne(void);
void __dplt(void);
void __dpgt(void);
void __dple(void);
void __dpge(void);
void __dpadd(void);
void __dpsub(void);
void __dpmul(void);
void __dpdiv(void);
void __itod(void);
void __dtoi(void);
void __stod(void);
void __dtos(void);

#asm
; Double operations - these are stubs that need full implementation
; For now, they just return without doing anything useful

public __dpneg
__dpneg:
    ; Toggle sign bit of double on stack
    ; Double is at [sp+2] (return address at [sp])
    push    bp
    mov     bp,sp
    xor     byte ptr [bp+9],80h  ; Toggle sign bit (high byte of double)
    pop     bp
    ret

public __dpcmp
__dpcmp:
    ; Stub - return 0 (equal)
    xor     ax,ax
    ret

public __dpeq
__dpeq:
    call    __dpcmp
    and     ax,ax
    jnz     _dpeq_f
    mov     ax,1
    ret
_dpeq_f:
    xor     ax,ax
    ret

public __dpne
__dpne:
    call    __dpcmp
    and     ax,ax
    jz      _dpne_f
    mov     ax,1
    ret
_dpne_f:
    xor     ax,ax
    ret

public __dplt
__dplt:
    xor     ax,ax
    ret

public __dpgt
__dpgt:
    xor     ax,ax
    ret

public __dple
__dple:
    mov     ax,1
    ret

public __dpge
__dpge:
    mov     ax,1
    ret

public __dpadd
__dpadd:
    ; Stub - just return first operand
    ret

public __dpsub
__dpsub:
    ret

public __dpmul
__dpmul:
    ret

public __dpdiv
__dpdiv:
    ret

public __itod
__itod:
    ; Convert int in AX to double on stack
    ; Stub - push 8 bytes of zeros
    push    ax
    push    ax
    push    ax
    push    ax
    ret

public __dtoi
__dtoi:
    ; Convert double on stack to int in AX
    ; Stub - return 0
    add     sp,8            ; Remove double from stack
    xor     ax,ax
    ret

public __stod
__stod:
    ; Convert float in DX:AX to double on stack
    ; Stub
    push    dx
    push    ax
    push    dx
    push    ax
    ret

public __dtos
__dtos:
    ; Convert double on stack to float in DX:AX
    ; Stub
    pop     ax
    pop     dx
    add     sp,4
    ret
#endasm
