	.text
	.macro	SHA1_ROUND80	source
	//0
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	and	%ebx, %r10d
	mov	(\source), %esi
	xor	%edx, %r10d
	rol	$30, %ebx
	bswap	%esi
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	mov	%esi, (%rsp)
	add	%esi, %r11d
	//1
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	and	%eax, %r10d
	mov	4(\source), %edx
	xor	%ecx, %r10d
	bswap	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 4(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//2
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	and	%esi, %r10d
	mov	8(\source), %ecx
	xor	%ebx, %r10d
	rol	$30, %esi
	bswap	%ecx
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	mov	%ecx, 8(%rsp)
	add	%ecx, %r11d
	//3
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	and	%edx, %r10d
	mov	12(\source), %ebx
	xor	%eax, %r10d
	bswap	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 12(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//4
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	and	%ecx, %r10d
	mov	16(\source), %eax
	xor	%esi, %r10d
	rol	$30, %ecx
	bswap	%eax
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	mov	%eax, 16(%rsp)
	add	%eax, %r11d
	//5
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	and	%ebx, %r10d
	mov	20(\source), %esi
	xor	%edx, %r10d
	bswap	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 20(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//6
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	and	%eax, %r10d
	mov	24(\source), %edx
	xor	%ecx, %r10d
	rol	$30, %eax
	bswap	%edx
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	mov	%edx, 24(%rsp)
	add	%edx, %r11d
	//7
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	and	%esi, %r10d
	mov	28(\source), %ecx
	xor	%ebx, %r10d
	bswap	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 28(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//8
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	and	%edx, %r10d
	mov	32(\source), %ebx
	xor	%eax, %r10d
	rol	$30, %edx
	bswap	%ebx
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	mov	%ebx, 32(%rsp)
	add	%ebx, %r11d
	//9
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	and	%ecx, %r10d
	mov	36(\source), %eax
	xor	%esi, %r10d
	bswap	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 36(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//10
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	and	%ebx, %r10d
	mov	40(\source), %esi
	xor	%edx, %r10d
	rol	$30, %ebx
	bswap	%esi
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	mov	%esi, 40(%rsp)
	add	%esi, %r11d
	//11
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	and	%eax, %r10d
	mov	44(\source), %edx
	xor	%ecx, %r10d
	bswap	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 44(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//12
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	and	%esi, %r10d
	mov	48(\source), %ecx
	xor	%ebx, %r10d
	rol	$30, %esi
	bswap	%ecx
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	mov	%ecx, 48(%rsp)
	add	%ecx, %r11d
	//13
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	and	%edx, %r10d
	mov	52(\source), %ebx
	xor	%eax, %r10d
	bswap	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 52(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//14
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	and	%ecx, %r10d
	mov	56(\source), %eax
	xor	%esi, %r10d
	rol	$30, %ecx
	bswap	%eax
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	mov	%eax, 56(%rsp)
	add	%eax, %r11d
	//15
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	and	%ebx, %r10d
	mov	60(\source), %esi
	xor	%edx, %r10d
	bswap	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 60(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//16
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	mov	(%rsp), %edx
	xor	8(%rsp), %edx
	and	%eax, %r10d
	xor	32(%rsp), %edx
	xor	%ecx, %r10d
	xor	52(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, (%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//17
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	mov	4(%rsp), %ecx
	xor	12(%rsp), %ecx
	and	%esi, %r10d
	xor	36(%rsp), %ecx
	xor	%ebx, %r10d
	xor	56(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 4(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//18
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	mov	8(%rsp), %ebx
	xor	16(%rsp), %ebx
	and	%edx, %r10d
	xor	40(%rsp), %ebx
	xor	%eax, %r10d
	xor	60(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 8(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//19
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	mov	(%rsp), %eax
	xor	12(%rsp), %eax
	and	%ecx, %r10d
	xor	20(%rsp), %eax
	xor	%esi, %r10d
	xor	44(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 12(%rsp)
	lea	0x5A827999(%r10d, %r11d, 1), %r11d
	//20
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	mov	4(%rsp), %esi
	xor	16(%rsp), %esi
	xor	%ebx, %r10d
	xor	24(%rsp), %esi
	xor	48(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 16(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//21
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	mov	8(%rsp), %edx
	xor	20(%rsp), %edx
	xor	%eax, %r10d
	xor	28(%rsp), %edx
	xor	52(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 20(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//22
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	mov	12(%rsp), %ecx
	xor	24(%rsp), %ecx
	xor	%esi, %r10d
	xor	32(%rsp), %ecx
	xor	56(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 24(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//23
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	mov	16(%rsp), %ebx
	xor	28(%rsp), %ebx
	xor	%edx, %r10d
	xor	36(%rsp), %ebx
	xor	60(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 28(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//24
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	mov	(%rsp), %eax
	xor	20(%rsp), %eax
	xor	%ecx, %r10d
	xor	32(%rsp), %eax
	xor	40(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 32(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//25
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	mov	4(%rsp), %esi
	xor	24(%rsp), %esi
	xor	%ebx, %r10d
	xor	36(%rsp), %esi
	xor	44(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 36(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//26
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	mov	8(%rsp), %edx
	xor	28(%rsp), %edx
	xor	%eax, %r10d
	xor	40(%rsp), %edx
	xor	48(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 40(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//27
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	mov	12(%rsp), %ecx
	xor	32(%rsp), %ecx
	xor	%esi, %r10d
	xor	44(%rsp), %ecx
	xor	52(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 44(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//28
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	mov	16(%rsp), %ebx
	xor	36(%rsp), %ebx
	xor	%edx, %r10d
	xor	48(%rsp), %ebx
	xor	56(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 48(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//29
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	mov	20(%rsp), %eax
	xor	40(%rsp), %eax
	xor	%ecx, %r10d
	xor	52(%rsp), %eax
	xor	60(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 52(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//30
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	mov	(%rsp), %esi
	xor	24(%rsp), %esi
	xor	%ebx, %r10d
	xor	44(%rsp), %esi
	xor	56(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 56(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//31
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	mov	4(%rsp), %edx
	xor	28(%rsp), %edx
	xor	%eax, %r10d
	xor	48(%rsp), %edx
	xor	60(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 60(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//32
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	mov	(%rsp), %ecx
	xor	8(%rsp), %ecx
	xor	%esi, %r10d
	xor	32(%rsp), %ecx
	xor	52(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, (%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//33
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	mov	4(%rsp), %ebx
	xor	12(%rsp), %ebx
	xor	%edx, %r10d
	xor	36(%rsp), %ebx
	xor	56(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 4(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//34
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	mov	8(%rsp), %eax
	xor	16(%rsp), %eax
	xor	%ecx, %r10d
	xor	40(%rsp), %eax
	xor	60(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 8(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//35
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	mov	(%rsp), %esi
	xor	12(%rsp), %esi
	xor	%ebx, %r10d
	xor	20(%rsp), %esi
	xor	44(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 12(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//36
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	mov	4(%rsp), %edx
	xor	16(%rsp), %edx
	xor	%eax, %r10d
	xor	24(%rsp), %edx
	xor	48(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 16(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//37
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	mov	8(%rsp), %ecx
	xor	20(%rsp), %ecx
	xor	%esi, %r10d
	xor	28(%rsp), %ecx
	xor	52(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 20(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//38
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	mov	12(%rsp), %ebx
	xor	24(%rsp), %ebx
	xor	%edx, %r10d
	xor	32(%rsp), %ebx
	xor	56(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 24(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//39
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	mov	16(%rsp), %eax
	xor	28(%rsp), %eax
	xor	%ecx, %r10d
	xor	36(%rsp), %eax
	xor	60(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 28(%rsp)
	lea	0x6ED9EBA1(%r10d, %r11d, 1), %r11d
	//40
	mov	%ecx, %edi
	mov	%r11d, %eax
	rol	$5, %r11d
	mov	%ecx, %r10d
	and	%ebx, %edi
	add	%esi, %r11d
	mov	(%rsp), %esi
	or	%ebx, %r10d
	xor	20(%rsp), %esi
	and	%edx, %r10d
	xor	32(%rsp), %esi
	or	%edi, %r10d
	xor	40(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 32(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//41
	mov	%ebx, %edi
	mov	%r11d, %esi
	rol	$5, %r11d
	mov	%ebx, %r10d
	and	%eax, %edi
	add	%edx, %r11d
	mov	4(%rsp), %edx
	or	%eax, %r10d
	xor	24(%rsp), %edx
	and	%ecx, %r10d
	xor	36(%rsp), %edx
	or	%edi, %r10d
	xor	44(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 36(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//42
	mov	%eax, %edi
	mov	%r11d, %edx
	rol	$5, %r11d
	mov	%eax, %r10d
	and	%esi, %edi
	add	%ecx, %r11d
	mov	8(%rsp), %ecx
	or	%esi, %r10d
	xor	28(%rsp), %ecx
	and	%ebx, %r10d
	xor	40(%rsp), %ecx
	or	%edi, %r10d
	xor	48(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 40(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//43
	mov	%esi, %edi
	mov	%r11d, %ecx
	rol	$5, %r11d
	mov	%esi, %r10d
	and	%edx, %edi
	add	%ebx, %r11d
	mov	12(%rsp), %ebx
	or	%edx, %r10d
	xor	32(%rsp), %ebx
	and	%eax, %r10d
	xor	44(%rsp), %ebx
	or	%edi, %r10d
	xor	52(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 44(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//44
	mov	%edx, %edi
	mov	%r11d, %ebx
	rol	$5, %r11d
	mov	%edx, %r10d
	and	%ecx, %edi
	add	%eax, %r11d
	mov	16(%rsp), %eax
	or	%ecx, %r10d
	xor	36(%rsp), %eax
	and	%esi, %r10d
	xor	48(%rsp), %eax
	or	%edi, %r10d
	xor	56(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 48(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//45
	mov	%ecx, %edi
	mov	%r11d, %eax
	rol	$5, %r11d
	mov	%ecx, %r10d
	and	%ebx, %edi
	add	%esi, %r11d
	mov	20(%rsp), %esi
	or	%ebx, %r10d
	xor	40(%rsp), %esi
	and	%edx, %r10d
	xor	52(%rsp), %esi
	or	%edi, %r10d
	xor	60(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 52(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//46
	mov	%ebx, %edi
	mov	%r11d, %esi
	rol	$5, %r11d
	mov	%ebx, %r10d
	and	%eax, %edi
	add	%edx, %r11d
	mov	(%rsp), %edx
	or	%eax, %r10d
	xor	24(%rsp), %edx
	and	%ecx, %r10d
	xor	44(%rsp), %edx
	or	%edi, %r10d
	xor	56(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 56(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//47
	mov	%eax, %edi
	mov	%r11d, %edx
	rol	$5, %r11d
	mov	%eax, %r10d
	and	%esi, %edi
	add	%ecx, %r11d
	mov	4(%rsp), %ecx
	or	%esi, %r10d
	xor	28(%rsp), %ecx
	and	%ebx, %r10d
	xor	48(%rsp), %ecx
	or	%edi, %r10d
	xor	60(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 60(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//48
	mov	%esi, %edi
	mov	%r11d, %ecx
	rol	$5, %r11d
	mov	%esi, %r10d
	and	%edx, %edi
	add	%ebx, %r11d
	mov	(%rsp), %ebx
	or	%edx, %r10d
	xor	8(%rsp), %ebx
	and	%eax, %r10d
	xor	32(%rsp), %ebx
	or	%edi, %r10d
	xor	52(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, (%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//49
	mov	%edx, %edi
	mov	%r11d, %ebx
	rol	$5, %r11d
	mov	%edx, %r10d
	and	%ecx, %edi
	add	%eax, %r11d
	mov	4(%rsp), %eax
	or	%ecx, %r10d
	xor	12(%rsp), %eax
	and	%esi, %r10d
	xor	36(%rsp), %eax
	or	%edi, %r10d
	xor	56(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 4(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//50
	mov	%ecx, %edi
	mov	%r11d, %eax
	rol	$5, %r11d
	mov	%ecx, %r10d
	and	%ebx, %edi
	add	%esi, %r11d
	mov	8(%rsp), %esi
	or	%ebx, %r10d
	xor	16(%rsp), %esi
	and	%edx, %r10d
	xor	40(%rsp), %esi
	or	%edi, %r10d
	xor	60(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 8(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//51
	mov	%ebx, %edi
	mov	%r11d, %esi
	rol	$5, %r11d
	mov	%ebx, %r10d
	and	%eax, %edi
	add	%edx, %r11d
	mov	(%rsp), %edx
	or	%eax, %r10d
	xor	12(%rsp), %edx
	and	%ecx, %r10d
	xor	20(%rsp), %edx
	or	%edi, %r10d
	xor	44(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 12(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//52
	mov	%eax, %edi
	mov	%r11d, %edx
	rol	$5, %r11d
	mov	%eax, %r10d
	and	%esi, %edi
	add	%ecx, %r11d
	mov	4(%rsp), %ecx
	or	%esi, %r10d
	xor	16(%rsp), %ecx
	and	%ebx, %r10d
	xor	24(%rsp), %ecx
	or	%edi, %r10d
	xor	48(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 16(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//53
	mov	%esi, %edi
	mov	%r11d, %ecx
	rol	$5, %r11d
	mov	%esi, %r10d
	and	%edx, %edi
	add	%ebx, %r11d
	mov	8(%rsp), %ebx
	or	%edx, %r10d
	xor	20(%rsp), %ebx
	and	%eax, %r10d
	xor	28(%rsp), %ebx
	or	%edi, %r10d
	xor	52(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 20(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//54
	mov	%edx, %edi
	mov	%r11d, %ebx
	rol	$5, %r11d
	mov	%edx, %r10d
	and	%ecx, %edi
	add	%eax, %r11d
	mov	12(%rsp), %eax
	or	%ecx, %r10d
	xor	24(%rsp), %eax
	and	%esi, %r10d
	xor	32(%rsp), %eax
	or	%edi, %r10d
	xor	56(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 24(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//55
	mov	%ecx, %edi
	mov	%r11d, %eax
	rol	$5, %r11d
	mov	%ecx, %r10d
	and	%ebx, %edi
	add	%esi, %r11d
	mov	16(%rsp), %esi
	or	%ebx, %r10d
	xor	28(%rsp), %esi
	and	%edx, %r10d
	xor	36(%rsp), %esi
	or	%edi, %r10d
	xor	60(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 28(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//56
	mov	%ebx, %edi
	mov	%r11d, %esi
	rol	$5, %r11d
	mov	%ebx, %r10d
	and	%eax, %edi
	add	%edx, %r11d
	mov	(%rsp), %edx
	or	%eax, %r10d
	xor	20(%rsp), %edx
	and	%ecx, %r10d
	xor	32(%rsp), %edx
	or	%edi, %r10d
	xor	40(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 32(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//57
	mov	%eax, %edi
	mov	%r11d, %edx
	rol	$5, %r11d
	mov	%eax, %r10d
	and	%esi, %edi
	add	%ecx, %r11d
	mov	4(%rsp), %ecx
	or	%esi, %r10d
	xor	24(%rsp), %ecx
	and	%ebx, %r10d
	xor	36(%rsp), %ecx
	or	%edi, %r10d
	xor	44(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 36(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//58
	mov	%esi, %edi
	mov	%r11d, %ecx
	rol	$5, %r11d
	mov	%esi, %r10d
	and	%edx, %edi
	add	%ebx, %r11d
	mov	8(%rsp), %ebx
	or	%edx, %r10d
	xor	28(%rsp), %ebx
	and	%eax, %r10d
	xor	40(%rsp), %ebx
	or	%edi, %r10d
	xor	48(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 40(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//59
	mov	%edx, %edi
	mov	%r11d, %ebx
	rol	$5, %r11d
	mov	%edx, %r10d
	and	%ecx, %edi
	add	%eax, %r11d
	mov	12(%rsp), %eax
	or	%ecx, %r10d
	xor	32(%rsp), %eax
	and	%esi, %r10d
	xor	44(%rsp), %eax
	or	%edi, %r10d
	xor	52(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 44(%rsp)
	lea	0x8F1BBCDC(%r10d, %r11d, 1), %r11d
	//60
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	mov	16(%rsp), %esi
	xor	36(%rsp), %esi
	xor	%ebx, %r10d
	xor	48(%rsp), %esi
	xor	56(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 48(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//61
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	mov	20(%rsp), %edx
	xor	40(%rsp), %edx
	xor	%eax, %r10d
	xor	52(%rsp), %edx
	xor	60(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 52(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//62
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	mov	(%rsp), %ecx
	xor	24(%rsp), %ecx
	xor	%esi, %r10d
	xor	44(%rsp), %ecx
	xor	56(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 56(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//63
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	mov	4(%rsp), %ebx
	xor	28(%rsp), %ebx
	xor	%edx, %r10d
	xor	48(%rsp), %ebx
	xor	60(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 60(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//64
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	mov	(%rsp), %eax
	xor	8(%rsp), %eax
	xor	%ecx, %r10d
	xor	32(%rsp), %eax
	xor	52(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, (%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//65
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	mov	4(%rsp), %esi
	xor	12(%rsp), %esi
	xor	%ebx, %r10d
	xor	36(%rsp), %esi
	xor	56(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 4(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//66
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	mov	8(%rsp), %edx
	xor	16(%rsp), %edx
	xor	%eax, %r10d
	xor	40(%rsp), %edx
	xor	60(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 8(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//67
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	mov	(%rsp), %ecx
	xor	12(%rsp), %ecx
	xor	%esi, %r10d
	xor	20(%rsp), %ecx
	xor	44(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 12(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//68
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	mov	4(%rsp), %ebx
	xor	16(%rsp), %ebx
	xor	%edx, %r10d
	xor	24(%rsp), %ebx
	xor	48(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 16(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//69
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	mov	8(%rsp), %eax
	xor	20(%rsp), %eax
	xor	%ecx, %r10d
	xor	28(%rsp), %eax
	xor	52(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 20(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//70
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	mov	12(%rsp), %esi
	xor	24(%rsp), %esi
	xor	%ebx, %r10d
	xor	32(%rsp), %esi
	xor	56(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 24(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//71
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	mov	16(%rsp), %edx
	xor	28(%rsp), %edx
	xor	%eax, %r10d
	xor	36(%rsp), %edx
	xor	60(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 28(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//72
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	mov	(%rsp), %ecx
	xor	20(%rsp), %ecx
	xor	%esi, %r10d
	xor	32(%rsp), %ecx
	xor	40(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	mov	%ecx, 32(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//73
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	mov	4(%rsp), %ebx
	xor	24(%rsp), %ebx
	xor	%edx, %r10d
	xor	36(%rsp), %ebx
	xor	44(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	mov	%ebx, 36(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//74
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	mov	8(%rsp), %eax
	xor	28(%rsp), %eax
	xor	%ecx, %r10d
	xor	40(%rsp), %eax
	xor	48(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	mov	%eax, 40(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//75
	mov	%ecx, %r10d
	mov	%r11d, %eax
	rol	$5, %r11d
	xor	%edx, %r10d
	add	%esi, %r11d
	mov	12(%rsp), %esi
	xor	32(%rsp), %esi
	xor	%ebx, %r10d
	xor	44(%rsp), %esi
	xor	52(%rsp), %esi
	rol	%esi
	rol	$30, %ebx
	add	%esi, %r10d
	mov	%esi, 44(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//76
	mov	%ebx, %r10d
	mov	%r11d, %esi
	rol	$5, %r11d
	xor	%ecx, %r10d
	add	%edx, %r11d
	mov	16(%rsp), %edx
	xor	36(%rsp), %edx
	xor	%eax, %r10d
	xor	48(%rsp), %edx
	xor	56(%rsp), %edx
	rol	%edx
	rol	$30, %eax
	add	%edx, %r10d
	mov	%edx, 48(%rsp)
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//77
	mov	%eax, %r10d
	mov	%r11d, %edx
	rol	$5, %r11d
	xor	%ebx, %r10d
	add	%ecx, %r11d
	mov	20(%rsp), %ecx
	xor	40(%rsp), %ecx
	xor	%esi, %r10d
	xor	52(%rsp), %ecx
	xor	60(%rsp), %ecx
	rol	%ecx
	rol	$30, %esi
	add	%ecx, %r10d
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//78
	mov	%esi, %r10d
	mov	%r11d, %ecx
	rol	$5, %r11d
	xor	%eax, %r10d
	add	%ebx, %r11d
	mov	(%rsp), %ebx
	xor	24(%rsp), %ebx
	xor	%edx, %r10d
	xor	44(%rsp), %ebx
	xor	56(%rsp), %ebx
	rol	%ebx
	rol	$30, %edx
	add	%ebx, %r10d
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	//79
	mov	%edx, %r10d
	mov	%r11d, %ebx
	rol	$5, %r11d
	xor	%esi, %r10d
	add	%eax, %r11d
	mov	4(%rsp), %eax
	xor	28(%rsp), %eax
	xor	%ecx, %r10d
	xor	48(%rsp), %eax
	xor	60(%rsp), %eax
	rol	%eax
	rol	$30, %ecx
	add	%eax, %r10d
	lea	0xCA62C1D6(%r10d, %r11d, 1), %r11d
	.endm

#extern "C" void sha1_init_context( unsigned char context[92] );
#extern "C" void sha1_update( unsigned char context[92], const void *input, unsigned int inputlen );
#extern "C" void sha1_final( unsigned char context[92] );
#extern "C" void sha1_digest( unsigned char digest[20], const void *input, unsigned int inputlen );
.global sha1_init_context, sha1_update, sha1_final, sha1_digest

	.align	2
sha1_init_context:
.Lsha1_init_context:
	movl	$0x67452301, (%rdi)
	movl	$0xEFCDAB89, 4(%rdi)
	movl	$0x98BADCFE, 8(%rdi)
	movl	$0x10325476, 12(%rdi)
	movl	$0xC3D2E1F0, 16(%rdi)
	movq	$0, 20(%rdi)
	ret

	.align	2
sha1_digest:
	add	$-92, %rsp
	push	%rdi
	lea	8(%rsp), %rdi
	call	.Lsha1_init_context
	call	.Lsha1_update
	lea	8(%rsp), %rdi
	call	.Lsha1_final
	pop	%rdi
	mov	(%rsp), %rax
	mov	8(%rsp), %rcx
	mov	16(%rsp), %edx
	mov	%rax, (%rdi)
	mov	%rcx, 8(%rdi)
	mov	%edx, 16(%rdi)
	add	$92, %rsp
	ret
	
	.align	2
sha1_update:
.Lsha1_update:
	push	%rbx
	push	%r12
	mov	%rdx, %r8
	mov	%rdi, %r9
	mov	%rsi, %r12
	add	$-64, %rsp
	mov	20(%rdi), %rax
	add	%rdx, 20(%rdi)
	and	$0x3F, %rax
	mov	$64, %rcx
	jz	.Lhas_no_remain
	sub	%rax, %rcx
	cmp	%rcx, %r8
	lea	28(%r9, %rax), %rdi
	jae	.Lneed_transform
	mov	%r8, %rcx
	and	$7, %r8
	shr	$3, %rcx
	cld
	rep	movsq
	mov	%r8, %rcx
	rep	movsb
	add	$64, %rsp
	pop	%r12
	pop	%rbx
	ret
.Lneed_transform:
	sub	%rcx, %r8
	mov	%rcx, %rax
	shr	$3, %rcx
	and	$7, %rax
	cld
	rep	movsq
	mov	%rax, %rcx
	rep	movsb
	mov	%rsi, %r12
	add	$-64, %rsp
	mov	(%r9),	 %r11d
	mov	4(%r9),  %ebx
	mov	8(%r9),  %ecx
	mov	12(%r9), %edx
	mov	16(%r9), %esi
	lea	28(%r9), %rdi
	SHA1_ROUND80	%rdi
	add	%r11d, (%r9)
	add	%ebx, 4(%r9)
	add	%ecx, 8(%r9)
	add	%edx, 12(%r9)
	add	%esi, 16(%r9)
	add	$64, %rsp
.Lhas_no_remain:
	mov	%r8, %rcx
	mov	%r8, %rax
	and	$0x3F, %rcx	
	jz	.Lneed_not_copy_remain
	and	$0xFFFFFFFFFFFFFFC0, %rax
	lea	(%r12, %rax), %rsi
	mov	%rcx, %rax
	shr	$3, %rcx
	and	$7, %rax
	lea	28(%r9), %rdi
	cld
	rep	movsq
	mov	%rax, %rcx
	rep	movsb
.Lneed_not_copy_remain:
	shr	$6, %r8
	jz	.Lfinish
	mov	(%r9),   %r11d
	mov	4(%r9),  %ebx
	mov	8(%r9),  %ecx
	mov	12(%r9), %edx
	mov	16(%r9), %esi
.Lupdate_next:
	SHA1_ROUND80	%r12
	add	(%r9),   %r11d
	add	4(%r9),  %ebx
	add	8(%r9),  %ecx
	add	12(%r9), %edx
	add	16(%r9), %esi
	add	$64, %r12
	add	$-1, %r8
	mov	%r11d, (%r9)
	mov	%ebx, 4(%r9)
	mov	%ecx, 8(%r9)
	mov	%edx, 12(%r9)
	mov	%esi, 16(%r9)
	jnz	.Lupdate_next
.Lfinish:
	add	$64, %rsp
	pop	%r12
	pop	%rbx
	ret

	.align	2
sha1_final:
.Lsha1_final:
	push	%rbx
	add	$-64, %rsp
	mov	%rdi, %r9
	lea	28(%rdi), %r8
	mov	20(%rdi), %rcx
	and	$0x3F, %rcx
	movb	$0x80, 28(%r9, %rcx)	
	cmp	$56, %rcx
	lea	29(%r9, %rcx), %rdi
	jb	.Lpadding_len
	neg	%rcx
	add	$63, %rcx
	xor	%rax, %rax
	mov	%rcx, %rdx
	shr	$3, %rcx
	and	$7, %rdx
	cld
	rep	stosq
	mov	%rdx, %rcx
	rep	stosb
	mov	(%r9),	 %r11d
	mov	4(%r9),  %ebx
	mov	8(%r9),  %ecx
	mov	12(%r9), %edx
	mov	16(%r9), %esi
	SHA1_ROUND80	%r8
	add	%r11d, (%r9)
	add	%ebx, 4(%r9)
	add	%ecx, 8(%r9)
	add	%edx, 12(%r9)
	add	%esi, 16(%r9)
	mov	%r8, %rdi
	mov	$-1, %rcx
.Lpadding_len:
	neg	%rcx
	add	$55, %rcx
	xor	%rax, %rax
	mov	%rcx, %rdx
	shr	$3, %rcx
	and	$7, %rdx
	cld
	rep	stosq
	mov	%rdx, %rcx
	rep	stosb
	mov	20(%r9), %rax
	shl	$3, %rax
	bswap	%rax
	mov	%rax, (%rdi)
	mov	(%r9),	 %r11d
	mov	4(%r9),  %ebx
	mov	8(%r9),  %ecx
	mov	12(%r9), %edx
	mov	16(%r9), %esi
	SHA1_ROUND80	%r8
	add	(%r9),   %r11d
	add	4(%r9),  %ebx
	add	8(%r9),  %ecx
	add	12(%r9), %edx
	add	16(%r9), %esi
	bswap	%r11d
	bswap	%ebx
	bswap	%ecx
	bswap	%edx
	bswap	%esi
	mov	%r11d, (%r9)
	mov	%ebx, 4(%r9)
	mov	%ecx, 8(%r9)
	mov	%edx, 12(%r9)
	mov	%esi, 16(%r9)
	add	$64, %rsp
	pop	%rbx
	ret



