%include "lib.inc"

global _start

section .data

; change the values for your operation here
first_number: dq 20
second_number: dq 2

; set the operation here
; operations
; 1 - addition
; 2 - subtraction
; 3 - multiplication
; 4 - division
operation: dq 1


; messages
msg_division_by_zero: db 'Error: division by 0!', 10
msg_invalid_operation: db 'Invalid operation!', 10


section .text

choose_operation:
	mov r9, [operation]
	cmp r9, 1
	je addition
	cmp r9, 2
	je subtraction
	cmp r9, 3
	je multiplication
	cmp r9, 4
	je division

	mov rdi, msg_invalid_operation
	call print_string ; located in lib.inc file
	call end

addition:
	mov rax, [first_number]
	add rax, [second_number]
	ret

subtraction:
	mov rax, [first_number]
	sub rax, [second_number]
	ret

multiplication:
	mov rax, [first_number] ; the first factor must be on the rax
	imul qword[second_number] ; the result is on the rax
	ret

division:
	mov r8, [second_number]
	test r8, r8
	jz .division_by_zero ; if the divider is zero, shows a error message
	mov rax, [first_number] ; the dividend must be on the rax
	xor rdx, rdx ; clear the rdx before the division, because rdx can affect the result if not cleared
	idiv r8 ; the division result is on rax
	ret
.division_by_zero:
	mov rdi, msg_division_by_zero
	call print_string ; located in lib.inc file
	call end

end:
	mov rax, 60 ; system_exit code
	xor rdx, rdx ; return code
	syscall

_start:
	xor rax, rax ; clear rax before the operation
	call choose_operation
	
	mov rdi, rax ; move the result to rdi to print it

	; both located in lib.inc file
	call print_int
	call print_newline

	call end
