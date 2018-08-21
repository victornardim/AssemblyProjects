; this is an I/O library for assembly

section .text

; input:
; rdi - ponteiro para a string
; retorno:
; rax - tamanho da string
string_length:
	xor rax, rax
.loop:
	cmp byte[rdi + rax], 0 ; checa se nao rdi na 'iteracao' atual eh um caractere terminador nulo 0
			       ; o rax (contador) somado indica a posicao do endereco que deve ser olhado apos o endereco inicial
			       ; funciona como se fosse um array
			       ; exemplo:
			       ; rdi[7] = "string\0"
			       ; rax = 3
			       ; [rdi + rax] = rdi[3] = 'i'
			       ; isso por que rdi ja aponta naturalmente para o primeiro endereco de memoria
			       ; como se fosse o indice 0, onde esta contido o caractere 's'
			       ; rdi = 0x100 (endereco 100) onde esta contido o primeiro caractere 's'
			       ; portanto [rdi + rax] = 0x100 + 3 = 0x103 (endereco 103) onde esta contido o caractere 'i'

	je .end ; caso seja um caractere terminador nulo 0, finaliza a funcao
	inc rax ; caso nao, incrementa rax, que guarda o tamanho da string
	jmp .loop
.end:
	ret







; input:
; rdi - ponteiro para a string
print_string:
	call string_length
	mov rdx, rax
	mov rax, 1 ; write
	mov rsi, rdi
	mov rdi, 1 ; stdout
	syscall
	ret







; input:
; rdi - caractere a ser printado
print_char:
    	push rdi ; coloca o valor de rdi na pilha de hardware

	mov rdi, rsp ; move para rdi o valor do endereco de memoria do elemento no topo da pilha (rdi) que fica contido em rsp
		     ; pois print_string so aceita um ponteiro (endereco de memoria) para a string

	call print_string
	pop rdi
    	ret







print_newline:
    	mov rdi, 10
    	jmp print_char ; diferenca de jump para call eh que jump vai para a outra funcao e nao retorna para essa ao final
		       ; call vai para a outra funcao e ao finalizar volta para ca
		       ; este eh um exemplo de tail call







; input:
; rdi - inteiro nao sinalizado a ser printado
print_uint:
    	mov rax, rdi
    	mov rdi, rsp ; pega o endereco do ultimo elemento na pilha (exemplo 100)
    	push 0 ; adiciona um elemento na pilha, que decrementa rsp em 8 bytes (exemplo 92)
    	sub rsp, 16 ; subtrai 16 de rsp, garantindo um total de 24 bytes para armazenar a string (exemplo 76)
    
    	dec rdi ; decrementa rdi, que guarda o endereco da pilha inicialmente coletado
	    	; fazendo com que ele "entre" no espaco reservado (exemplo 99)

    	mov r8, 10

.loop:
	xor rdx, rdx
	div r8
	or  dl, 0x30 ; 30hex = 48dec ---todo numero quando somado 48 resultado em seu codigo ASCII
	dec rdi ; vai decrementando rdi para que cada caracter novo seja posicionado em endereco diferente
	mov [rdi], dl ; move o caracter ASCII referente ao resto da divisao para o endereco apontado por rdi
	test rax, rax
	jnz .loop
	   
	call print_string
	    
	add rsp, 24 ; adiciona 24 bytes (a mesma quantidade subtraida no comeco) para que rsp aponte para o comeco da string
	ret







; input:
; rdi - inteiro a ser printado
print_int:
	test rdi, rdi
	jns print_uint ; caso seja positivo, ja imprime
	push rdi ; caso nao armazena o inteiro
	mov rdi, '-'
	call print_char ; imprime o sinal de negativo
	pop rdi ; recupera o inteiro
	neg rdi ; executa uma negacao de complemento de dois (binariamente, inverte o numero e soma 1)
		; necessario pois como eh um negativo sua representacao binaria eh a inversa ao numero positivo
		; exemplo: 7 = 0111 --- -7 = inverte(1000) = 1000 + 0001 = 1001

	call print_uint ; imprime o numero
	ret	







; input:
; rdi - primeira string
; rsi - segunda string
; retorno:
; rax - falg booleana para indicar se as strings sao iguais
string_equals:
	mov al, byte[rdi] ; move um byte (caractere) para a menor parte de rax, para poder comparar
	cmp al, byte[rsi] ; compara um byte (caractere) de rdi (movido para al) com um byte (caractere) de rsi
	jne .nequal
	inc rdi ; caminha em um endereco de memoria, no endereco da string apontada por rdi
	inc rsi ; caminha em um endereco de memoria, no endereco da string apontada por rsi
	test al, al ; testa para saber se ja chegou no final da string (caractere nulo '0')
	jnz string_equals
	mov rax, 1
	ret
.nequal:
	xor rax, rax
	ret







; retorno:
; rsi - endereco do buffer que aponta para o endereco do caractere lido, na pilha de hardware
; rax - o caractere lido
read_char:
	push 0
	xor rax, rax ; rax = 0 ---read
	xor rdi, rdi ; rdi = 0 ---stdin
	mov rsi, rsp ; buffer = endereco do topo da pilha de hardware
	mov rdx, 1 ; tamanho do buffer
	syscall
	pop rax ; guarda o caractere lido
	ret







; input:
; rsi - o tamanho alocado para o buffer
; retorno:
; rax - ponteiro para a string lida
; rdx - o tamanho da string lida
read_word:
	; 10, 13 e 9 ---caracteres brancos
	; rax = onde fica armazenada a palavra ao final do metodo
	; rsi = tamanho reservado para o buffer de destino
	; rdx = tamanho da string lida

	mov r8, rsi ; tamanho reservado para o buffer de destino
	dec r8 ; desconsidera o caractere terminador nulo 0
	xor r9, r9 ; contador de caracteres movidos de stdin para rdi

.first:
	; nesta primeira parte eh feita uma tratativa para saber se a string eh composta
	; somente por caracteres em branco
	; caso seja, o metodo eh finalizado, devolvendo 0

	push rdi ; armazena o valor de rdi na pilha de hardware, pois ele sera alterado em read_char
	call read_char ; le o proximo caractere em stdin e o salva em rax
	pop rdi ; retorna o valor original de rdi
	cmp al, ' '
	je .first
	cmp al, 10
	je .first
	cmp al, 13
	je .first
	cmp al, 9
	je .first
	test al, al
	jz .end ; caso seja o final da string (caratere nulo 0), finaliza o metodo

.concat:
	; nesta parte eh feita a concatenacao dos caracteres lidos de stdin para rdi
	; isso acontece ate que seja encontrado um caracter em branco ou o buffer atinja o tamanho limite

	mov byte[rdi + r9], al ; move o caractere lido de stdin contido na menor parte de rax (al)
			       ; para rdi no endereco da 'iteracao' atual, indicada contador r9
	inc r9

	push rdi
	call read_char
	pop rdi
	cmp al, ' '
	je .end
	cmp al, 10
	je .end
	cmp al, 13
	je .end
	cmp al, 9
	je .end
	test al, al
	jz .end
	cmp r9, r8
	je .too_large ; caso o contador de caracteres movidos atinja o mesmo valor reservado para o buffer, finaliza o metodo
		      ; devolvendo 0, indicando que houve um erro na leitura da string
		      ; isto ocorre pois significa que a string de stdin eh maior do que o tamanho reservado para o buffer

	jmp .concat

.end:
	mov byte[rdi + r9], 0 ; move 0 (terminador nulo de string) para a ultima posicao da string em rdi
    	mov rax, rdi ; move a string lida para rax
	mov rdx, r9 ; move o tamanho da string lida para rax
	ret

.too_large:
	xor rax, rax ; zera rax, devolvendo 0, indicando que houve um problema com a string a ser lida (excedeu o tamanho)
	ret







; input:
; rdi - ponteiro para a string
; retorno:
; rax - numero para ser feito o parse
; rdx - tamanho do numero em caracteres
parse_uint:
	xor rax, rax
	xor r9, r9 ; contador
	mov r8, 10 ; base para multiplicacao
.loop:
	movzx rcx, byte[rdi + r9] ; move o caractere atual para rcx com zero extendido
				  ; ou seja, zera todos os bits mais significativos nao utilizados

	cmp cl, '0'
	jb .end ; se for menor que o caractere ASCII '0' nao eh numerico, entao finaliza
	cmp cl, '9'
	ja .end ; se for maior que o caractere ASCII '9' nao eh numerico, entao finaliza
	xor rdx, rdx ; zera rdx, que contem uma parte do resultado da multiplicacao feita nas outras iteracoes
	mul r8 ; multiplica o que esta em rax por 10, para que todo novo numero somado, seja colocado um digito a frente
	       ; desta forma a virgula sempre caminha uma casa para a direita
	       ; exemplo: 287
	       ; iteracao 1 --- 0 * 10 = 0 + 2 = 2
	       ; iteracao 2 --- 2 * 10 = 20 + 8 = 28
	       ; iteracao 3 --- 28 * 10 = 280 + 7 = 287

	and cl, 0x0f ; ignora os 4 bits mais significativos
	add rax, rcx ; adiciona o numero resultante da iteracao ao numero total
	inc r9
	jmp .loop

.end:
	mov rdx, r9
	ret







; input:
; rdi - ponteiro para a string
; retorno:
; rax - numero para ser feito o parse
; rdx - tamanho do numero em caracteres
parse_int:
	cmp byte[rdi], '-' ; para saber se eh um numero negativo
	je .negative
	jmp parse_uint

.negative:
	inc rdi ; desconsidera o sinal de negativo, para poder utilizar o metodo parse_uint
	call parse_uint
	test rdx, rdx
	jz .error ; caso o tamanho seja 0, algum erro ocorreu, entao retorna 0
	neg rax ; nega o numero binariamente utilizando complemento de dois, tornando ele negativo aritmeticamente
	inc rdx ; aumenta 1 no tamanho, em funcao do sinal de negativo que nao eh computado pelo parse_uint
	ret

.error:
	xor rax, rax
	ret







; input
; rdi - ponteiro para a string
; rdx - o tamanho alocado para o buffer
; retorno
; rax - tamanho da string
; rsi - ponteiro para a string copiada
string_copy:
	; salva todos os dados dos registradores rdi, rsi e rdx
	; para poder chamar a funcao string_length que altera o valor de alguns desses
	push rdi
	push rsi
	push rdx
	call string_length
	pop rdx
	pop rsi
	pop rdi
	
	cmp rax, rdx ; se o tamanho informado for menor do que a string, retorna 0
	jae .too_large

.copy_loop:
	mov dl, [rdi]
	mov [rsi], dl ; move byte a byte (caractere a caractere) da string para o buffer
	inc rdi ; caminha em um endereco de memoria, no endereco da string apontada por rdi
	inc rsi ; caminha em um endereco de memoria, no endereco da string apontada por rsi
	test dl, dl
	jnz .copy_loop ; testa para saber se ja chegou no final da string (caractere nulo '0')
	ret

.too_large:
	xor rax, rax
	ret
