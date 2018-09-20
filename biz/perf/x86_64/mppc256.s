	.text

	.macro	PUTLIT				# modify %eax %ebx %ecx
	movzb	(%r8), %eax
	mov	$-8, %ecx
	mov	$-9, %ebx
	test	$0x80, %eax
	setne	%ah
	cmovne	%ebx, %ecx
	and	$0x17F, %eax
	add	%edx, %ecx
	add	$1, %r8
	mov	%ecx, %edx
	.endm

	.macro	UPDATE				# modify %rbx %edx
	mov	%ecx, %edx
	cmp	$32, %edx
	ja	.Lneed_not_update_\@
	mov	%r11, %rbx
	add	$32, %edx
	bswap	%rbx
	shl	$32, %r11
	mov	%ebx, (%rdi)
	add	$4, %rdi
.Lneed_not_update_\@:
	.endm

	.macro	PUTLEN	cmp, and, or, len	# modify %eax %ecx
	cmp	$\cmp, %eax
	jae	.Llen_ge_\cmp
	and	$\and, %eax
	add	$-\len, %ecx
	or	$\or, %eax
	jmp	.Lupdate_next_pre
.Llen_ge_\cmp:
	.endm

#extern "C" unsigned char *mppc256_transform( unsigned char *obuf, size_t isize, unsigned char **histptr, unsigned char *hash[256]);	
.global mppc256_transform
# %r9 -> r, %rsi -> s, %rdi -> obuf, %r8 -> histptr

	.align 2
mppc256_transform:
	push	%rbx
	mov	%RdX, %r8
	mov	%RsI, %r9
	mov	%RcX, %r10
	xor	%r11, %r11
	mov	$64, %edx
	mov	(%r8), %rsi
	cmp	$2, %r9
	mov	%rsi, %r8
	jbe	.Llast_bytes
	add	%rsi, %r9
.Lnext_bytes:
	movzb	(%rsi), %rax
	mov	(%r10, %rax, 8), %rbx		# unsigned char *p = *q;
	cmp	%rsi, %rbx			# %rbx -> p
	mov	%rsi, (%r10, %rax, 8)		# *q = s;
	jb	.Lif_1
	PUTLIT
	mov	%r8, %rsi
	jmp	.Lupdate_next_pre
.Lif_1:
	movzwl	(%rsi), %eax
	movzwl	(%rbx), %ecx
	cmp	%eax, %ecx
	jz	.Lif_2
	PUTLIT
	mov	%r8, %rsi
	jmp	.Lupdate_next_pre
.Lif_2:
	add	$2, %rsi
	add	$2, %rbx
	movzb	(%rsi), %eax
	movzb	(%rbx), %ecx
	cmp	%eax, %ecx
	jz	.Lelse
	PUTLIT
	mov	%r8, %rsi
	jmp	.Lupdate_next_pre
.Lelse:
	add	$1, %rsi
	add	$1, %rbx
	cmp	%rsi, %r9
	jbe	.Lbreak_while
	movzb	(%rsi), %eax
	movzb	(%rbx), %ecx
	cmp	%eax, %ecx
	jz	.Lelse
.Lbreak_while:
	mov	%rsi, %rax
	neg	%rbx
	sub	%r8, %rax		# %rax -> len
	mov	%rsi, %r8
	add	%rsi, %rbx		# %rbx -> off
	mov	%edx, %ecx
	cmp	$64, %ebx		# %rbx must in dict range and use %ebx part sufficiently
	jae	.Loff_ge_64
	add	$-10, %ecx
	or	$0x3C0, %ebx
	jmp	.Loff_update
.Loff_ge_64:
	cmp	$320, %ebx
	jae	.Loff_ge_320
	add	$-64, %ebx
	add	$-12, %ecx
	or	$0xE00, %ebx
	jmp	.Loff_update
.Loff_ge_320:
	add	$-320, %ebx
	add	$-16, %ecx
	or	$0xC000, %ebx
.Loff_update:
	shl	%cl, %rbx
	or	%rbx, %r11
	UPDATE
	cmp	$4, %eax
	mov	%edx, %ecx
	jae	.Llen_ge_4
	add	$-1, %ecx
	mov	%ecx, %edx
	jmp	.Lupdate_next
.Llen_ge_4:
	PUTLEN	8, 3, 8, 4
	PUTLEN	16, 7, 0x30, 6
	PUTLEN	32, 0xF, 0xE0, 8 
	PUTLEN	64, 0x1F, 0x3C0, 10
	PUTLEN	128, 0x3F, 0xF80, 12 
	PUTLEN	256, 0x7F, 0x3F00, 14 
	PUTLEN	512, 0xFF, 0xFE00, 16 
	PUTLEN	1024, 0x1FF, 0x3FC00, 18 
	PUTLEN	2048, 0x3FF, 0xFF800, 20 
	PUTLEN	4096, 0x7FF, 0x3FF000, 22
	and	$0xFFF, %eax
	add	$-24, %ecx
	or	$0xFFE000, %eax
.Lupdate_next_pre:
	shl	%cl, %rax
	or	%rax, %r11
.Lupdate_next:
	UPDATE
	lea	2(%rsi), %rax
	cmp	%rax, %r9
	ja	.Lnext_bytes
	sub	%rsi, %r9
.Llast_bytes:
	cmp	$1, %r9
	jb	.Lend_of_block
	je	.Lremain_1
	PUTLIT
	shl	%cl, %rax
	or	%rax, %r11
.Lremain_1:
	PUTLIT
	shl	%cl, %rax
	or	%rax, %r11
.Lend_of_block:
	mov	%edx, %ecx
	mov	$0x3C0, %rax
	add	$-10, %ecx
	shl	%cl, %rax
	or	%rax, %r11
	UPDATE
	mov	%edx, %ecx
	mov	%r11, %rbx
	cmp	$64, %ecx
	bswap	%rbx
	mov	%rdi, %rax
	jz	.Lfinal_none
	cmp	$56, %ecx
	jb	.Lfinal_not_1
	mov	%bl, (%rdi)
	add	$1, %rax
	jmp	.Lfinal_none
.Lfinal_not_1:
	cmp	$48, %ecx
	jb	.Lfinal_not_2
	mov	%bx, (%rdi)
	add	$2, %rax
	jmp	.Lfinal_none
.Lfinal_not_2:
	cmp	$40, %ecx
	jb	.Lfinal_not_3
	mov	%ebx, %ecx
	mov	%bx, (%rdi)
	shr	$16, %ecx
	mov	%cl, 2(%rdi)
	add	$3, %rax
	jmp	.Lfinal_none
.Lfinal_not_3:
	mov	%ebx, (%rdi)
	add	$4, %rax
.Lfinal_none:
	mov	%r8, (%r8)
	pop	%rbx
	ret
