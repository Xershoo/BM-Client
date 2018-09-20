	.text
	.macro	RC4_BLOCK
	add	$1, %al
	movzb	(%rdi, %rax), %rcx
	add	%cl, %bl
	movzb	(%rdi, %rbx), %rdx
	mov	%cl, (%rdi, %rbx)
	mov	%dl, (%rdi, %rax)
	add	%cl, %dl
	movzb	(%rdi, %rdx), %rcx
	.endm
	.macro	UPDATE_BYTE
	RC4_BLOCK
	xor	%cl, (%rsi)
	add	$1, %rsi
	add	$-1, %r10
	.endm
#extern "C" void rc4_init_context ( unsigned char context[264], void *key, unsigned int keylen );
#extern "C" void rc4_update ( unsigned char context[264], void *input, unsigned int inputlen );
.global rc4_init_context, rc4_update

	.align	2
rc4_init_context:
# context -> %rdi, key -> %rsi, keylen -> %rdx
	mov	%rdi, %rax
	mov	$0x0706050403020100, %r9 
	mov	$0x0808080808080808, %r8
.Linit_fill:
	mov	%r9, (%rax)
	add	$8, %rax
	add	%r8, %r9
	jnc	.Linit_fill
	xor	%r8, %r8
	xor	%rcx, %rcx
	mov	%rcx, (%rax)
	mov	%rsi, %r10
	lea	(%rdx, %rsi), %r11
.Lpass_fill:	
	movzb	(%rdi, %r8), %rax
	movzb	(%rsi), %rdx 
	add	%al, %cl
	add	%dl, %cl
	add	$1, %rsi
	movzb	(%rdi, %rcx), %rdx
	cmp	%rsi, %r11
	mov	%al, (%rdi, %rcx)
	mov	%dl, (%rdi, %r8)
	cmovz	%r10, %rsi
	add	$1, %r8
	cmp	$256, %r8
	jb	.Lpass_fill
	ret

	.align	2
rc4_update:
# context -> %rdi, buffer -> %rsi, keylen -> %rdx
	mov	%rbx, %r11
	test	%rdx, %rdx
	mov	%rdx, %r10
	mov	256(%rdi), %eax
	mov	260(%rdi), %ebx
	jz	.Lfinish
	test	$1, %esi
	jz	.Lword_align
	UPDATE_BYTE
	jz	.Lfinish
.Lword_align:
	test	$2, %esi
	jz	.Ldword_align
	UPDATE_BYTE
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
.Ldword_align:
	test	$4, %esi
	jz	.Lqword_align
	UPDATE_BYTE
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
.Lqword_align:
	lea	(%rsi, %r10), %rcx
	and	$0xFFFFFFFFFFFFFFF8, %rcx
	and	$7, %r10
	mov	%rcx, %r9
	cmp	%rsi, %rcx
	jz	.Llast_bytes
.Ldo_qword_align:
	RC4_BLOCK
	mov	%rcx, %r8
	RC4_BLOCK
	shl	$8, %rcx
	or	%rcx, %r8
	RC4_BLOCK
	shl	$16, %rcx
	or	%rcx, %r8
	RC4_BLOCK
	shl	$24, %rcx
	or	%rcx, %r8
	RC4_BLOCK
	shl	$32, %rcx
	or	%rcx, %r8
	RC4_BLOCK
	shl	$40, %rcx
	or	%rcx, %r8
	RC4_BLOCK
	shl	$48, %rcx
	or	%rcx, %r8
	RC4_BLOCK
	shl	$56, %rcx
	or	%rcx, %r8
	xor	%r8, (%rsi)
	add	$8, %rsi
	cmp	%rsi, %r9
	jnz	.Ldo_qword_align
.Llast_bytes:
	test	%r10, %r10
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
	RC4_BLOCK
	xor	%cl, (%rsi)
.Lfinish:
	mov	%eax, 256(%rdi)
	mov	%ebx, 260(%rdi)
	mov	%r11, %rbx
	ret

