	.text
        .align 2
#extern "C" void base64_encode(void *output, const void *input, unsigned long len);
#extern "C" unsigned long base64_decode(void *output, const void *input, unsigned long len);
	.globl base64_encode, base64_decode
        .type    base64_encode, @function
        .type    base64_decode, @function
base64_encode:
	push	%ebp
	push	%esi
	push	%edi
	push	%ebx
	mov	20(%esp), %edi
	mov	24(%esp), %esi
	mov	28(%esp), %ecx
	call	.Lencode_table_next
	.ascii	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
.Lencode_table_next:
	pop	%ebp
.Lloop_encode:
	cmp	$2, %ecx
	jbe	.Llast_bytes_encode

	movzb	(%esi), %eax
	movzwl	1(%esi), %edx
	shl	$8, %edx
	or	%edx, %eax

	bswap	%eax

	mov	%eax, %ebx
	shr	$26, %ebx
	movzb	(%ebp, %ebx), %edx
	mov	%dl, (%edi)

	mov	%eax, %ebx
	shr	$20, %ebx
	and	$63, %ebx
	movzb	(%ebp, %ebx), %edx
	mov	%dl, 1(%edi)

	mov	%eax, %ebx
	shr	$14, %ebx
	and	$63, %ebx
	movzb	(%ebp, %ebx), %edx
	mov	%dl, 2(%edi)

	shr	$8, %eax
	and	$63, %eax
	movzb	(%ebp, %eax), %edx
	mov	%dl, 3(%edi)

	add	$4, %edi
	add	$3, %esi
	sub	$3, %ecx
	jmp	.Lloop_encode
.Llast_bytes_encode:
	jb	.Llast_1_0

	movzwl	(%esi), %eax

	bswap	%eax

	mov	%eax, %ebx
	shr	$26, %ebx
	movzb	(%ebp, %ebx), %edx
	mov	%dl, (%edi)

	mov	%eax, %ebx
	shr	$20, %ebx
	and	$63, %ebx
	movzb	(%ebp, %ebx), %edx
	mov	%dl, 1(%edi)

	mov	%eax, %ebx
	shr	$14, %ebx
	and	$63, %ebx
	movzb	(%ebp, %ebx), %edx
	mov	%dl, 2(%edi)

	mov	$0x3D, %dl
	mov	%dl, 3(%edi)

	jmp	.Lfinish_encode
.Llast_1_0:
	test	%ecx, %ecx
	jz	.Lfinish_encode
	movzb	(%esi), %eax

	bswap	%eax
	
	mov	%eax, %ebx
	shr	$26, %ebx
	movzb	(%ebp, %ebx), %edx
	mov	%dl, (%edi)

	mov	%eax, %ebx
	shr	$20, %ebx
	and	$63, %ebx
	movzb	(%ebp, %ebx), %edx
	mov	%dl, 1(%edi)

	mov	$0x3D, %dl
	mov	%dl, 2(%edi)
	mov	%dl, 3(%edi)
.Lfinish_encode:
	pop	%ebx
	pop	%edi
	pop	%esi
	pop	%ebp
	ret

        .align 2
base64_decode:
	push	%ebp
	push	%esi
	push	%edi
	push	%ebx
	xor	%eax, %eax
	push	%eax
	mov	24(%esp), %edi
	mov	28(%esp), %esi
	mov	32(%esp), %ecx
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
	pop	%ebp
.Lloop_decode:
	cmp	$4, %ecx
	jbe	.Llast_bytes_decode

	movzb	(%esi), %ebx
	movzb	(%ebp, %ebx), %eax

	movzb	1(%esi), %ebx
	shl	$2, %eax
	movzb	(%ebp, %ebx), %ebx
	mov	%ebx, %edx
	shr	$4, %ebx
	shl	$4, %edx
	or	%ebx, %eax
	
	movzb	2(%esi), %ebx
	mov	%al, (%edi)
	movzb	(%ebp, %ebx), %ebx
	mov	%ebx, %eax
	shr	$2, %ebx
	shl	$6, %eax
	or	%ebx, %edx

	movzb	3(%esi), %ebx
	mov	%dl, 1(%edi)
	movzb	(%ebp, %ebx), %ebx
	or	%ebx, %eax

	add	$4, %esi
	sub	$4, %ecx
	mov	%al, 2(%edi)
	add	$3, %edi
	add	$3, (%esp)
	jmp	.Lloop_decode

.Llast_bytes_decode:
	jne	.Lfinish_decode

	movzb	(%esi), %ebx
	movzb	(%ebp, %ebx), %eax

	movzb	1(%esi), %ebx
	shl	$2, %eax
	movzb	(%ebp, %ebx), %ebx
	mov	%ebx, %edx
	shr	$4, %ebx
	shl	$4, %edx
	or	%ebx, %eax
	mov	%al, (%edi)

	movzb	2(%esi), %ebx
	add	$1, (%esp)
	cmp	$0x3D, %ebx
	jz	.Lfinish_decode

	movzb	(%ebp, %ebx), %ebx
	mov	%ebx, %eax
	shr	$2, %ebx
	shl	$6, %eax
	or	%ebx, %edx
	mov	%dl, 1(%edi)

	movzb	3(%esi), %ebx
	add	$1, (%esp)
	cmp	$0x3D, %ebx
	jz	.Lfinish_decode

	movzb	(%ebp, %ebx), %ebx
	add	$1, (%esp)
	or	%ebx, %eax
	mov	%al, 2(%edi)
.Lfinish_decode:
	pop	%eax
	pop	%ebx
	pop	%edi
	pop	%esi
	pop	%ebp
	ret

