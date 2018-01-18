Ideal

P386
Model  use16 FARSTACK Large, C 
Locals

	PUBLIC  LimitUInt, LimitByte, LimitInt, LimitSByte
;int LimitInt( LPSTR )            //return 0 if Ok
;int LimitByte( LPSTR )

CodeSeg

PROC  LimitByte               ; 0..255     
      ARG  @@pStr: DWORD 
      USES bx, cx, dx, ds
      ;ax = 0 - Ok

	lds	bx, [@@pStr]
	xor	ax, ax		
	cmp	[byte ptr bx], 0
	Jnz	@@enter
	ret
@@enter:
	mov	cl, 10

@@loop:
	mul	cl
	jc	@@end

	cmp     [byte ptr bx], '0'
	jb      @@next_byte
	cmp     [byte ptr bx], '9'
	ja	@@next_byte

	mov	dl, [byte ptr bx]
        sub	dl, '0'
	add	al, dl
	jc	@@end

@@next_byte:
	inc	bx
	cmp	[byte ptr ds:bx], 0
	Jnz	@@loop

	xor	ax, ax
	ret
@@end:
	mov	ax, 1
	ret

	ENDP


PROC  LimitUInt 
      ARG  @@pStr: DWORD
      USES bx, cx, dx, ds

	lds	bx, [@@pStr]
	xor	ax, ax		
	cmp	[byte ptr bx], 0
	Jnz	@@enter
	ret
@@enter:
	mov	cx, 10

@@loop:
 	mul	cx
	jc	@@end

	cmp     [byte ptr bx], '0'
	jb      @@next_byte
	cmp     [byte ptr bx], '9'
	ja	@@next_byte

	movzx	dx, [byte ptr bx]
        sub	dx, '0'
	add	ax, dx
	jc	@@end

@@next_byte:
	inc	bx
	cmp	[byte ptr ds:bx], 0
	Jnz	@@loop

	xor	ax, ax
	ret
@@end:
	mov	ax, 1
	ret

	ENDP


PROC  LimitSByte              ;
      ARG  @@pStr: DWORD
      USES bx, cx, dx, ds
      ;ax = 0 - Ok

	lds	bx, [@@pStr]
	xor	ax, ax		
	cmp	[byte ptr bx], 0
	Jnz	@@enter
	ret
@@enter:
	mov	cx, 10

@@loop:
	imul	cl
	jc	@@end

	cmp     [byte ptr bx], '-'
	jnz     @@byte_check
	mov	ch, 1
	jmp 	short @@next_byte

@@byte_check:
	cmp     [byte ptr bx], '0'
	jb      @@next_byte
	cmp     [byte ptr bx], '9'
	ja	@@next_byte

	mov	dl, [byte ptr bx]
        sub	dl, '0'
	or	ch, ch
        jnz	@@signed
	add	al, dl
	jmp	short @@test_carry
@@signed:
	sub     al, dl
@@test_carry:
	jc	@@end

@@next_byte:
	inc	bx
	cmp	[byte ptr ds:bx], 0
	Jnz	@@loop

	xor	ax, ax
	ret
@@end:
	mov	ax, 1
	ret

	ENDP


PROC  LimitInt              ;
      ARG  @@pStr: DWORD
      USES bx, cx, dx, si, ds
      ;ax = 0 - Ok

	lds	bx, [@@pStr]
	xor	ax, ax		
	cmp	[byte ptr bx], 0
	Jnz	@@enter
	ret
@@enter:
	mov	cx, 10
        xor	si, si

@@loop:
	imul	cx
	jc	@@end

	cmp     [byte ptr bx], '-'
	jnz     @@byte_check
	mov	si, 1
	jmp 	short @@next_byte

@@byte_check:
	cmp     [byte ptr bx], '0'
	jb      @@next_byte
	cmp     [byte ptr bx], '9'
	ja	@@next_byte

	movzx	dx, [byte ptr bx]
        sub	dx, '0'
	or	si, si
        jnz	@@signed
	add	ax, dx
	jmp	short @@test_carry
@@signed:
	sub     ax, dx
@@test_carry:
	jc	@@end

@@next_byte:
	inc	bx
	cmp	[byte ptr ds:bx], 0
	Jnz	@@loop

	xor	ax, ax
	ret
@@end:
	mov	ax, 1
	ret

	ENDP


	END
