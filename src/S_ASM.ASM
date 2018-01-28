Ideal

P386
Model  use16 FARSTACK Large, C 
Locals

	PUBLIC  SortSimple32_, Cmp32

CodeSeg

	D	EQU	0

PROC	Cmp32 

	mov	ax, [es:esi]
	sub	ax, [es:edi]
	ret

	ENDP	

;char far* pArrFlat, unsigned int uiOffs,
 ;   int (far* pCmp)(), int iSizeItem, unsugned long dwSize );

PROC  SortSimple32_ 
      ARG  @@pArrFlat: DWORD, @@uiOffs: WORD, @@pCmp: DWORD
      ARG  @@iSizeItem: WORD, @@dwSize: DWORD
      USES  eax, ebx, ecx, edx, edi, esi, es

	les	bx, [@@pArrFlat - D]    ;es - data selector
	xor	ax, ax                  ;ax - sorte flag

	mov     edx, [@@dwSize - D]     ;edx = dwSize - 1
 	dec     edx
	xor	ecx, ecx                ;ecx - counter

@@loop1:
	or	ax, ax
	jnz     @@end_sort
	cmp	ecx, edx
	jge     @@end_sort

        push	ecx

	mov	ebx, edx
	sub	ebx, ecx
	mov	ax, 1
	xor	ecx, ecx
	movzx	esi, [@@iSizeItem - D]
	movzx	edi, [@@uiOffs - D]
	add	edi, esi
	movzx	esi, [@@uiOffs - D]
@@loop2:
	cmp	ecx, ebx
	jge     @@end_loop2

	push    ax
	call    [@@pCmp - D]
	cmp	ax, 0
	pop	ax
	jle	@@next_loop

        push	ecx
	movzx   ecx, [@@iSizeItem - D]
	call    near @@exchange
        pop     ecx
	xor	ax, ax

@@next_loop:
	push	ax
	movzx	eax, [@@iSizeItem - D]
	add     esi, eax
	add     edi, eax
        pop	ax
	inc	ecx
	jmp   	short @@loop2

@@end_loop2:
	pop	ecx

	inc	ecx
	jmp	short @@loop1

@@end_sort:

	ret


@@exchange:
	push	ds ax esi edi

        cld
	;mov     cx, [@@iSizeItem - D]
	push	es
	pop	ds
;asm     les     di, rightP
;asm     lds     si, leftP


        shr     cx, 1                   ; test for an odd number of bytes 
        jnc     @@xch_wordLoop

        mov     al, [es:edi]
        ;movsb                           ; swap bytes, advancing pointers 
	movs	[byte ptr edi], [byte ptr esi]   
        mov     [esi-1], al
        jz      @@xch_end               ; if CX was originally 1       


@@xch_wordLoop:
        mov     ax, [es:edi]
	;movsw           		; swap words, advancing pointers
	movs	[word ptr edi], [word ptr esi]
        mov     [esi-2], ax
        loop    @@xch_wordLoop

@@xch_end:
	pop     edi esi ax ds
	retn

	ENDP

	END

