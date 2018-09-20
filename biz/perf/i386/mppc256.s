	.text

	.macro	PUTBITS shift, data
	movd	\shift, %mm1
	movd	\data, %mm2
	psllq	%mm1, %mm2
	por	%mm2, %mm0
	.endm

	.macro	PUTLIT				# modify %eax %ebx %edx
	movzb	(%ecx), %eax
	mov	$-8, %edx
	mov	$-9, %ebx
	test	$0x80, %eax
	setne	%ah
	cmovne	%ebx, %edx
	movd	%mm1, %ebx	
	and	$0x17F, %eax
	add	%ebx, %edx
	movd	%eax, %mm2
	movd	%edx, %mm1
	add	$1, %ecx
	psllq	%mm1, %mm2
	por	%mm2, %mm0
	.endm

	.macro	UPDATE				# modify %ebx %edx
	movd	%mm1, %edx
	cmp	$32, %edx
	ja	.Lneed_not_update_\@
	pshufw	$0xE, %mm0, %mm2
	add	$32, %edx
	movd	%mm2, %ebx
	bswap	%ebx
	psllq	$32, %mm0
	movd	%edx, %mm1
	mov	%ebx, (%edi)
	add	$4, %edi
.Lneed_not_update_\@:
	.endm

	.macro	PUTLEN	cmp, and, or, len	# modify %eax %edx
	cmp	$\cmp, %eax
	jae	.Llen_ge_\cmp
	and	$\and, %eax
	add	$-\len, %edx
	or	$\or, %eax
	PUTBITS	%edx, %eax
	jmp	.Lupdate_next
	.align	2, 0x90
.Llen_ge_\cmp:
	.endm

#extern "C" unsigned char *mppc256_transform( unsigned char *obuf, size_t isize, unsigned char **histptr, unsigned char *hash[256]);	
.global mppc256_transform
	.type	mppc256_transform, @function
# %ebp -> r, %esi -> s, %edi -> obuf, %ecx -> histptr

	.align 2
mppc256_transform:
	add	$-16, %esp
	mov	%ebp, (%esp)
	mov	%esi, 4(%esp)
	mov	%edi, 8(%esp)
	mov	%ebx, 12(%esp)
	mov	28(%esp), %ecx
	mov	24(%esp), %ebp
	mov	20(%esp), %edi
	mov	$64, %eax
	pxor	%mm0, %mm0
	movd	%eax, %mm1
	mov	(%ecx), %esi
	cmp	$2, %ebp
	mov	%esi, %ecx
	jbe	.Llast_bytes
	add	%esi, %ebp
	mov	32(%esp), %edx
	.align	2, 0x90
.Lnext_bytes:
	movzb	(%esi), %eax
	mov	(%edx, %eax, 4), %ebx		# unsigned char *p = *q;
	cmp	%esi, %ebx			# %ebx -> p
	mov	%esi, (%edx, %eax, 4)		# *q = s;
	jb	.Lif_1
	PUTLIT
	mov	%ecx, %esi
	jmp	.Lupdate_next
	.align	2, 0x90
.Lif_1:
	movzwl	(%esi), %eax
	movzwl	(%ebx), %edx
	cmp	%eax, %edx
	jz	.Lif_2
	PUTLIT
	mov	%ecx, %esi
	jmp	.Lupdate_next
	.align	2, 0x90
.Lif_2:
	add	$2, %esi
	add	$2, %ebx
	movzb	(%esi), %eax
	movzb	(%ebx), %edx
	cmp	%eax, %edx
	jz	.Lelse
	PUTLIT
	mov	%ecx, %esi
	jmp	.Lupdate_next
	.align	2, 0x90
.Lelse:
	add	$1, %esi
	add	$1, %ebx
	cmp	%esi, %ebp
	jbe	.Lbreak_while
	movzb	(%esi), %eax
	movzb	(%ebx), %edx
	cmp	%eax, %edx
	jz	.Lelse
.Lbreak_while:
	mov	%esi, %eax
	neg	%ebx
	sub	%ecx, %eax		# %eax -> len
	mov	%esi, %ecx
	add	%esi, %ebx		# %ebx -> off
	movd	%mm1, %edx
	cmp	$64, %ebx
	jae	.Loff_ge_64
	add	$-10, %edx
	or	$0x3C0, %ebx
	PUTBITS %edx, %ebx
	jmp	.Loff_update
.Loff_ge_64:
	cmp	$320, %ebx
	jae	.Loff_ge_320
	add	$-64, %ebx
	add	$-12, %edx
	or	$0xE00, %ebx
	PUTBITS %edx, %ebx
	jmp	.Loff_update
.Loff_ge_320:
	add	$-320, %ebx
	add	$-16, %edx
	or	$0xC000, %ebx
	PUTBITS %edx, %ebx
.Loff_update:
	UPDATE
	cmp	$4, %eax
	movd	%mm1, %edx
	jae	.Llen_ge_4
	add	$-1, %edx
	movd	%edx, %mm1
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
	add	$-24, %edx
	or	$0xFFE000, %eax
	PUTBITS	%edx, %eax
	.align	2, 0x90
.Lupdate_next:
	UPDATE
	lea	2(%esi), %eax
	mov	32(%esp), %edx
	cmp	%eax, %ebp
	ja	.Lnext_bytes
	sub	%esi, %ebp
.Llast_bytes:
	cmp	$1, %ebp
	jb	.Lend_of_block
	je	.Lremain_1
	PUTLIT
.Lremain_1:
	PUTLIT
.Lend_of_block:
	movd	%mm1, %edx
	mov	$0x3C0, %eax
	add	$-10, %edx
	PUTBITS	%edx, %eax
	UPDATE
	psrlq	$32, %mm0
	movd	%mm1, %edx	
	movd	%mm0, %ebx
	cmp	$64, %edx
	bswap	%ebx
	mov	%edi, %eax
	jz	.Lfinal_none
	cmp	$56, %edx
	jb	.Lfinal_not_1
	mov	%bl, (%edi)
	add	$1, %eax
	jmp	.Lfinal_none
.Lfinal_not_1:
	cmp	$48, %edx
	jb	.Lfinal_not_2
	mov	%bx, (%edi)
	add	$2, %eax
	jmp	.Lfinal_none
.Lfinal_not_2:
	cmp	$40, %edx
	jb	.Lfinal_not_3
	mov	%ebx, %edx
	mov	%bx, (%edi)
	shr	$16, %edx
	mov	%dl, 2(%edi)
	add	$3, %eax
	jmp	.Lfinal_none
.Lfinal_not_3:
	mov	%ebx, (%edi)
	add	$4, %eax
.Lfinal_none:
	mov	28(%esp), %esi
	mov	%ecx, (%esi)
	emms
	mov	(%esp), %ebp
	mov	4(%esp), %esi
	mov	8(%esp), %edi
	mov	12(%esp), %ebx
	add	$16, %esp
	ret
