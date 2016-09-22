; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

num 		equ 0

;-----------------------LCD_OutDec-----------------------
; Output a 24-bit number in unsigned decimal format
; Input: R0 (call by value) 24-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; Iterative implimentation
LCD_OutDec
	mov r12, lr			; save link register
	sub sp, #8			; allocate local variable
	mov r1, #0			; set null as the end char
	str r1, [sp, #num]	; store end char to the stack
start
	sub sp, #8          ; allocate local variable
	str r0, [sp, #num]	; store local variable (param)
	cmp r0, #10			; base case (num < 10)
	bhs skip_base
	add r0, #0x30		; convert to ASCII
	bl ST7735_OutChar	; print digit
	add sp, #8			; deallocate num
	b nxt_digit			; print rest of the digits
skip_base
	mov r2, #10
	udiv r0, r0, r2		; shift num right one decimal place
	b start
nxt_digit
	ldr r0, [sp, #num]
	cmp r0, #0
	beq done
	mov r2, #10
	udiv r1, r0, r2     ; start mod 10
	mul r1, r1, r2
	sub r0, r1		    ; end mod 10
	add r0, #0x30       ; convert to ASCII
	bl ST7735_OutChar   ; output char
	add sp, #8			; deallocate local variable
	b nxt_digit			; 'return'
done
	add sp, #8			; deallocate end char (= '\0')
	mov lr, r12			; restore link register
    bx lr
;* * * * * * * * End of LCD_OutDec * * * * * * * *

link equ 0	; BINDING
n    equ 8
cnt  equ 16
dig  equ 24

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 24-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
; Recursive implimentation
LCD_OutFix
	mov r1, #3
start_Fix
	sub sp, #32			; ALLOCATION
	str lr, [sp, #link]
	str r0, [sp, #n]
	str r1, [sp, #cnt]
	mov r2, #10			; mod 10 start
	udiv r1, r0, r2
	mul r1, r1, r2
	sub r0, r1			; mod 10 end
	add r0, #0x30		; convert to ASCII
	str r0, [sp, #dig]
	ldr r0, [sp, #cnt]	; ACCESS (any load of a local variable)
	cmp r0, #0
	beq base_case       ; cnt == 0
	ldr r0, [sp, #n]    ; load current number
	mov r2, #10
	udiv r0, r0, r2     ; devide number by 10 (shift right one decimal place)
	ldr r1, [sp, #cnt]  ; decrement counter
	sub r1, #1
	bl start_Fix		; LCD_OutFix(n // 10, cnt - 1)
	ldr r0, [sp, #dig]	; load digit once others before print
	cmp r12, #1			; check 'not too big' boolean
	beq no_star_1		
	mov r0, #0x2A		; if too big, print only stars
no_star_1
	bl ST7735_OutChar
	ldr lr, [sp, #link]
	add sp, #32			; DEALLOCATION (once before every return)
	bx lr
base_case				; cnt == 0
	mov r12, #1			; 'not too big' boolean
	ldr r0, [sp, #n]
	cmp r0, #10
	blo not_too_big
	mov r12, #0
not_too_big      		; n is < 10 after count hits 0
	ldr r0, [sp, #dig]
	cmp r12, #1			; check if not too big
	beq no_star			; if too big, set r0 to asterisk, else, br no star
	mov r0, #0x2A
no_star
	bl ST7735_OutChar	; print first char
	mov r0, #0x2E
	bl ST7735_OutChar	; print decimal
	ldr lr, [sp, #link]
	add sp, #32			; DEALLOCATION
	bx lr
	
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file