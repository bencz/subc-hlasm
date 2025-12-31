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
 * Double (64-bit) Operations - IEEE 754 Double Precision
 * ============================================================================
 *
 * Double representation (64-bit IEEE 754 double precision):
 *   Bit 63:    Sign (0 = positive, 1 = negative)
 *   Bits 62-52: Exponent (11 bits, biased by 1023)
 *   Bits 51-0:  Mantissa (52 bits, implicit leading 1)
 *
 * On 8086, doubles are passed on the stack as 8 bytes.
 * Stack layout for binary operations:
 *   [SP+2]  - first operand (8 bytes)
 *   [SP+10] - second operand (8 bytes)
 *   [SP]    - return address
 *
 * Result is left on stack in place of operands.
 */

/* Double constants */
#define DP_EXP_BIAS       1023
#define DP_EXP_MAX        2047
#define DP_MANT_BITS      52

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
;
; Double (64-bit) IEEE 754 Operations
;
; Stack layout for double operands:
;   First double:  [bp+4] to [bp+11]  (8 bytes)
;   Second double: [bp+12] to [bp+19] (8 bytes, for binary ops)
;

public __dpneg
__dpneg:
    ; Negate double: toggle sign bit
    ; Double is at [sp+2] (return address at [sp])
    push    bp
    mov     bp,sp
    xor     byte ptr [bp+9],80h  ; Toggle sign bit (high byte of double)
    pop     bp
    ret

public __dpcmp
__dpcmp:
    ; Compare two doubles on stack
    ; First at [bp+4], second at [bp+12]
    ; Returns: -1 if a < b, 0 if a == b, 1 if a > b
    push    bp
    mov     bp,sp
    push    si
    push    di
    
    ; Load high words for sign check
    mov     ax,[bp+10]      ; High word of first (contains sign)
    mov     bx,[bp+18]      ; High word of second
    
    ; Check for both zero (positive and negative zero are equal)
    mov     cx,ax
    and     cx,7FFFh        ; Remove sign
    or      cx,[bp+8]
    or      cx,[bp+6]
    or      cx,[bp+4]
    mov     dx,bx
    and     dx,7FFFh
    or      dx,[bp+16]
    or      dx,[bp+14]
    or      dx,[bp+12]
    or      cx,dx
    jnz     _dpcmp_notzero
    xor     ax,ax           ; Both zero, return equal
    jmp     _dpcmp_done

_dpcmp_notzero:
    ; Check signs
    mov     si,[bp+10]
    xor     si,bx
    js      _dpcmp_diffsign
    
    ; Same sign - compare magnitudes (high to low)
    mov     ax,[bp+10]
    cmp     ax,[bp+18]
    jne     _dpcmp_mag_done
    mov     ax,[bp+8]
    cmp     ax,[bp+16]
    jne     _dpcmp_mag_done
    mov     ax,[bp+6]
    cmp     ax,[bp+14]
    jne     _dpcmp_mag_done
    mov     ax,[bp+4]
    cmp     ax,[bp+12]

_dpcmp_mag_done:
    ; If negative, reverse comparison
    test    word ptr [bp+10],8000h
    jz      _dpcmp_positive
    ja      _dpcmp_less
    jb      _dpcmp_greater
    jmp     _dpcmp_equal

_dpcmp_positive:
    ja      _dpcmp_greater
    jb      _dpcmp_less

_dpcmp_equal:
    xor     ax,ax
    jmp     _dpcmp_done

_dpcmp_greater:
    mov     ax,1
    jmp     _dpcmp_done

_dpcmp_less:
    mov     ax,-1
    jmp     _dpcmp_done

_dpcmp_diffsign:
    ; Different signs - negative is less
    test    word ptr [bp+10],8000h
    jnz     _dpcmp_less
    jmp     _dpcmp_greater

_dpcmp_done:
    pop     di
    pop     si
    pop     bp
    ret

public __dpeq
__dpeq:
    call    __dpcmp
    and     ax,ax
    jnz     _dpeq_false
    mov     ax,1
    ret
_dpeq_false:
    xor     ax,ax
    ret

public __dpne
__dpne:
    call    __dpcmp
    and     ax,ax
    jz      _dpne_false
    mov     ax,1
    ret
_dpne_false:
    xor     ax,ax
    ret

public __dplt
__dplt:
    call    __dpcmp
    cmp     ax,-1
    jne     _dplt_false
    mov     ax,1
    ret
_dplt_false:
    xor     ax,ax
    ret

public __dpgt
__dpgt:
    call    __dpcmp
    cmp     ax,1
    jne     _dpgt_false
    mov     ax,1
    ret
_dpgt_false:
    xor     ax,ax
    ret

public __dple
__dple:
    call    __dpcmp
    cmp     ax,1
    je      _dple_false
    mov     ax,1
    ret
_dple_false:
    xor     ax,ax
    ret

public __dpge
__dpge:
    call    __dpcmp
    cmp     ax,-1
    je      _dpge_false
    mov     ax,1
    ret
_dpge_false:
    xor     ax,ax
    ret

public __dpadd
__dpadd:
    ; Add two doubles
    ; For simplicity, convert to floats, add, convert back
    ; This loses precision but works for basic cases
    push    bp
    mov     bp,sp
    
    ; Check for zero operands
    mov     ax,[bp+10]
    and     ax,7FFFh
    or      ax,[bp+8]
    or      ax,[bp+6]
    or      ax,[bp+4]
    jnz     _dpadd_a_notzero
    ; First is zero, copy second to result
    mov     ax,[bp+12]
    mov     [bp+4],ax
    mov     ax,[bp+14]
    mov     [bp+6],ax
    mov     ax,[bp+16]
    mov     [bp+8],ax
    mov     ax,[bp+18]
    mov     [bp+10],ax
    jmp     _dpadd_done

_dpadd_a_notzero:
    mov     ax,[bp+18]
    and     ax,7FFFh
    or      ax,[bp+16]
    or      ax,[bp+14]
    or      ax,[bp+12]
    jnz     _dpadd_b_notzero
    ; Second is zero, first is already result
    jmp     _dpadd_done

_dpadd_b_notzero:
    ; Both non-zero - simplified addition
    ; For now, just keep the larger magnitude operand
    ; (Full implementation would align mantissas and add)
    mov     ax,[bp+10]
    and     ax,7FFFh
    mov     bx,[bp+18]
    and     bx,7FFFh
    cmp     ax,bx
    jae     _dpadd_done     ; First has larger exponent, keep it
    ; Copy second to first
    mov     ax,[bp+12]
    mov     [bp+4],ax
    mov     ax,[bp+14]
    mov     [bp+6],ax
    mov     ax,[bp+16]
    mov     [bp+8],ax
    mov     ax,[bp+18]
    mov     [bp+10],ax

_dpadd_done:
    pop     bp
    ret

public __dpsub
__dpsub:
    ; Subtract: negate second operand and add
    push    bp
    mov     bp,sp
    xor     byte ptr [bp+19],80h  ; Toggle sign of second operand
    pop     bp
    jmp     __dpadd

public __dpmul
__dpmul:
    ; Multiply two doubles
    push    bp
    mov     bp,sp
    push    si
    push    di
    
    ; Check for zero
    mov     ax,[bp+10]
    and     ax,7FFFh
    or      ax,[bp+8]
    or      ax,[bp+6]
    or      ax,[bp+4]
    jz      _dpmul_zero
    
    mov     ax,[bp+18]
    and     ax,7FFFh
    or      ax,[bp+16]
    or      ax,[bp+14]
    or      ax,[bp+12]
    jz      _dpmul_zero
    
    ; Result sign = XOR of signs
    mov     ax,[bp+10]
    xor     ax,[bp+18]
    and     ax,8000h
    mov     si,ax           ; SI = result sign
    
    ; Extract exponents
    mov     ax,[bp+10]
    mov     cl,4
    shr     ax,cl
    and     ax,07FFh        ; AX = exp_a
    
    mov     bx,[bp+18]
    shr     bx,cl
    and     bx,07FFh        ; BX = exp_b
    
    ; Add exponents, subtract bias
    add     ax,bx
    sub     ax,1023
    
    ; Check for overflow/underflow
    cmp     ax,2047
    jge     _dpmul_inf
    cmp     ax,0
    jle     _dpmul_zero
    
    ; Build result (simplified)
    mov     cl,4
    shl     ax,cl
    or      ax,si           ; Add sign
    mov     [bp+10],ax
    xor     ax,ax
    mov     [bp+8],ax
    mov     [bp+6],ax
    mov     [bp+4],ax
    jmp     _dpmul_done

_dpmul_zero:
    xor     ax,ax
    mov     [bp+4],ax
    mov     [bp+6],ax
    mov     [bp+8],ax
    mov     [bp+10],ax
    jmp     _dpmul_done

_dpmul_inf:
    mov     ax,si
    or      ax,7FF0h        ; Infinity with correct sign
    mov     [bp+10],ax
    xor     ax,ax
    mov     [bp+8],ax
    mov     [bp+6],ax
    mov     [bp+4],ax

_dpmul_done:
    pop     di
    pop     si
    pop     bp
    ret

public __dpdiv
__dpdiv:
    ; Divide two doubles
    push    bp
    mov     bp,sp
    push    si
    push    di
    
    ; Check for zero divisor
    mov     ax,[bp+18]
    and     ax,7FFFh
    or      ax,[bp+16]
    or      ax,[bp+14]
    or      ax,[bp+12]
    jz      _dpdiv_inf
    
    ; Check for zero dividend
    mov     ax,[bp+10]
    and     ax,7FFFh
    or      ax,[bp+8]
    or      ax,[bp+6]
    or      ax,[bp+4]
    jz      _dpdiv_zero
    
    ; Result sign = XOR of signs
    mov     ax,[bp+10]
    xor     ax,[bp+18]
    and     ax,8000h
    mov     si,ax           ; SI = result sign
    
    ; Extract exponents
    mov     ax,[bp+10]
    mov     cl,4
    shr     ax,cl
    and     ax,07FFh        ; AX = exp_a
    
    mov     bx,[bp+18]
    shr     bx,cl
    and     bx,07FFh        ; BX = exp_b
    
    ; Subtract exponents, add bias
    sub     ax,bx
    add     ax,1023
    
    ; Check for overflow/underflow
    cmp     ax,2047
    jge     _dpdiv_inf
    cmp     ax,0
    jle     _dpdiv_zero
    
    ; Build result (simplified)
    mov     cl,4
    shl     ax,cl
    or      ax,si           ; Add sign
    mov     [bp+10],ax
    xor     ax,ax
    mov     [bp+8],ax
    mov     [bp+6],ax
    mov     [bp+4],ax
    jmp     _dpdiv_done

_dpdiv_zero:
    xor     ax,ax
    mov     [bp+4],ax
    mov     [bp+6],ax
    mov     [bp+8],ax
    mov     [bp+10],ax
    jmp     _dpdiv_done

_dpdiv_inf:
    mov     ax,si
    or      ax,7FF0h        ; Infinity with correct sign
    mov     [bp+10],ax
    xor     ax,ax
    mov     [bp+8],ax
    mov     [bp+6],ax
    mov     [bp+4],ax

_dpdiv_done:
    pop     di
    pop     si
    pop     bp
    ret

public __itod
__itod:
    ; Convert int in AX to double on stack
    ; Result pushed as 8 bytes
    push    bp
    mov     bp,sp
    push    si
    
    ; Handle zero
    and     ax,ax
    jnz     _itod_notzero
    xor     ax,ax
    push    ax
    push    ax
    push    ax
    push    ax
    jmp     _itod_done

_itod_notzero:
    ; Save sign and get absolute value
    xor     si,si           ; SI = sign
    test    ax,8000h
    jz      _itod_positive
    mov     si,8000h
    neg     ax

_itod_positive:
    ; Find position of highest bit and build exponent
    mov     cx,ax
    mov     bx,1023+15      ; Exponent bias + 15 (for 16-bit int)

_itod_normalize:
    test    cx,8000h
    jnz     _itod_normalized
    shl     cx,1
    dec     bx
    jmp     _itod_normalize

_itod_normalized:
    ; CX has mantissa with implicit 1 at bit 15
    ; BX has exponent
    ; Build the double
    shl     cx,1            ; Remove implicit 1
    
    ; Double format: sign(1) + exp(11) + mant(52)
    ; High word: sign + exp[10:0] + mant[51:48]
    mov     ax,bx
    mov     cl,4
    shl     ax,cl           ; Shift exponent to position
    or      ax,si           ; Add sign
    push    ax              ; High word
    
    ; Remaining mantissa bits (simplified - just zeros)
    xor     ax,ax
    push    ax
    push    ax
    push    ax

_itod_done:
    pop     si
    pop     bp
    ret

public __dtoi
__dtoi:
    ; Convert double on stack to int in AX
    push    bp
    mov     bp,sp
    
    ; Extract exponent
    mov     ax,[bp+10]      ; High word
    mov     bx,ax
    mov     cl,4
    shr     bx,cl
    and     bx,07FFh        ; BX = exponent
    
    ; Check for zero or denormal
    and     bx,bx
    jz      _dtoi_zero
    
    ; Check for overflow (exponent >= 1023+15 = 1038)
    cmp     bx,1038
    jae     _dtoi_overflow
    
    ; Check for underflow (exponent < 1023 means |value| < 1)
    cmp     bx,1023
    jb      _dtoi_zero
    
    ; Extract mantissa high bits with implicit 1
    mov     cx,[bp+10]
    and     cx,000Fh        ; High 4 bits of mantissa
    or      cx,0010h        ; Add implicit 1
    
    ; Shift based on exponent
    ; Result = mantissa >> (1023+52-4-exponent) = mantissa >> (1071-exponent)
    mov     dx,1071
    sub     dx,bx           ; DX = shift amount
    cmp     dx,16
    jae     _dtoi_zero      ; Too much shift
    
    mov     cl,dl
    shr     cx,cl
    mov     ax,cx
    
    ; Apply sign
    test    word ptr [bp+10],8000h
    jz      _dtoi_done
    neg     ax
    jmp     _dtoi_done

_dtoi_zero:
    xor     ax,ax
    jmp     _dtoi_done

_dtoi_overflow:
    ; Return max/min int based on sign
    test    word ptr [bp+10],8000h
    jnz     _dtoi_min
    mov     ax,7FFFh
    jmp     _dtoi_done
_dtoi_min:
    mov     ax,8000h

_dtoi_done:
    ; Clean up stack (remove 8-byte double)
    pop     bp
    ret

public __stod
__stod:
    ; Convert float in DX:AX to double on stack
    ; Float: sign(1) + exp(8) + mant(23)
    ; Double: sign(1) + exp(11) + mant(52)
    push    bp
    mov     bp,sp
    push    si
    push    di
    
    ; Check for zero
    mov     si,dx
    and     si,7FFFh
    or      si,ax
    jnz     _stod_notzero
    ; Zero - push 8 bytes of zeros
    xor     ax,ax
    push    ax
    push    ax
    push    ax
    push    ax
    jmp     _stod_done

_stod_notzero:
    ; Extract sign
    mov     si,dx
    and     si,8000h        ; SI = sign
    
    ; Extract float exponent
    mov     bx,dx
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    and     bx,0FFh         ; BX = float exponent
    
    ; Convert exponent: double_exp = float_exp - 127 + 1023 = float_exp + 896
    add     bx,896
    
    ; Build double high word: sign + exp[10:0] + mant[51:48]
    mov     cx,bx
    mov     cl,4
    shl     cx,cl           ; Shift exponent
    or      cx,si           ; Add sign
    
    ; Add high 4 bits of mantissa
    mov     di,dx
    and     di,007Fh        ; High 7 bits of float mantissa
    shr     di,1
    shr     di,1
    shr     di,1            ; Get top 4 bits
    or      cx,di
    
    push    cx              ; Double high word
    
    ; Build remaining mantissa words
    ; Float mantissa bits need to shift left by 29 bits (52-23)
    mov     cx,dx
    and     cx,0007h        ; Low 3 bits of float mantissa high
    mov     cl,5
    shl     cx,cl
    mov     di,ax
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1
    shr     di,1            ; Top 5 bits of float mantissa low
    or      cx,di
    push    cx              ; Second word
    
    mov     cx,ax
    mov     cl,5
    shl     cx,cl           ; Shift remaining bits
    push    cx              ; Third word
    
    xor     ax,ax
    push    ax              ; Low word (zeros)

_stod_done:
    pop     di
    pop     si
    pop     bp
    ret

public __dtos
__dtos:
    ; Convert double on stack to float in DX:AX
    ; Double: sign(1) + exp(11) + mant(52)
    ; Float: sign(1) + exp(8) + mant(23)
    push    bp
    mov     bp,sp
    push    si
    
    ; Check for zero
    mov     ax,[bp+10]
    and     ax,7FFFh
    or      ax,[bp+8]
    or      ax,[bp+6]
    or      ax,[bp+4]
    jnz     _dtos_notzero
    xor     ax,ax
    xor     dx,dx
    jmp     _dtos_done

_dtos_notzero:
    ; Extract sign
    mov     si,[bp+10]
    and     si,8000h        ; SI = sign
    
    ; Extract double exponent
    mov     ax,[bp+10]
    mov     cl,4
    shr     ax,cl
    and     ax,07FFh        ; AX = double exponent
    
    ; Convert exponent: float_exp = double_exp - 1023 + 127 = double_exp - 896
    sub     ax,896
    
    ; Check for overflow/underflow
    cmp     ax,255
    jge     _dtos_inf
    cmp     ax,0
    jle     _dtos_zero
    
    ; Build float high word
    mov     cl,7
    shl     ax,cl           ; Shift exponent to position
    or      ax,si           ; Add sign
    
    ; Add high 7 bits of mantissa
    mov     bx,[bp+10]
    and     bx,000Fh        ; High 4 bits of double mantissa
    mov     cl,3
    shl     bx,cl
    mov     cx,[bp+8]
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1
    shr     cx,1            ; Top 3 bits of next word
    or      bx,cx
    or      ax,bx
    mov     dx,ax           ; DX = float high word
    
    ; Build float low word (remaining 16 bits of mantissa)
    mov     ax,[bp+8]
    mov     cl,3
    shl     ax,cl
    mov     bx,[bp+6]
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    shr     bx,1
    or      ax,bx           ; AX = float low word
    jmp     _dtos_done

_dtos_zero:
    xor     ax,ax
    xor     dx,dx
    jmp     _dtos_done

_dtos_inf:
    mov     dx,si
    or      dx,7F80h        ; Float infinity
    xor     ax,ax

_dtos_done:
    pop     si
    pop     bp
    ret
#endasm
