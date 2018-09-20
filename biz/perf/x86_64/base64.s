	.text
        .align 2
#extern "C" void base64_encode(void *output, const void *input, unsigned long len);
#extern "C" unsigned long base64_decode(void *output, const void *input, unsigned long len);
	.globl base64_encode, base64_decode
base64_encode:
	push	%rbx
	mov	%rcx, %r9
	mov	%rdx, %r11
	call	.Lencode_table_next
	.ascii	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
.Lencode_table_next:
	pop	%r8
.Lloop_encode:
	cmp	$6, %r11
	jb	.Llast_bytes_encode_0
	mov	(%rsi), %r10d
	movzwl	4(%rsi), %edx
	shl	$32, %rdx
	or	%rdx, %r10
	bswap	%r10
	mov	%r10, %rax
	mov	%r10, %rbx
	mov	%r10, %rcx
	mov	%r10, %rdx
	shr	$58, %rax
	shr	$52, %rbx
	shr	$46, %rcx
	shr	$40, %rdx
	and	$63, %rbx
	and	$63, %rcx
	and	$63, %rdx
	movzb	(%r8, %rax), %eax
	movzb	(%r8, %rbx), %ebx
	movzb	(%r8, %rcx), %ecx
	movzb	(%r8, %rdx), %edx
	mov	%al, (%rdi)
	mov	%bl, 1(%rdi)
	mov	%cl, 2(%rdi)
	mov	%dl, 3(%rdi)
	mov	%r10, %rax
	mov	%r10, %rbx
	mov	%r10, %rcx
	mov	%r10, %rdx
	shr	$34, %rax
	shr	$28, %rbx
	shr	$22, %rcx
	shr	$16, %rdx
	and	$63, %rax
	and	$63, %rbx
	and	$63, %rcx
	and	$63, %rdx
	movzb	(%r8, %rax), %eax
	movzb	(%r8, %rbx), %ebx
	movzb	(%r8, %rcx), %ecx
	movzb	(%r8, %rdx), %edx
	mov	%al, 4(%rdi)
	mov	%bl, 5(%rdi)
	mov	%cl, 6(%rdi)
	mov	%dl, 7(%rdi)
	add	$8, %rdi
	add	$6, %rsi
	sub	$6, %r11
	jmp	.Lloop_encode
.Llast_bytes_encode_0:
	cmp	$3, %r11
	jb	.Llast_bytes_encode_1
	movzwl	(%rsi), %eax
	movzbl	2(%rsi), %edx
	shl	$16, %edx
	or	%edx, %eax
	bswap	%eax
	mov	%eax, %ebx
	mov	%eax, %ecx
	mov	%eax, %edx
	shr	$26, %eax
	shr	$20, %ebx
	shr	$14, %ecx
	shr	$8, %edx
	and	$63, %ebx
	and	$63, %ecx
	and	$63, %edx
	movzb	(%r8, %rax), %eax
	movzb	(%r8, %rbx), %ebx
	movzb	(%r8, %rcx), %ecx
	movzb	(%r8, %rdx), %edx
	mov	%al, (%rdi)
	mov	%bl, 1(%rdi)
	mov	%cl, 2(%rdi)
	mov	%dl, 3(%rdi)
	add	$4, %rdi
	add	$3, %rsi
	sub	$3, %r11
	jz	.Lfinish_encode
.Llast_bytes_encode_1:
	cmp	$2, %r11
	jb      .Llast_1_0
	movzwl	(%rsi), %eax
	bswap	%eax
	mov	%eax, %ebx
	mov	%eax, %ecx
	shr	$26, %eax
	shr	$20, %ebx
	shr	$14, %ecx
	and	$63, %ebx
	and	$63, %ecx
	movzb	(%r8, %rax), %eax
	movzb	(%r8, %rbx), %ebx
	movzb	(%r8, %rcx), %ecx
	mov	%al, (%rdi)
	mov	%bl, 1(%rdi)
	mov	%cl, 2(%rdi)
	movb	$0x3D, 3(%rdi)
	jmp     .Lfinish_encode
.Llast_1_0:
	test	%r11, %r11
	jz	.Lfinish_encode
	movzb	(%rsi), %eax
	bswap	%eax
	mov	%eax, %ebx
	shr	$26, %eax
	shr	$20, %ebx
	and	$63, %ebx
	movzb	(%r8, %rax), %eax
	movzb	(%r8, %rbx), %ebx
	mov	%al, (%rdi)
	mov	%bl, 1(%rdi)
	movb	$0x3D, 2(%rdi)
	movb	$0x3D, 3(%rdi)
.Lfinish_encode:
	pop	%rbx
	ret

        .align 2
base64_decode:
	push	%rbx
	mov	%rcx, %r9
	mov	%rdx, %r11
	xor	%r10, %r10
	call	.Ldecode_table_next
	.byte 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	.byte 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	.byte 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3E,0xFF,0xFF,0xFF,0x3F
	.byte 0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	.byte 0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E
	.byte 0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF
	.byte 0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28
	.byte 0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0xFF,0xFF,0xFF,0xFF,0xFF
.Ldecode_table_next:
	pop	%r8
.Lloop_decode:
	cmp	$4, %r11
	jbe	.Llast_bytes_decode
	movzb	(%rsi), %eax
	movzb	1(%rsi), %ebx
	movzb	2(%rsi), %ecx
	movzb	3(%rsi), %edx
	movzb	(%r8, %rax), %eax
	movzb	(%r8, %rbx), %ebx
	movzb	(%r8, %rcx), %ecx
	movzb	(%r8, %rdx), %edx
	shl	$26, %eax
	shl	$20, %ebx
	shl	$14, %ecx
	shl	$8,  %edx
	or	%ebx, %eax
	or	%ecx, %eax
	or	%edx, %eax
	bswap	%eax
	mov	%ax, (%rdi)	
	shr	$16, %eax
	mov	%al, 2(%rdi)
	add	$4, %rsi
	add	$3, %rdi
	add	$3, %r10
	sub	$4, %r11
	jmp	.Lloop_decode
.Llast_bytes_decode:
	jne	.Lfinish_decode
	movzb	(%rsi), %ebx
	movzb	(%r8, %rbx), %eax
	movzb	1(%rsi), %ebx
	shl	$2, %eax
	movzb	(%r8, %rbx), %ebx
	mov	%ebx, %edx
	shr	$4, %ebx
	shl	$4, %edx
	or	%ebx, %eax
	mov	%al, (%rdi)
	movzb	2(%rsi), %ebx
	add	$1, %r10
	cmp	$0x3D, %rbx
	jz	.Lfinish_decode
	movzb	(%r8, %rbx), %ebx
	mov	%ebx, %eax
	shr	$2, %ebx
	shl	$6, %eax
	or	%ebx, %edx
	mov	%dl, 1(%rdi)
	movzb	3(%rsi), %ebx
	add	$1, %r10
	cmp	$0x3D, %ebx
	jz	.Lfinish_decode
	movzb	(%r8, %rbx), %ebx
	add	$1, %r10
	or	%ebx, %eax
	mov	%al, 2(%rdi)
.Lfinish_decode:
	pop	%rbx
	mov	%r10, %rax
	ret

