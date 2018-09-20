	.text
	.macro	RC4_BLOCK
	add	$1, %al
	movzb	(%edi, %eax), %ecx
	add	%cl, %bl
	movzb	(%edi, %ebx), %edx
	mov	%cl, (%edi, %ebx)
	mov	%dl, (%edi, %eax)
	add	%cl, %dl
	movzb	(%edi, %edx), %ecx
	.endm
	.macro	UPDATE_BYTE
	RC4_BLOCK
	xor	%cl, (%esi)
	add	$1, %esi
	add	$-1, %ebp
	.endm
#extern "C" void rc4_init_context ( unsigned char context[264], void *key, unsigned int keylen );
#extern "C" void rc4_update ( unsigned char context[264], void *input, unsigned int inputlen );
.global rc4_init_context, rc4_update
	.type	rc4_init_context, @function
	.type	rc4_update, @function

	.align	2
rc4_init_context:
	add	$-16, %esp
	mov	%ebp, (%esp)
	mov	%ebx, 4(%esp)
	mov	%esi, 8(%esp)
	mov	%edi, 12(%esp)
	mov	20(%esp), %edi
	mov	24(%esp), %esi
	mov	28(%esp), %ebp
	mov	%edi, %ebx
	mov	$0x03020100, %eax 
.Linit_fill:
	mov	%eax, (%ebx)
	add	$4, %ebx
	add	$0x04040404, %eax
	jnc	.Linit_fill
	xor	%ecx, %ecx
	add	%esi, %ebp
	mov	%ecx, (%ebx)
	mov	%ecx, 4(%ebx)
	xor	%ebx, %ebx
.Lpass_fill:	
	movzb	(%edi, %ecx), %eax
	movzb	(%esi), %edx 
	add	%al, %bl
	add	%dl, %bl
	add	$1, %esi
	movzb	(%edi, %ebx), %edx
	cmp	%esi, %ebp
	mov	%al, (%edi, %ebx)
	mov	%dl, (%edi, %ecx)
	cmovz	24(%esp), %esi
	add	$1, %ecx
	cmp	$256, %ecx
	jb	.Lpass_fill
	mov	12(%esp), %edi
	mov	8(%esp), %esi
	mov	4(%esp), %ebx
	mov	(%esp), %ebp
	add	$16, %esp
	ret

	.align	2
rc4_update:
	add	$-16, %esp
	mov	%ebp, (%esp)
	mov	%ebx, 4(%esp)
	mov	%esi, 8(%esp)
	mov	%edi, 12(%esp)
	mov	28(%esp), %ebp
	mov	24(%esp), %esi
	mov	20(%esp), %edi
	test	%ebp, %ebp
	mov	256(%edi), %eax
	mov	260(%edi), %ebx
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
	lea	(%esi, %ebp), %ecx
	and	$3, %ebp
	and	$0xFFFFFFFC, %ecx
	mov	%ebp, 28(%esp)
	mov	%ecx, 24(%esp)
	cmp	%esi, %ecx
	jz	.Llast_bytes
	.align	2, 0x90
.Ldo_dword_align:
	RC4_BLOCK
	mov	%ecx, %ebp
	RC4_BLOCK
	shl	$8, %ecx
	or	%ecx, %ebp
	RC4_BLOCK
	shl	$16, %ecx
	or	%ecx, %ebp
	RC4_BLOCK
	shl	$24, %ecx
	or	%ecx, %ebp
	xor	%ebp, (%esi)
	add	$4, %esi
	cmp	%esi, 24(%esp)
	jnz	.Ldo_dword_align
.Llast_bytes:
	mov	28(%esp), %ebp
	test	%ebp, %ebp
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
	UPDATE_BYTE
	jz	.Lfinish
	RC4_BLOCK
	xor	%cl, (%esi)
.Lfinish:
	mov	%eax, 256(%edi)
	mov	%ebx, 260(%edi)
	mov	12(%esp), %edi
	mov	8(%esp), %esi
	mov	4(%esp), %ebx
	mov	(%esp), %ebp
	add	$16, %esp
	ret

