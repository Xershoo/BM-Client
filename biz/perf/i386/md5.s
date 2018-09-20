	.text
	.macro	MD5_ROUND64
	//0
	mov	%ecx, %edi
	mov	(%esi), %ebp
	xor	%edx, %edi
	and	%ebx, %edi
	lea	0xD76AA478(%eax, %ebp, 1), %eax
	xor	%edx, %edi
	add	%edi, %eax
	mov	%ebx, %edi
	rol	$7, %eax
	mov	4(%esi), %ebp
	add	%ebx, %eax
	//1
	xor	%ecx, %edi
	and	%eax, %edi
	lea	0xE8C7B756(%edx, %ebp, 1), %edx
	xor	%ecx, %edi
	add	%edi, %edx
	mov	%eax, %edi
	rol	$12, %edx
	mov	8(%esi), %ebp
	add	%eax, %edx
	//2
	xor	%ebx, %edi
	and	%edx, %edi
	lea	0x242070DB(%ecx, %ebp, 1), %ecx
	xor	%ebx, %edi
	add	%edi, %ecx
	mov	%edx, %edi
	rol	$17, %ecx
	mov	12(%esi), %ebp
	add	%edx, %ecx
	//3
	xor	%eax, %edi
	and	%ecx, %edi
	lea	0xC1BDCEEE(%ebx, %ebp, 1), %ebx
	xor	%eax, %edi
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$22, %ebx
	mov	16(%esi), %ebp
	add	%ecx, %ebx
	//4
	xor	%edx, %edi
	and	%ebx, %edi
	lea	0xF57C0FAF(%eax, %ebp, 1), %eax
	xor	%edx, %edi
	add	%edi, %eax
	mov	%ebx, %edi
	rol	$7, %eax
	mov	20(%esi), %ebp
	add	%ebx, %eax
	//5
	xor	%ecx, %edi
	and	%eax, %edi
	lea	0x4787C62A(%edx, %ebp, 1), %edx
	xor	%ecx, %edi
	add	%edi, %edx
	mov	%eax, %edi
	rol	$12, %edx
	mov	24(%esi), %ebp
	add	%eax, %edx
	//6
	xor	%ebx, %edi
	and	%edx, %edi
	lea	0xA8304613(%ecx, %ebp, 1), %ecx
	xor	%ebx, %edi
	add	%edi, %ecx
	mov	%edx, %edi
	rol	$17, %ecx
	mov	28(%esi), %ebp
	add	%edx, %ecx
	//7
	xor	%eax, %edi
	and	%ecx, %edi
	lea	0xFD469501(%ebx, %ebp, 1), %ebx
	xor	%eax, %edi
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$22, %ebx
	mov	32(%esi), %ebp
	add	%ecx, %ebx
	//8
	xor	%edx, %edi
	and	%ebx, %edi
	lea	0x698098D8(%eax, %ebp, 1), %eax
	xor	%edx, %edi
	add	%edi, %eax
	mov	%ebx, %edi
	rol	$7, %eax
	mov	36(%esi), %ebp
	add	%ebx, %eax
	//9
	xor	%ecx, %edi
	and	%eax, %edi
	lea	0x8B44F7AF(%edx, %ebp, 1), %edx
	xor	%ecx, %edi
	add	%edi, %edx
	mov	%eax, %edi
	rol	$12, %edx
	mov	40(%esi), %ebp
	add	%eax, %edx
	//10
	xor	%ebx, %edi
	and	%edx, %edi
	lea	0xFFFF5BB1(%ecx, %ebp, 1), %ecx
	xor	%ebx, %edi
	add	%edi, %ecx
	mov	%edx, %edi
	rol	$17, %ecx
	mov	44(%esi), %ebp
	add	%edx, %ecx
	//11
	xor	%eax, %edi
	and	%ecx, %edi
	lea	0x895CD7BE(%ebx, %ebp, 1), %ebx
	xor	%eax, %edi
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$22, %ebx
	mov	48(%esi), %ebp
	add	%ecx, %ebx
	//12
	xor	%edx, %edi
	and	%ebx, %edi
	lea	0x6B901122(%eax, %ebp, 1), %eax
	xor	%edx, %edi
	add	%edi, %eax
	mov	%ebx, %edi
	rol	$7, %eax
	mov	52(%esi), %ebp
	add	%ebx, %eax
	//13
	xor	%ecx, %edi
	and	%eax, %edi
	lea	0xFD987193(%edx, %ebp, 1), %edx
	xor	%ecx, %edi
	add	%edi, %edx
	mov	%eax, %edi
	rol	$12, %edx
	mov	56(%esi), %ebp
	add	%eax, %edx
	//14
	xor	%ebx, %edi
	and	%edx, %edi
	lea	0xA679438E(%ecx, %ebp, 1), %ecx
	xor	%ebx, %edi
	add	%edi, %ecx
	mov	%edx, %edi
	rol	$17, %ecx
	mov	60(%esi), %ebp
	add	%edx, %ecx
	//15
	xor	%eax, %edi
	and	%ecx, %edi
	lea	0x49B40821(%ebx, %ebp, 1), %ebx
	xor	%eax, %edi
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$22, %ebx
	mov	4(%esi), %ebp
	add	%ecx, %ebx
	//16
	lea	0xF61E2562(%eax, %ebp, 1), %eax
	xor	%ebx, %edi
	and	%edx, %edi
	mov	24(%esi), %ebp
	xor	%ecx, %edi
	add	%edi, %eax
	mov	%ebx, %edi
	rol	$5, %eax
	add	%ebx, %eax
	//17
	lea	0xC040B340(%edx, %ebp, 1), %edx
	xor	%eax, %edi
	and	%ecx, %edi
	mov	44(%esi), %ebp
	xor	%ebx, %edi
	add	%edi, %edx
	mov	%eax, %edi
	rol	$9, %edx
	add	%eax, %edx
	//18
	lea	0x265E5A51(%ecx, %ebp, 1), %ecx
	xor	%edx, %edi
	and	%ebx, %edi
	mov	(%esi), %ebp
	xor	%eax, %edi
	add	%edi, %ecx
	mov	%edx, %edi
	rol	$14, %ecx
	add	%edx, %ecx
	//19
	lea	0xE9B6C7AA(%ebx, %ebp, 1), %ebx
	xor	%ecx, %edi
	and	%eax, %edi
	mov	20(%esi), %ebp
	xor	%edx, %edi
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$20, %ebx
	add	%ecx, %ebx
	//20
	lea	0xD62F105D(%eax, %ebp, 1), %eax
	xor	%ebx, %edi
	and	%edx, %edi
	mov	40(%esi), %ebp
	xor	%ecx, %edi
	add	%edi, %eax
	mov	%ebx, %edi
	rol	$5, %eax
	add	%ebx, %eax
	//21
	lea	0x2441453(%edx, %ebp, 1), %edx
	xor	%eax, %edi
	and	%ecx, %edi
	mov	60(%esi), %ebp
	xor	%ebx, %edi
	add	%edi, %edx
	mov	%eax, %edi
	rol	$9, %edx
	add	%eax, %edx
	//22
	lea	0xD8A1E681(%ecx, %ebp, 1), %ecx
	xor	%edx, %edi
	and	%ebx, %edi
	mov	16(%esi), %ebp
	xor	%eax, %edi
	add	%edi, %ecx
	mov	%edx, %edi
	rol	$14, %ecx
	add	%edx, %ecx
	//23
	lea	0xE7D3FBC8(%ebx, %ebp, 1), %ebx
	xor	%ecx, %edi
	and	%eax, %edi
	mov	36(%esi), %ebp
	xor	%edx, %edi
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$20, %ebx
	add	%ecx, %ebx
	//24
	lea	0x21E1CDE6(%eax, %ebp, 1), %eax
	xor	%ebx, %edi
	and	%edx, %edi
	mov	56(%esi), %ebp
	xor	%ecx, %edi
	add	%edi, %eax
	mov	%ebx, %edi
	rol	$5, %eax
	add	%ebx, %eax
	//25
	lea	0xC33707D6(%edx, %ebp, 1), %edx
	xor	%eax, %edi
	and	%ecx, %edi
	mov	12(%esi), %ebp
	xor	%ebx, %edi
	add	%edi, %edx
	mov	%eax, %edi
	rol	$9, %edx
	add	%eax, %edx
	//26
	lea	0xF4D50D87(%ecx, %ebp, 1), %ecx
	xor	%edx, %edi
	and	%ebx, %edi
	mov	32(%esi), %ebp
	xor	%eax, %edi
	add	%edi, %ecx
	mov	%edx, %edi
	rol	$14, %ecx
	add	%edx, %ecx
	//27
	lea	0x455A14ED(%ebx, %ebp, 1), %ebx
	xor	%ecx, %edi
	and	%eax, %edi
	mov	52(%esi), %ebp
	xor	%edx, %edi
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$20, %ebx
	add	%ecx, %ebx
	//28
	lea	0xA9E3E905(%eax, %ebp, 1), %eax
	xor	%ebx, %edi
	and	%edx, %edi
	mov	8(%esi), %ebp
	xor	%ecx, %edi
	add	%edi, %eax
	mov	%ebx, %edi
	rol	$5, %eax
	add	%ebx, %eax
	//29
	lea	0xFCEFA3F8(%edx, %ebp, 1), %edx
	xor	%eax, %edi
	and	%ecx, %edi
	mov	28(%esi), %ebp
	xor	%ebx, %edi
	add	%edi, %edx
	mov	%eax, %edi
	rol	$9, %edx
	add	%eax, %edx
	//30
	lea	0x676F02D9(%ecx, %ebp, 1), %ecx
	xor	%edx, %edi
	and	%ebx, %edi
	mov	48(%esi), %ebp
	xor	%eax, %edi
	add	%edi, %ecx
	mov	%edx, %edi
	rol	$14, %ecx
	add	%edx, %ecx
	//31
	lea	0x8D2A4C8A(%ebx, %ebp, 1), %ebx
	xor	%ecx, %edi
	and	%eax, %edi
	mov	20(%esi), %ebp
	xor	%edx, %edi
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$20, %ebx
	add	%ecx, %ebx
	//32
	xor	%edx, %edi
	xor	%ebx, %edi
	lea	0xFFFA3942(%eax, %ebp, 1), %eax
	add	%edi, %eax
	mov	32(%esi), %ebp
	rol	$4, %eax
	mov	%ebx, %edi
	//33
	lea	0x8771F681(%edx, %ebp, 1), %edx
	add	%ebx, %eax
	xor	%ecx, %edi
	xor	%eax, %edi
	mov	44(%esi), %ebp
	add	%edi, %edx
	mov	%eax, %edi
	rol	$11, %edx
	add	%eax, %edx
	//34
	xor	%ebx, %edi
	xor	%edx, %edi
	lea	0x6D9D6122(%ecx, %ebp, 1), %ecx
	add	%edi, %ecx
	mov	56(%esi), %ebp
	rol	$16, %ecx
	mov	%edx, %edi
	//35
	lea	0xFDE5380C(%ebx, %ebp, 1), %ebx
	add	%edx, %ecx
	xor	%eax, %edi
	xor	%ecx, %edi
	mov	4(%esi), %ebp
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$23, %ebx
	add	%ecx, %ebx
	//36
	xor	%edx, %edi
	xor	%ebx, %edi
	lea	0xA4BEEA44(%eax, %ebp, 1), %eax
	add	%edi, %eax
	mov	16(%esi), %ebp
	rol	$4, %eax
	mov	%ebx, %edi
	//37
	lea	0x4BDECFA9(%edx, %ebp, 1), %edx
	add	%ebx, %eax
	xor	%ecx, %edi
	xor	%eax, %edi
	mov	28(%esi), %ebp
	add	%edi, %edx
	mov	%eax, %edi
	rol	$11, %edx
	add	%eax, %edx
	//38
	xor	%ebx, %edi
	xor	%edx, %edi
	lea	0xF6BB4B60(%ecx, %ebp, 1), %ecx
	add	%edi, %ecx
	mov	40(%esi), %ebp
	rol	$16, %ecx
	mov	%edx, %edi
	//39
	lea	0xBEBFBC70(%ebx, %ebp, 1), %ebx
	add	%edx, %ecx
	xor	%eax, %edi
	xor	%ecx, %edi
	mov	52(%esi), %ebp
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$23, %ebx
	add	%ecx, %ebx
	//40
	xor	%edx, %edi
	xor	%ebx, %edi
	lea	0x289B7EC6(%eax, %ebp, 1), %eax
	add	%edi, %eax
	mov	(%esi), %ebp
	rol	$4, %eax
	mov	%ebx, %edi
	//41
	lea	0xEAA127FA(%edx, %ebp, 1), %edx
	add	%ebx, %eax
	xor	%ecx, %edi
	xor	%eax, %edi
	mov	12(%esi), %ebp
	add	%edi, %edx
	mov	%eax, %edi
	rol	$11, %edx
	add	%eax, %edx
	//42
	xor	%ebx, %edi
	xor	%edx, %edi
	lea	0xD4EF3085(%ecx, %ebp, 1), %ecx
	add	%edi, %ecx
	mov	24(%esi), %ebp
	rol	$16, %ecx
	mov	%edx, %edi
	//43
	lea	0x4881D05(%ebx, %ebp, 1), %ebx
	add	%edx, %ecx
	xor	%eax, %edi
	xor	%ecx, %edi
	mov	36(%esi), %ebp
	add	%edi, %ebx
	mov	%ecx, %edi
	rol	$23, %ebx
	add	%ecx, %ebx
	//44
	xor	%edx, %edi
	xor	%ebx, %edi
	lea	0xD9D4D039(%eax, %ebp, 1), %eax
	add	%edi, %eax
	mov	48(%esi), %ebp
	rol	$4, %eax
	mov	%ebx, %edi
	//45
	lea	0xE6DB99E5(%edx, %ebp, 1), %edx
	add	%ebx, %eax
	xor	%ecx, %edi
	xor	%eax, %edi
	mov	60(%esi), %ebp
	add	%edi, %edx
	mov	%eax, %edi
	rol	$11, %edx
	add	%eax, %edx
	//46
	xor	%ebx, %edi
	xor	%edx, %edi
	lea	0x1FA27CF8(%ecx, %ebp, 1), %ecx
	add	%edi, %ecx
	mov	8(%esi), %ebp
	rol	$16, %ecx
	mov	%edx, %edi
	//47
	lea	0xC4AC5665(%ebx, %ebp, 1), %ebx
	add	%edx, %ecx
	xor	%eax, %edi
	xor	%ecx, %edi
	mov	(%esi), %ebp
	add	%edi, %ebx
	mov	$0xFFFFFFFF, %edi
	rol	$23, %ebx
	xor	%edx, %edi
	add	%ecx, %ebx
	//48
	or	%ebx, %edi
	lea	0xF4292244(%eax, %ebp, 1), %eax
	xor	%ecx, %edi
	mov	28(%esi), %ebp
	add	%edi, %eax
	mov	$0xFFFFFFFF, %edi
	rol	$6, %eax
	xor	%ecx, %edi
	add	%ebx, %eax
	//49
	or	%eax, %edi
	lea	0x432AFF97(%edx, %ebp, 1), %edx
	xor	%ebx, %edi
	mov	56(%esi), %ebp
	add	%edi, %edx
	mov	$0xFFFFFFFF, %edi
	rol	$10, %edx
	xor	%ebx, %edi
	add	%eax, %edx
	//50
	or	%edx, %edi
	lea	0xAB9423A7(%ecx, %ebp, 1), %ecx
	xor	%eax, %edi
	mov	20(%esi), %ebp
	add	%edi, %ecx
	mov	$0xFFFFFFFF, %edi
	rol	$15, %ecx
	xor	%eax, %edi
	add	%edx, %ecx
	//51
	or	%ecx, %edi
	lea	0xFC93A039(%ebx, %ebp, 1), %ebx
	xor	%edx, %edi
	mov	48(%esi), %ebp
	add	%edi, %ebx
	mov	$0xFFFFFFFF, %edi
	rol	$21, %ebx
	xor	%edx, %edi
	add	%ecx, %ebx
	//52
	or	%ebx, %edi
	lea	0x655B59C3(%eax, %ebp, 1), %eax
	xor	%ecx, %edi
	mov	12(%esi), %ebp
	add	%edi, %eax
	mov	$0xFFFFFFFF, %edi
	rol	$6, %eax
	xor	%ecx, %edi
	add	%ebx, %eax
	//53
	or	%eax, %edi
	lea	0x8F0CCC92(%edx, %ebp, 1), %edx
	xor	%ebx, %edi
	mov	40(%esi), %ebp
	add	%edi, %edx
	mov	$0xFFFFFFFF, %edi
	rol	$10, %edx
	xor	%ebx, %edi
	add	%eax, %edx
	//54
	or	%edx, %edi
	lea	0xFFEFF47D(%ecx, %ebp, 1), %ecx
	xor	%eax, %edi
	mov	4(%esi), %ebp
	add	%edi, %ecx
	mov	$0xFFFFFFFF, %edi
	rol	$15, %ecx
	xor	%eax, %edi
	add	%edx, %ecx
	//55
	or	%ecx, %edi
	lea	0x85845DD1(%ebx, %ebp, 1), %ebx
	xor	%edx, %edi
	mov	32(%esi), %ebp
	add	%edi, %ebx
	mov	$0xFFFFFFFF, %edi
	rol	$21, %ebx
	xor	%edx, %edi
	add	%ecx, %ebx
	//56
	or	%ebx, %edi
	lea	0x6FA87E4F(%eax, %ebp, 1), %eax
	xor	%ecx, %edi
	mov	60(%esi), %ebp
	add	%edi, %eax
	mov	$0xFFFFFFFF, %edi
	rol	$6, %eax
	xor	%ecx, %edi
	add	%ebx, %eax
	//57
	or	%eax, %edi
	lea	0xFE2CE6E0(%edx, %ebp, 1), %edx
	xor	%ebx, %edi
	mov	24(%esi), %ebp
	add	%edi, %edx
	mov	$0xFFFFFFFF, %edi
	rol	$10, %edx
	xor	%ebx, %edi
	add	%eax, %edx
	//58
	or	%edx, %edi
	lea	0xA3014314(%ecx, %ebp, 1), %ecx
	xor	%eax, %edi
	mov	52(%esi), %ebp
	add	%edi, %ecx
	mov	$0xFFFFFFFF, %edi
	rol	$15, %ecx
	xor	%eax, %edi
	add	%edx, %ecx
	//59
	or	%ecx, %edi
	lea	0x4E0811A1(%ebx, %ebp, 1), %ebx
	xor	%edx, %edi
	mov	16(%esi), %ebp
	add	%edi, %ebx
	mov	$0xFFFFFFFF, %edi
	rol	$21, %ebx
	xor	%edx, %edi
	add	%ecx, %ebx
	//60
	or	%ebx, %edi
	lea	0xF7537E82(%eax, %ebp, 1), %eax
	xor	%ecx, %edi
	mov	44(%esi), %ebp
	add	%edi, %eax
	mov	$0xFFFFFFFF, %edi
	rol	$6, %eax
	xor	%ecx, %edi
	add	%ebx, %eax
	//61
	or	%eax, %edi
	lea	0xBD3AF235(%edx, %ebp, 1), %edx
	xor	%ebx, %edi
	mov	8(%esi), %ebp
	add	%edi, %edx
	mov	$0xFFFFFFFF, %edi
	rol	$10, %edx
	xor	%ebx, %edi
	add	%eax, %edx
	//62
	or	%edx, %edi
	lea	0x2AD7D2BB(%ecx, %ebp, 1), %ecx
	xor	%eax, %edi
	mov	36(%esi), %ebp
	add	%edi, %ecx
	mov	$0xFFFFFFFF, %edi
	rol	$15, %ecx
	xor	%eax, %edi
	add	%edx, %ecx
	//63
	or	%ecx, %edi
	lea	0xEB86D391(%ebx, %ebp, 1), %ebx
	xor	%edx, %edi
	add	%edi, %ebx
	rol	$21, %ebx
	add	%ecx, %ebx
	.endm

#extern "C" void md5_init_context( unsigned char context[88] );
#extern "C" void md5_update( unsigned char context[88], const void *input, unsigned int inputlen );
#extern "C" void md5_final( unsigned char context[88] );
#extern "C" void md5_digest( unsigned char digest[16], const void *input, unsigned int inputlen );

.global md5_init_context, md5_update, md5_final, md5_digest
	.type	md5_init_context, @function
	.type	md5_update, @function
	.type	md5_final, @function
	.type	md5_digest, @function

	.align	2
md5_init_context:
.md5_init_context:
	mov	4(%esp), %edx
	xor	%eax, %eax
	movl	$0x67452301, (%edx)
	movl	$0xEFCDAB89, 4(%edx)
	movl	$0x98BADCFE, 8(%edx)
	movl	$0x10325476, 12(%edx)
	mov	%eax, 16(%edx)
	mov	%eax, 20(%edx)
	ret

	.align	2
md5_digest:
	add	$-88, %esp
	push	%esp
	call	.md5_init_context
	add	$4, %esp
	mov	%esp, %ecx
	push	100(%ecx)
	push	96(%ecx)
	push	%ecx
	call	.md5_update
	add	$12, %esp
	push	%esp
	call	.md5_final
	add	$4, %esp
	mov	92(%esp), %ecx
	mov	(%esp), %eax
	mov	4(%esp), %edx
	mov	%eax, (%ecx)
	mov	%edx, 4(%ecx)
	mov	8(%esp), %eax
	mov	12(%esp), %edx
	mov	%eax, 8(%ecx)
	mov	%edx, 12(%ecx)
	add	$88, %esp
	ret

	.align	2
md5_update:
.md5_update:
	add	$-16, %esp
	mov	%ebx, (%esp)
	mov	%esi, 4(%esp)
	mov	%edi, 8(%esp)
	mov	%ebp, 12(%esp)
	mov	20(%esp), %ebx
	mov	24(%esp), %esi
	mov	28(%esp), %ebp
	mov	16(%ebx), %eax
	add	%ebp, 16(%ebx)
	adc	$0, 20(%ebx)
	and	$0x3F, %eax
	mov	$64, %ecx
	jz	.Lhas_no_remain
	sub	%eax, %ecx
	cmp	%ecx, %ebp
	lea	24(%ebx, %eax), %edi
	jae	.Lneed_transform
	mov	%ebp, %ecx
	and	$3, %ebp
	shr	$2, %ecx
	cld
	rep	movsl
	mov	%ebp, %ecx
	rep	movsb
	mov	12(%esp), %ebp
	mov	8(%esp), %edi
	mov	4(%esp), %esi
	mov	(%esp), %ebx
	add	$16, %esp
	ret
.Lneed_transform:
	sub	%ecx, %ebp
	mov	%ecx, %eax
	shr	$2, %ecx
	and	$3, %eax
	cld
	rep	movsl
	mov	%eax, %ecx
	rep	movsb
	push	%ebx
	call	md5_transform
.Lhas_no_remain:
	mov	%ebp, %ecx
	mov	%ebp, %eax
	and	$0x3F, %ecx
	jz	.Lneed_not_copy_remain
	mov	%esi, %edx
	and	$0xFFFFFFC0, %eax
	add	%eax, %esi
	mov	%ecx, %eax
	shr	$2, %ecx
	and	$3, %eax
	lea	24(%ebx), %edi
	cld
	rep	movsl
	mov	%eax, %ecx
	rep	movsb
	mov	%edx, %esi
.Lneed_not_copy_remain:
	shr	$6, %ebp
	jz	.Lfinish
	mov	%ebx, %edi
	mov	%ebx, 24(%esp)
	mov	(%edi),	  %eax
	mov	4(%edi),  %ebx
	mov	8(%edi),  %ecx
	mov	12(%edi), %edx
.Lupdate_next:
	mov	%ebp, 28(%esp)
	MD5_ROUND64
	mov	20(%esp), %edi
	mov	28(%esp), %ebp
	add	$64, %esi
	add	(%edi), %eax
	add	4(%edi), %ebx
	add	8(%edi), %ecx
	add	12(%edi), %edx
	add	$-1, %ebp
	mov	%eax, (%edi)
	mov	%ebx, 4(%edi)
	mov	%ecx, 8(%edi)
	mov	%edx, 12(%edi)
	jnz	.Lupdate_next
.Lfinish:
	mov	12(%esp), %ebp
	mov	8(%esp), %edi
	mov	4(%esp), %esi
	mov	(%esp), %ebx
	add	$16, %esp
	ret

	.align	2
md5_final:
.md5_final:
	add	$-16, %esp
	mov	%ebx, (%esp)
	mov	%esi, 4(%esp)
	mov	%edi, 8(%esp)
	mov	%ebp, 12(%esp)
	mov	20(%esp), %ebx
	mov	16(%ebx), %ecx
	and	$0x3F, %ecx
	movb	$0x80, 24(%ebx, %ecx)
	cmp	$56, %ecx
	lea	25(%ebx, %ecx), %edi
	jb	.Lpadding_len
	neg	%ecx
	add	$63, %ecx
	xor	%eax, %eax
	mov	%ecx, %edx	
	shr	$2, %ecx
	and	$3, %edx
	cld
	rep	stosl
	mov	%edx, %ecx
	rep	stosb
	push	%ebx
	call	md5_transform
	lea	24(%ebx), %edi
	mov	$-1, %ecx
.Lpadding_len:
	neg	%ecx
	add	$55, %ecx
	xor	%eax, %eax
	mov	%ecx, %edx
	shr	$2, %ecx
	and	$3, %edx
	cld
	rep	stosl
	mov	%edx, %ecx
	rep	stosb
	mov	16(%ebx), %eax
	mov	20(%ebx), %edx
	shld	$3, %eax, %edx
	shl	$3, %eax
	mov	%eax, (%edi)
	mov	%edx, 4(%edi)
	push	%ebx
	call	md5_transform
	mov	12(%esp), %ebp
	mov	8(%esp), %edi
	mov	4(%esp), %esi
	mov	(%esp), %ebx
	add	$16, %esp
	ret

	.align	2
md5_transform:
	add	$-16, %esp
	mov	%ebx, (%esp)
	mov	%esi, 4(%esp)
	mov	%edi, 8(%esp)
	mov	%ebp, 12(%esp)
	mov	20(%esp), %edi
	mov	(%edi),	  %eax
	mov	4(%edi),  %ebx
	mov	8(%edi),  %ecx
	mov	12(%edi), %edx
	lea	24(%edi), %esi
	MD5_ROUND64
	mov	20(%esp), %edi
	add	%eax, (%edi)
	add	%ebx, 4(%edi)
	add	%ecx, 8(%edi)
	add	%edx, 12(%edi)
	mov	(%esp), %ebx
	mov	4(%esp), %esi
	mov	8(%esp), %edi
	mov	12(%esp), %ebp
	add	$16, %esp
	ret	$4

