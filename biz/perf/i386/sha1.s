	.text
	.macro	SHA1_ROUND80
	//0
	movd	%eax, %mm0
	mov	%ecx, %ebp
	rol	$5, %eax
	xor	%edx, %ecx
	and	%ebx, %ecx
	xor	%edx, %ecx
	add	%eax, %esi
	mov	(%edi), %eax
	rol	$30, %ebx
	bswap	%eax
	lea	0x5A827999(%ecx, %esi, 1), %esi
	mov	%eax, (%esp)
	add	%eax, %esi
	movd	%mm0, %eax
	mov	%ebp, %ecx
	//1
	movd	%esi, %mm0
	mov	%ebx, %ebp
	rol	$5, %esi
	xor	%ecx, %ebx
	and	%eax, %ebx
	xor	%ecx, %ebx
	add	%esi, %edx
	mov	4(%edi), %esi
	rol	$30, %eax
	bswap	%esi
	mov	%esi, 4(%esp)
	add	%esi, %ebx
	lea	0x5A827999(%ebx, %edx, 1), %edx
	movd	%mm0, %esi
	mov	%ebp, %ebx
	//2
	movd	%edx, %mm0
	mov	%eax, %ebp
	rol	$5, %edx
	xor	%ebx, %eax
	and	%esi, %eax
	xor	%ebx, %eax
	add	%edx, %ecx
	mov	8(%edi), %edx
	rol	$30, %esi
	bswap	%edx
	lea	0x5A827999(%eax, %ecx, 1), %ecx
	mov	%edx, 8(%esp)
	add	%edx, %ecx
	movd	%mm0, %edx
	mov	%ebp, %eax
	//3
	movd	%ecx, %mm0
	mov	%esi, %ebp
	rol	$5, %ecx
	xor	%eax, %esi
	and	%edx, %esi
	xor	%eax, %esi
	add	%ecx, %ebx
	mov	12(%edi), %ecx
	rol	$30, %edx
	bswap	%ecx
	mov	%ecx, 12(%esp)
	add	%ecx, %esi
	lea	0x5A827999(%esi, %ebx, 1), %ebx
	movd	%mm0, %ecx
	mov	%ebp, %esi
	//4
	movd	%ebx, %mm0
	mov	%edx, %ebp
	rol	$5, %ebx
	xor	%esi, %edx
	and	%ecx, %edx
	xor	%esi, %edx
	add	%ebx, %eax
	mov	16(%edi), %ebx
	rol	$30, %ecx
	bswap	%ebx
	lea	0x5A827999(%edx, %eax, 1), %eax
	mov	%ebx, 16(%esp)
	add	%ebx, %eax
	movd	%mm0, %ebx
	mov	%ebp, %edx
	//5
	movd	%eax, %mm0
	mov	%ecx, %ebp
	rol	$5, %eax
	xor	%edx, %ecx
	and	%ebx, %ecx
	xor	%edx, %ecx
	add	%eax, %esi
	mov	20(%edi), %eax
	rol	$30, %ebx
	bswap	%eax
	mov	%eax, 20(%esp)
	add	%eax, %ecx
	lea	0x5A827999(%ecx, %esi, 1), %esi
	movd	%mm0, %eax
	mov	%ebp, %ecx
	//6
	movd	%esi, %mm0
	mov	%ebx, %ebp
	rol	$5, %esi
	xor	%ecx, %ebx
	and	%eax, %ebx
	xor	%ecx, %ebx
	add	%esi, %edx
	mov	24(%edi), %esi
	rol	$30, %eax
	bswap	%esi
	lea	0x5A827999(%ebx, %edx, 1), %edx
	mov	%esi, 24(%esp)
	add	%esi, %edx
	movd	%mm0, %esi
	mov	%ebp, %ebx
	//7
	movd	%edx, %mm0
	mov	%eax, %ebp
	rol	$5, %edx
	xor	%ebx, %eax
	and	%esi, %eax
	xor	%ebx, %eax
	add	%edx, %ecx
	mov	28(%edi), %edx
	rol	$30, %esi
	bswap	%edx
	mov	%edx, 28(%esp)
	add	%edx, %eax
	lea	0x5A827999(%eax, %ecx, 1), %ecx
	movd	%mm0, %edx
	mov	%ebp, %eax
	//8
	movd	%ecx, %mm0
	mov	%esi, %ebp
	rol	$5, %ecx
	xor	%eax, %esi
	and	%edx, %esi
	xor	%eax, %esi
	add	%ecx, %ebx
	mov	32(%edi), %ecx
	rol	$30, %edx
	bswap	%ecx
	lea	0x5A827999(%esi, %ebx, 1), %ebx
	mov	%ecx, 32(%esp)
	add	%ecx, %ebx
	movd	%mm0, %ecx
	mov	%ebp, %esi
	//9
	movd	%ebx, %mm0
	mov	%edx, %ebp
	rol	$5, %ebx
	xor	%esi, %edx
	and	%ecx, %edx
	xor	%esi, %edx
	add	%ebx, %eax
	mov	36(%edi), %ebx
	rol	$30, %ecx
	bswap	%ebx
	mov	%ebx, 36(%esp)
	add	%ebx, %edx
	lea	0x5A827999(%edx, %eax, 1), %eax
	movd	%mm0, %ebx
	mov	%ebp, %edx
	//10
	movd	%eax, %mm0
	mov	%ecx, %ebp
	rol	$5, %eax
	xor	%edx, %ecx
	and	%ebx, %ecx
	xor	%edx, %ecx
	add	%eax, %esi
	mov	40(%edi), %eax
	rol	$30, %ebx
	bswap	%eax
	lea	0x5A827999(%ecx, %esi, 1), %esi
	mov	%eax, 40(%esp)
	add	%eax, %esi
	movd	%mm0, %eax
	mov	%ebp, %ecx
	//11
	movd	%esi, %mm0
	mov	%ebx, %ebp
	rol	$5, %esi
	xor	%ecx, %ebx
	and	%eax, %ebx
	xor	%ecx, %ebx
	add	%esi, %edx
	mov	44(%edi), %esi
	rol	$30, %eax
	bswap	%esi
	mov	%esi, 44(%esp)
	add	%esi, %ebx
	lea	0x5A827999(%ebx, %edx, 1), %edx
	movd	%mm0, %esi
	mov	%ebp, %ebx
	//12
	movd	%edx, %mm0
	mov	%eax, %ebp
	rol	$5, %edx
	xor	%ebx, %eax
	and	%esi, %eax
	xor	%ebx, %eax
	add	%edx, %ecx
	mov	48(%edi), %edx
	rol	$30, %esi
	bswap	%edx
	lea	0x5A827999(%eax, %ecx, 1), %ecx
	mov	%edx, 48(%esp)
	add	%edx, %ecx
	movd	%mm0, %edx
	mov	%ebp, %eax
	//13
	movd	%ecx, %mm0
	mov	%esi, %ebp
	rol	$5, %ecx
	xor	%eax, %esi
	and	%edx, %esi
	xor	%eax, %esi
	add	%ecx, %ebx
	mov	52(%edi), %ecx
	rol	$30, %edx
	bswap	%ecx
	mov	%ecx, 52(%esp)
	add	%ecx, %esi
	lea	0x5A827999(%esi, %ebx, 1), %ebx
	movd	%mm0, %ecx
	mov	%ebp, %esi
	//14
	movd	%ebx, %mm0
	mov	%edx, %ebp
	rol	$5, %ebx
	xor	%esi, %edx
	and	%ecx, %edx
	xor	%esi, %edx
	add	%ebx, %eax
	mov	56(%edi), %ebx
	rol	$30, %ecx
	bswap	%ebx
	lea	0x5A827999(%edx, %eax, 1), %eax
	mov	%ebx, 56(%esp)
	add	%ebx, %eax
	movd	%mm0, %ebx
	mov	%ebp, %edx
	//15
	movd	%eax, %mm0
	mov	%ecx, %ebp
	rol	$5, %eax
	xor	%edx, %ecx
	and	%ebx, %ecx
	xor	%edx, %ecx
	add	%eax, %esi
	mov	60(%edi), %eax
	rol	$30, %ebx
	bswap	%eax
	mov	%eax, 60(%esp)
	add	%eax, %ecx
	lea	0x5A827999(%ecx, %esi, 1), %esi
	movd	%mm0, %eax
	mov	%ebp, %ecx
	//16
	mov	%esi, %ebp
	mov	%ebx, %edi
	rol	$5, %esi
	xor	%ecx, %edi
	add	%esi, %edx
	mov	(%esp), %esi
	xor	8(%esp), %esi
	and	%eax, %edi
	xor	32(%esp), %esi
	xor	%ecx, %edi
	xor	52(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, (%esp)
	add	%esi, %edi
	lea	0x5A827999(%edi, %edx, 1), %edx
	mov	%ebp, %esi
	//17
	mov	%edx, %ebp
	mov	%eax, %edi
	rol	$5, %edx
	xor	%ebx, %edi
	add	%edx, %ecx
	mov	4(%esp), %edx
	xor	12(%esp), %edx
	and	%esi, %edi
	xor	36(%esp), %edx
	xor	%ebx, %edi
	xor	56(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 4(%esp)
	add	%edx, %edi
	lea	0x5A827999(%edi, %ecx, 1), %ecx
	mov	%ebp, %edx
	//18
	mov	%ecx, %ebp
	mov	%esi, %edi
	rol	$5, %ecx
	xor	%eax, %edi
	add	%ecx, %ebx
	mov	8(%esp), %ecx
	xor	16(%esp), %ecx
	and	%edx, %edi
	xor	40(%esp), %ecx
	xor	%eax, %edi
	xor	60(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 8(%esp)
	add	%ecx, %edi
	lea	0x5A827999(%edi, %ebx, 1), %ebx
	mov	%ebp, %ecx
	//19
	mov	%ebx, %ebp
	mov	%edx, %edi
	rol	$5, %ebx
	xor	%esi, %edi
	add	%ebx, %eax
	mov	(%esp), %ebx
	xor	12(%esp), %ebx
	and	%ecx, %edi
	xor	20(%esp), %ebx
	xor	%esi, %edi
	xor	44(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 12(%esp)
	add	%ebx, %edi
	lea	0x5A827999(%edi, %eax, 1), %eax
	mov	%ebp, %ebx
	//20
	mov	%eax, %ebp
	rol	$5, %eax
	mov	%ecx, %edi
	xor	%edx, %edi
	add	%eax, %esi
	mov	4(%esp), %eax
	xor	16(%esp), %eax
	xor	%ebx, %edi
	xor	24(%esp), %eax
	xor	48(%esp), %eax
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 16(%esp)
	add	%eax, %edi
	lea	0x6ED9EBA1(%edi, %esi, 1), %esi
	mov	%ebp, %eax
	//21
	mov	%esi, %ebp
	rol	$5, %esi
	mov	%ebx, %edi
	xor	%ecx, %edi
	add	%esi, %edx
	mov	8(%esp), %esi
	xor	20(%esp), %esi
	xor	%eax, %edi
	xor	28(%esp), %esi
	xor	52(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 20(%esp)
	add	%esi, %edi
	lea	0x6ED9EBA1(%edi, %edx, 1), %edx
	mov	%ebp, %esi
	//22
	mov	%edx, %ebp
	rol	$5, %edx
	mov	%eax, %edi
	xor	%ebx, %edi
	add	%edx, %ecx
	mov	12(%esp), %edx
	xor	24(%esp), %edx
	xor	%esi, %edi
	xor	32(%esp), %edx
	xor	56(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 24(%esp)
	add	%edx, %edi
	lea	0x6ED9EBA1(%edi, %ecx, 1), %ecx
	mov	%ebp, %edx
	//23
	mov	%ecx, %ebp
	rol	$5, %ecx
	mov	%esi, %edi
	xor	%eax, %edi
	add	%ecx, %ebx
	mov	16(%esp), %ecx
	xor	28(%esp), %ecx
	xor	%edx, %edi
	xor	36(%esp), %ecx
	xor	60(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 28(%esp)
	add	%ecx, %edi
	lea	0x6ED9EBA1(%edi, %ebx, 1), %ebx
	mov	%ebp, %ecx
	//24
	mov	%ebx, %ebp
	rol	$5, %ebx
	mov	%edx, %edi
	xor	%esi, %edi
	add	%ebx, %eax
	mov	(%esp), %ebx
	xor	20(%esp), %ebx
	xor	%ecx, %edi
	xor	32(%esp), %ebx
	xor	40(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 32(%esp)
	add	%ebx, %edi
	lea	0x6ED9EBA1(%edi, %eax, 1), %eax
	mov	%ebp, %ebx
	//25
	mov	%eax, %ebp
	rol	$5, %eax
	mov	%ecx, %edi
	xor	%edx, %edi
	add	%eax, %esi
	mov	4(%esp), %eax
	xor	24(%esp), %eax
	xor	%ebx, %edi
	xor	36(%esp), %eax
	xor	44(%esp), %eax
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 36(%esp)
	add	%eax, %edi
	lea	0x6ED9EBA1(%edi, %esi, 1), %esi
	mov	%ebp, %eax
	//26
	mov	%esi, %ebp
	rol	$5, %esi
	mov	%ebx, %edi
	xor	%ecx, %edi
	add	%esi, %edx
	mov	8(%esp), %esi
	xor	28(%esp), %esi
	xor	%eax, %edi
	xor	40(%esp), %esi
	xor	48(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 40(%esp)
	add	%esi, %edi
	lea	0x6ED9EBA1(%edi, %edx, 1), %edx
	mov	%ebp, %esi
	//27
	mov	%edx, %ebp
	rol	$5, %edx
	mov	%eax, %edi
	xor	%ebx, %edi
	add	%edx, %ecx
	mov	12(%esp), %edx
	xor	32(%esp), %edx
	xor	%esi, %edi
	xor	44(%esp), %edx
	xor	52(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 44(%esp)
	add	%edx, %edi
	lea	0x6ED9EBA1(%edi, %ecx, 1), %ecx
	mov	%ebp, %edx
	//28
	mov	%ecx, %ebp
	rol	$5, %ecx
	mov	%esi, %edi
	xor	%eax, %edi
	add	%ecx, %ebx
	mov	16(%esp), %ecx
	xor	36(%esp), %ecx
	xor	%edx, %edi
	xor	48(%esp), %ecx
	xor	56(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 48(%esp)
	add	%ecx, %edi
	lea	0x6ED9EBA1(%edi, %ebx, 1), %ebx
	mov	%ebp, %ecx
	//29
	mov	%ebx, %ebp
	rol	$5, %ebx
	mov	%edx, %edi
	xor	%esi, %edi
	add	%ebx, %eax
	mov	20(%esp), %ebx
	xor	40(%esp), %ebx
	xor	%ecx, %edi
	xor	52(%esp), %ebx
	xor	60(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 52(%esp)
	add	%ebx, %edi
	lea	0x6ED9EBA1(%edi, %eax, 1), %eax
	mov	%ebp, %ebx
	//30
	mov	%eax, %ebp
	rol	$5, %eax
	mov	%ecx, %edi
	xor	%edx, %edi
	add	%eax, %esi
	mov	(%esp), %eax
	xor	24(%esp), %eax
	xor	%ebx, %edi
	xor	44(%esp), %eax
	xor	56(%esp), %eax
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 56(%esp)
	add	%eax, %edi
	lea	0x6ED9EBA1(%edi, %esi, 1), %esi
	mov	%ebp, %eax
	//31
	mov	%esi, %ebp
	rol	$5, %esi
	mov	%ebx, %edi
	xor	%ecx, %edi
	add	%esi, %edx
	mov	4(%esp), %esi
	xor	28(%esp), %esi
	xor	%eax, %edi
	xor	48(%esp), %esi
	xor	60(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 60(%esp)
	add	%esi, %edi
	lea	0x6ED9EBA1(%edi, %edx, 1), %edx
	mov	%ebp, %esi
	//32
	mov	%edx, %ebp
	rol	$5, %edx
	mov	%eax, %edi
	xor	%ebx, %edi
	add	%edx, %ecx
	mov	(%esp), %edx
	xor	8(%esp), %edx
	xor	%esi, %edi
	xor	32(%esp), %edx
	xor	52(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, (%esp)
	add	%edx, %edi
	lea	0x6ED9EBA1(%edi, %ecx, 1), %ecx
	mov	%ebp, %edx
	//33
	mov	%ecx, %ebp
	rol	$5, %ecx
	mov	%esi, %edi
	xor	%eax, %edi
	add	%ecx, %ebx
	mov	4(%esp), %ecx
	xor	12(%esp), %ecx
	xor	%edx, %edi
	xor	36(%esp), %ecx
	xor	56(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 4(%esp)
	add	%ecx, %edi
	lea	0x6ED9EBA1(%edi, %ebx, 1), %ebx
	mov	%ebp, %ecx
	//34
	mov	%ebx, %ebp
	rol	$5, %ebx
	mov	%edx, %edi
	xor	%esi, %edi
	add	%ebx, %eax
	mov	8(%esp), %ebx
	xor	16(%esp), %ebx
	xor	%ecx, %edi
	xor	40(%esp), %ebx
	xor	60(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 8(%esp)
	add	%ebx, %edi
	lea	0x6ED9EBA1(%edi, %eax, 1), %eax
	mov	%ebp, %ebx
	//35
	mov	%eax, %ebp
	rol	$5, %eax
	mov	%ecx, %edi
	xor	%edx, %edi
	add	%eax, %esi
	mov	(%esp), %eax
	xor	12(%esp), %eax
	xor	%ebx, %edi
	xor	20(%esp), %eax
	xor	44(%esp), %eax
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 12(%esp)
	add	%eax, %edi
	lea	0x6ED9EBA1(%edi, %esi, 1), %esi
	mov	%ebp, %eax
	//36
	mov	%esi, %ebp
	rol	$5, %esi
	mov	%ebx, %edi
	xor	%ecx, %edi
	add	%esi, %edx
	mov	4(%esp), %esi
	xor	16(%esp), %esi
	xor	%eax, %edi
	xor	24(%esp), %esi
	xor	48(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 16(%esp)
	add	%esi, %edi
	lea	0x6ED9EBA1(%edi, %edx, 1), %edx
	mov	%ebp, %esi
	//37
	mov	%edx, %ebp
	rol	$5, %edx
	mov	%eax, %edi
	xor	%ebx, %edi
	add	%edx, %ecx
	mov	8(%esp), %edx
	xor	20(%esp), %edx
	xor	%esi, %edi
	xor	28(%esp), %edx
	xor	52(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 20(%esp)
	add	%edx, %edi
	lea	0x6ED9EBA1(%edi, %ecx, 1), %ecx
	mov	%ebp, %edx
	//38
	mov	%ecx, %ebp
	rol	$5, %ecx
	mov	%esi, %edi
	xor	%eax, %edi
	add	%ecx, %ebx
	mov	12(%esp), %ecx
	xor	24(%esp), %ecx
	xor	%edx, %edi
	xor	32(%esp), %ecx
	xor	56(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 24(%esp)
	add	%ecx, %edi
	lea	0x6ED9EBA1(%edi, %ebx, 1), %ebx
	mov	%ebp, %ecx
	//39
	mov	%ebx, %ebp
	rol	$5, %ebx
	mov	%edx, %edi
	xor	%esi, %edi
	add	%ebx, %eax
	mov	16(%esp), %ebx
	xor	28(%esp), %ebx
	xor	%ecx, %edi
	xor	36(%esp), %ebx
	xor	60(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 28(%esp)
	add	%ebx, %edi
	lea	0x6ED9EBA1(%edi, %eax, 1), %eax
	mov	%ebp, %ebx
	//40
	movd	%eax, %mm0
	mov	%ecx, %ebp
	mov	%ecx, %edi
	rol	$5, %eax
	and	%ebx, %ecx
	add	%eax, %esi
	mov	(%esp), %eax
	xor	20(%esp), %eax
	or	%ebx, %edi
	xor	32(%esp), %eax
	and	%edx, %edi
	xor	40(%esp), %eax
	or	%ecx, %edi
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 32(%esp)
	add	%eax, %edi
	lea	0x8F1BBCDC(%edi, %esi, 1), %esi
	movd	%mm0, %eax
	mov	%ebp, %ecx
	//41
	movd	%esi, %mm0
	mov	%ebx, %ebp
	mov	%ebx, %edi
	rol	$5, %esi
	and	%eax, %ebx
	add	%esi, %edx
	mov	4(%esp), %esi
	or	%eax, %edi
	xor	24(%esp), %esi
	and	%ecx, %edi
	xor	36(%esp), %esi
	or	%ebx, %edi
	xor	44(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 36(%esp)
	add	%esi, %edi
	lea	0x8F1BBCDC(%edi, %edx, 1), %edx
	movd	%mm0, %esi
	mov	%ebp, %ebx
	//42
	movd	%edx, %mm0
	mov	%eax, %ebp
	mov	%eax, %edi
	rol	$5, %edx
	and	%esi, %eax
	add	%edx, %ecx
	mov	8(%esp), %edx
	or	%esi, %edi
	xor	28(%esp), %edx
	and	%ebx, %edi
	xor	40(%esp), %edx
	or	%eax, %edi
	xor	48(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 40(%esp)
	add	%edx, %edi
	lea	0x8F1BBCDC(%edi, %ecx, 1), %ecx
	movd	%mm0, %edx
	mov	%ebp, %eax
	//43
	movd	%ecx, %mm0
	mov	%esi, %ebp
	mov	%esi, %edi
	rol	$5, %ecx
	and	%edx, %esi
	add	%ecx, %ebx
	mov	12(%esp), %ecx
	or	%edx, %edi
	xor	32(%esp), %ecx
	and	%eax, %edi
	xor	44(%esp), %ecx
	or	%esi, %edi
	xor	52(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 44(%esp)
	add	%ecx, %edi
	lea	0x8F1BBCDC(%edi, %ebx, 1), %ebx
	movd	%mm0, %ecx
	mov	%ebp, %esi
	//44
	movd	%ebx, %mm0
	mov	%edx, %ebp
	mov	%edx, %edi
	rol	$5, %ebx
	and	%ecx, %edx
	add	%ebx, %eax
	mov	16(%esp), %ebx
	or	%ecx, %edi
	xor	36(%esp), %ebx
	and	%esi, %edi
	xor	48(%esp), %ebx
	or	%edx, %edi
	xor	56(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 48(%esp)
	add	%ebx, %edi
	lea	0x8F1BBCDC(%edi, %eax, 1), %eax
	movd	%mm0, %ebx
	mov	%ebp, %edx
	//45
	movd	%eax, %mm0
	mov	%ecx, %ebp
	mov	%ecx, %edi
	rol	$5, %eax
	and	%ebx, %ecx
	add	%eax, %esi
	mov	20(%esp), %eax
	xor	40(%esp), %eax
	or	%ebx, %edi
	xor	52(%esp), %eax
	and	%edx, %edi
	xor	60(%esp), %eax
	or	%ecx, %edi
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 52(%esp)
	add	%eax, %edi
	lea	0x8F1BBCDC(%edi, %esi, 1), %esi
	movd	%mm0, %eax
	mov	%ebp, %ecx
	//46
	movd	%esi, %mm0
	mov	%ebx, %ebp
	mov	%ebx, %edi
	rol	$5, %esi
	and	%eax, %ebx
	add	%esi, %edx
	mov	(%esp), %esi
	or	%eax, %edi
	xor	24(%esp), %esi
	and	%ecx, %edi
	xor	44(%esp), %esi
	or	%ebx, %edi
	xor	56(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 56(%esp)
	add	%esi, %edi
	lea	0x8F1BBCDC(%edi, %edx, 1), %edx
	movd	%mm0, %esi
	mov	%ebp, %ebx
	//47
	movd	%edx, %mm0
	mov	%eax, %ebp
	mov	%eax, %edi
	rol	$5, %edx
	and	%esi, %eax
	add	%edx, %ecx
	mov	4(%esp), %edx
	or	%esi, %edi
	xor	28(%esp), %edx
	and	%ebx, %edi
	xor	48(%esp), %edx
	or	%eax, %edi
	xor	60(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 60(%esp)
	add	%edx, %edi
	lea	0x8F1BBCDC(%edi, %ecx, 1), %ecx
	movd	%mm0, %edx
	mov	%ebp, %eax
	//48
	movd	%ecx, %mm0
	mov	%esi, %ebp
	mov	%esi, %edi
	rol	$5, %ecx
	and	%edx, %esi
	add	%ecx, %ebx
	mov	(%esp), %ecx
	or	%edx, %edi
	xor	8(%esp), %ecx
	and	%eax, %edi
	xor	32(%esp), %ecx
	or	%esi, %edi
	xor	52(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, (%esp)
	add	%ecx, %edi
	lea	0x8F1BBCDC(%edi, %ebx, 1), %ebx
	movd	%mm0, %ecx
	mov	%ebp, %esi
	//49
	movd	%ebx, %mm0
	mov	%edx, %ebp
	mov	%edx, %edi
	rol	$5, %ebx
	and	%ecx, %edx
	add	%ebx, %eax
	mov	4(%esp), %ebx
	or	%ecx, %edi
	xor	12(%esp), %ebx
	and	%esi, %edi
	xor	36(%esp), %ebx
	or	%edx, %edi
	xor	56(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 4(%esp)
	add	%ebx, %edi
	lea	0x8F1BBCDC(%edi, %eax, 1), %eax
	movd	%mm0, %ebx
	mov	%ebp, %edx
	//50
	movd	%eax, %mm0
	mov	%ecx, %ebp
	mov	%ecx, %edi
	rol	$5, %eax
	and	%ebx, %ecx
	add	%eax, %esi
	mov	8(%esp), %eax
	xor	16(%esp), %eax
	or	%ebx, %edi
	xor	40(%esp), %eax
	and	%edx, %edi
	xor	60(%esp), %eax
	or	%ecx, %edi
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 8(%esp)
	add	%eax, %edi
	lea	0x8F1BBCDC(%edi, %esi, 1), %esi
	movd	%mm0, %eax
	mov	%ebp, %ecx
	//51
	movd	%esi, %mm0
	mov	%ebx, %ebp
	mov	%ebx, %edi
	rol	$5, %esi
	and	%eax, %ebx
	add	%esi, %edx
	mov	(%esp), %esi
	or	%eax, %edi
	xor	12(%esp), %esi
	and	%ecx, %edi
	xor	20(%esp), %esi
	or	%ebx, %edi
	xor	44(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 12(%esp)
	add	%esi, %edi
	lea	0x8F1BBCDC(%edi, %edx, 1), %edx
	movd	%mm0, %esi
	mov	%ebp, %ebx
	//52
	movd	%edx, %mm0
	mov	%eax, %ebp
	mov	%eax, %edi
	rol	$5, %edx
	and	%esi, %eax
	add	%edx, %ecx
	mov	4(%esp), %edx
	or	%esi, %edi
	xor	16(%esp), %edx
	and	%ebx, %edi
	xor	24(%esp), %edx
	or	%eax, %edi
	xor	48(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 16(%esp)
	add	%edx, %edi
	lea	0x8F1BBCDC(%edi, %ecx, 1), %ecx
	movd	%mm0, %edx
	mov	%ebp, %eax
	//53
	movd	%ecx, %mm0
	mov	%esi, %ebp
	mov	%esi, %edi
	rol	$5, %ecx
	and	%edx, %esi
	add	%ecx, %ebx
	mov	8(%esp), %ecx
	or	%edx, %edi
	xor	20(%esp), %ecx
	and	%eax, %edi
	xor	28(%esp), %ecx
	or	%esi, %edi
	xor	52(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 20(%esp)
	add	%ecx, %edi
	lea	0x8F1BBCDC(%edi, %ebx, 1), %ebx
	movd	%mm0, %ecx
	mov	%ebp, %esi
	//54
	movd	%ebx, %mm0
	mov	%edx, %ebp
	mov	%edx, %edi
	rol	$5, %ebx
	and	%ecx, %edx
	add	%ebx, %eax
	mov	12(%esp), %ebx
	or	%ecx, %edi
	xor	24(%esp), %ebx
	and	%esi, %edi
	xor	32(%esp), %ebx
	or	%edx, %edi
	xor	56(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 24(%esp)
	add	%ebx, %edi
	lea	0x8F1BBCDC(%edi, %eax, 1), %eax
	movd	%mm0, %ebx
	mov	%ebp, %edx
	//55
	movd	%eax, %mm0
	mov	%ecx, %ebp
	mov	%ecx, %edi
	rol	$5, %eax
	and	%ebx, %ecx
	add	%eax, %esi
	mov	16(%esp), %eax
	xor	28(%esp), %eax
	or	%ebx, %edi
	xor	36(%esp), %eax
	and	%edx, %edi
	xor	60(%esp), %eax
	or	%ecx, %edi
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 28(%esp)
	add	%eax, %edi
	lea	0x8F1BBCDC(%edi, %esi, 1), %esi
	movd	%mm0, %eax
	mov	%ebp, %ecx
	//56
	movd	%esi, %mm0
	mov	%ebx, %ebp
	mov	%ebx, %edi
	rol	$5, %esi
	and	%eax, %ebx
	add	%esi, %edx
	mov	(%esp), %esi
	or	%eax, %edi
	xor	20(%esp), %esi
	and	%ecx, %edi
	xor	32(%esp), %esi
	or	%ebx, %edi
	xor	40(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 32(%esp)
	add	%esi, %edi
	lea	0x8F1BBCDC(%edi, %edx, 1), %edx
	movd	%mm0, %esi
	mov	%ebp, %ebx
	//57
	movd	%edx, %mm0
	mov	%eax, %ebp
	mov	%eax, %edi
	rol	$5, %edx
	and	%esi, %eax
	add	%edx, %ecx
	mov	4(%esp), %edx
	or	%esi, %edi
	xor	24(%esp), %edx
	and	%ebx, %edi
	xor	36(%esp), %edx
	or	%eax, %edi
	xor	44(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 36(%esp)
	add	%edx, %edi
	lea	0x8F1BBCDC(%edi, %ecx, 1), %ecx
	movd	%mm0, %edx
	mov	%ebp, %eax
	//58
	movd	%ecx, %mm0
	mov	%esi, %ebp
	mov	%esi, %edi
	rol	$5, %ecx
	and	%edx, %esi
	add	%ecx, %ebx
	mov	8(%esp), %ecx
	or	%edx, %edi
	xor	28(%esp), %ecx
	and	%eax, %edi
	xor	40(%esp), %ecx
	or	%esi, %edi
	xor	48(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 40(%esp)
	add	%ecx, %edi
	lea	0x8F1BBCDC(%edi, %ebx, 1), %ebx
	movd	%mm0, %ecx
	mov	%ebp, %esi
	//59
	movd	%ebx, %mm0
	mov	%edx, %ebp
	mov	%edx, %edi
	rol	$5, %ebx
	and	%ecx, %edx
	add	%ebx, %eax
	mov	12(%esp), %ebx
	or	%ecx, %edi
	xor	32(%esp), %ebx
	and	%esi, %edi
	xor	44(%esp), %ebx
	or	%edx, %edi
	xor	52(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 44(%esp)
	add	%ebx, %edi
	lea	0x8F1BBCDC(%edi, %eax, 1), %eax
	movd	%mm0, %ebx
	mov	%ebp, %edx
	//60
	mov	%eax, %ebp
	mov	%ecx, %edi
	rol	$5, %eax
	xor	%edx, %edi
	add	%eax, %esi
	mov	16(%esp), %eax
	xor	36(%esp), %eax
	xor	%ebx, %edi
	xor	48(%esp), %eax
	xor	56(%esp), %eax
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 48(%esp)
	add	%eax, %edi
	lea	0xCA62C1D6(%edi, %esi, 1), %esi
	mov	%ebp, %eax
	//61
	mov	%esi, %ebp
	mov	%ebx, %edi
	rol	$5, %esi
	xor	%ecx, %edi
	add	%esi, %edx
	mov	20(%esp), %esi
	xor	40(%esp), %esi
	xor	%eax, %edi
	xor	52(%esp), %esi
	xor	60(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 52(%esp)
	add	%esi, %edi
	lea	0xCA62C1D6(%edi, %edx, 1), %edx
	mov	%ebp, %esi
	//62
	mov	%edx, %ebp
	mov	%eax, %edi
	rol	$5, %edx
	xor	%ebx, %edi
	add	%edx, %ecx
	mov	(%esp), %edx
	xor	24(%esp), %edx
	xor	%esi, %edi
	xor	44(%esp), %edx
	xor	56(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 56(%esp)
	add	%edx, %edi
	lea	0xCA62C1D6(%edi, %ecx, 1), %ecx
	mov	%ebp, %edx
	//63
	mov	%ecx, %ebp
	mov	%esi, %edi
	rol	$5, %ecx
	xor	%eax, %edi
	add	%ecx, %ebx
	mov	4(%esp), %ecx
	xor	28(%esp), %ecx
	xor	%edx, %edi
	xor	48(%esp), %ecx
	xor	60(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 60(%esp)
	add	%ecx, %edi
	lea	0xCA62C1D6(%edi, %ebx, 1), %ebx
	mov	%ebp, %ecx
	//64
	mov	%ebx, %ebp
	mov	%edx, %edi
	rol	$5, %ebx
	xor	%esi, %edi
	add	%ebx, %eax
	mov	(%esp), %ebx
	xor	8(%esp), %ebx
	xor	%ecx, %edi
	xor	32(%esp), %ebx
	xor	52(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, (%esp)
	add	%ebx, %edi
	lea	0xCA62C1D6(%edi, %eax, 1), %eax
	mov	%ebp, %ebx
	//65
	mov	%eax, %ebp
	mov	%ecx, %edi
	rol	$5, %eax
	xor	%edx, %edi
	add	%eax, %esi
	mov	4(%esp), %eax
	xor	12(%esp), %eax
	xor	%ebx, %edi
	xor	36(%esp), %eax
	xor	56(%esp), %eax
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 4(%esp)
	add	%eax, %edi
	lea	0xCA62C1D6(%edi, %esi, 1), %esi
	mov	%ebp, %eax
	//66
	mov	%esi, %ebp
	mov	%ebx, %edi
	rol	$5, %esi
	xor	%ecx, %edi
	add	%esi, %edx
	mov	8(%esp), %esi
	xor	16(%esp), %esi
	xor	%eax, %edi
	xor	40(%esp), %esi
	xor	60(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 8(%esp)
	add	%esi, %edi
	lea	0xCA62C1D6(%edi, %edx, 1), %edx
	mov	%ebp, %esi
	//67
	mov	%edx, %ebp
	mov	%eax, %edi
	rol	$5, %edx
	xor	%ebx, %edi
	add	%edx, %ecx
	mov	(%esp), %edx
	xor	12(%esp), %edx
	xor	%esi, %edi
	xor	20(%esp), %edx
	xor	44(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 12(%esp)
	add	%edx, %edi
	lea	0xCA62C1D6(%edi, %ecx, 1), %ecx
	mov	%ebp, %edx
	//68
	mov	%ecx, %ebp
	mov	%esi, %edi
	rol	$5, %ecx
	xor	%eax, %edi
	add	%ecx, %ebx
	mov	4(%esp), %ecx
	xor	16(%esp), %ecx
	xor	%edx, %edi
	xor	24(%esp), %ecx
	xor	48(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 16(%esp)
	add	%ecx, %edi
	lea	0xCA62C1D6(%edi, %ebx, 1), %ebx
	mov	%ebp, %ecx
	//69
	mov	%ebx, %ebp
	mov	%edx, %edi
	rol	$5, %ebx
	xor	%esi, %edi
	add	%ebx, %eax
	mov	8(%esp), %ebx
	xor	20(%esp), %ebx
	xor	%ecx, %edi
	xor	28(%esp), %ebx
	xor	52(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 20(%esp)
	add	%ebx, %edi
	lea	0xCA62C1D6(%edi, %eax, 1), %eax
	mov	%ebp, %ebx
	//70
	mov	%eax, %ebp
	mov	%ecx, %edi
	rol	$5, %eax
	xor	%edx, %edi
	add	%eax, %esi
	mov	12(%esp), %eax
	xor	24(%esp), %eax
	xor	%ebx, %edi
	xor	32(%esp), %eax
	xor	56(%esp), %eax
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 24(%esp)
	add	%eax, %edi
	lea	0xCA62C1D6(%edi, %esi, 1), %esi
	mov	%ebp, %eax
	//71
	mov	%esi, %ebp
	mov	%ebx, %edi
	rol	$5, %esi
	xor	%ecx, %edi
	add	%esi, %edx
	mov	16(%esp), %esi
	xor	28(%esp), %esi
	xor	%eax, %edi
	xor	36(%esp), %esi
	xor	60(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 28(%esp)
	add	%esi, %edi
	lea	0xCA62C1D6(%edi, %edx, 1), %edx
	mov	%ebp, %esi
	//72
	mov	%edx, %ebp
	mov	%eax, %edi
	rol	$5, %edx
	xor	%ebx, %edi
	add	%edx, %ecx
	mov	(%esp), %edx
	xor	20(%esp), %edx
	xor	%esi, %edi
	xor	32(%esp), %edx
	xor	40(%esp), %edx
	rol	%edx
	rol	$30, %esi
	mov	%edx, 32(%esp)
	add	%edx, %edi
	lea	0xCA62C1D6(%edi, %ecx, 1), %ecx
	mov	%ebp, %edx
	//73
	mov	%ecx, %ebp
	mov	%esi, %edi
	rol	$5, %ecx
	xor	%eax, %edi
	add	%ecx, %ebx
	mov	4(%esp), %ecx
	xor	24(%esp), %ecx
	xor	%edx, %edi
	xor	36(%esp), %ecx
	xor	44(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	mov	%ecx, 36(%esp)
	add	%ecx, %edi
	lea	0xCA62C1D6(%edi, %ebx, 1), %ebx
	mov	%ebp, %ecx
	//74
	mov	%ebx, %ebp
	mov	%edx, %edi
	rol	$5, %ebx
	xor	%esi, %edi
	add	%ebx, %eax
	mov	8(%esp), %ebx
	xor	28(%esp), %ebx
	xor	%ecx, %edi
	xor	40(%esp), %ebx
	xor	48(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	mov	%ebx, 40(%esp)
	add	%ebx, %edi
	lea	0xCA62C1D6(%edi, %eax, 1), %eax
	mov	%ebp, %ebx
	//75
	mov	%eax, %ebp
	mov	%ecx, %edi
	rol	$5, %eax
	xor	%edx, %edi
	add	%eax, %esi
	mov	12(%esp), %eax
	xor	32(%esp), %eax
	xor	%ebx, %edi
	xor	44(%esp), %eax
	xor	52(%esp), %eax
	rol	%eax
	rol	$30, %ebx
	mov	%eax, 44(%esp)
	add	%eax, %edi
	lea	0xCA62C1D6(%edi, %esi, 1), %esi
	mov	%ebp, %eax
	//76
	mov	%esi, %ebp
	mov	%ebx, %edi
	rol	$5, %esi
	xor	%ecx, %edi
	add	%esi, %edx
	mov	16(%esp), %esi
	xor	36(%esp), %esi
	xor	%eax, %edi
	xor	48(%esp), %esi
	xor	56(%esp), %esi
	rol	%esi
	rol	$30, %eax
	mov	%esi, 48(%esp)
	add	%esi, %edi
	lea	0xCA62C1D6(%edi, %edx, 1), %edx
	mov	%ebp, %esi
	//77
	mov	%edx, %ebp
	mov	%eax, %edi
	rol	$5, %edx
	xor	%ebx, %edi
	add	%edx, %ecx
	mov	20(%esp), %edx
	xor	40(%esp), %edx
	xor	%esi, %edi
	xor	52(%esp), %edx
	xor	60(%esp), %edx
	rol	%edx
	rol	$30, %esi
	add	%edx, %edi
	lea	0xCA62C1D6(%edi, %ecx, 1), %ecx
	mov	%ebp, %edx
	//78
	mov	%ecx, %ebp
	mov	%esi, %edi
	rol	$5, %ecx
	xor	%eax, %edi
	add	%ecx, %ebx
	mov	(%esp), %ecx
	xor	24(%esp), %ecx
	xor	%edx, %edi
	xor	44(%esp), %ecx
	xor	56(%esp), %ecx
	rol	%ecx
	rol	$30, %edx
	add	%ecx, %edi
	lea	0xCA62C1D6(%edi, %ebx, 1), %ebx
	mov	%ebp, %ecx
	//79
	mov	%ebx, %ebp
	mov	%edx, %edi
	rol	$5, %ebx
	xor	%esi, %edi
	add	%ebx, %eax
	mov	4(%esp), %ebx
	xor	28(%esp), %ebx
	xor	%ecx, %edi
	xor	48(%esp), %ebx
	xor	60(%esp), %ebx
	rol	%ebx
	rol	$30, %ecx
	add	%ebx, %edi
	lea	0xCA62C1D6(%edi, %eax, 1), %eax
	mov	%ebp, %ebx
	.endm

#extern "C" void sha1_init_context( unsigned char context[92] );
#extern "C" void sha1_update( unsigned char context[92], const void *input, unsigned int inputlen );
#extern "C" void sha1_final( unsigned char context[92] );
#extern "C" void sha1_digest( unsigned char digest[20], const void *input, unsigned int inputlen );
.global sha1_init_context, sha1_update, sha1_final, sha1_digest
	.type	sha1_init_context, @function
	.type	sha1_update, @function
	.type	sha1_final, @function
	.type	sha1_digest, @function

	.align	2
sha1_init_context:
.sha1_init_context:
	mov	4(%esp), %edx
	xor	%eax, %eax
	movl	$0x67452301, (%edx)
	movl	$0xEFCDAB89, 4(%edx)
	movl	$0x98BADCFE, 8(%edx)
	movl	$0x10325476, 12(%edx)
	movl	$0xC3D2E1F0, 16(%edx)
	mov	%eax, 20(%edx)
	mov	%eax, 24(%edx)
	ret

	.align	2
sha1_digest:
	add	$-92, %esp
	push	%esp
	call	.sha1_init_context
	add	$4, %esp
	mov	%esp, %ecx
	push	104(%ecx)
	push	100(%ecx)
	push	%ecx
	call	.sha1_update
	add	$12, %esp
	push	%esp
	call	.sha1_final
	add	$4, %esp
	mov	96(%esp), %ecx
	mov	(%esp), %eax
	mov	4(%esp), %edx
	mov	%eax, (%ecx)
	mov	%edx, 4(%ecx)
	mov	8(%esp), %eax
	mov	12(%esp), %edx
	mov	%eax, 8(%ecx)
	mov	%edx, 12(%ecx)
	mov	16(%esp), %eax
	mov	%eax, 16(%ecx)
	add	$92, %esp
	ret

	.align	2
sha1_update:
.sha1_update:
	add	$-80, %esp
	mov	%ebx, 64(%esp)
	mov	%esi, 68(%esp)
	mov	%edi, 72(%esp)
	mov	%ebp, 76(%esp)
	mov	84(%esp), %ebx
	mov	88(%esp), %esi
	mov	92(%esp), %ebp
	mov	20(%ebx), %eax
	add	%ebp, 20(%ebx)
	adc	$0, 24(%ebx)
	and	$0x3F, %eax
	mov	$64, %ecx
	jz	.Lhas_no_remain
	sub	%eax, %ecx
	cmp	%ecx, %ebp
	lea	28(%ebx, %eax), %edi
	jae	.Lneed_transform
	mov	%ebp, %ecx
	and	$3, %ebp
	shr	$2, %ecx
	cld
	rep	movsl
	mov	%ebp, %ecx
	rep	movsb
	mov	64(%esp), %ebx
	mov	68(%esp), %esi
	mov	72(%esp), %edi
	mov	76(%esp), %ebp
	add	$80, %esp
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
	call	sha1_transform
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
	lea	28(%ebx), %edi
	cld
	rep	movsl
	mov	%eax, %ecx
	rep	movsb
	mov	%edx, %esi
.Lneed_not_copy_remain:
	shr	$6, %ebp
	jz	.Lfinish
	mov	%ebx, %edi
	mov	%esi, 88(%esp)
	mov	(%edi),	  %eax
	mov	4(%edi),  %ebx
	mov	8(%edi),  %ecx
	mov	12(%edi), %edx
	mov	16(%edi), %esi
	mov	88(%esp), %edi
.Lupdate_next:
	mov	%edi, 88(%esp)
	mov	%ebp, 92(%esp)
	SHA1_ROUND80
	mov	84(%esp), %edi
	add	(%edi), %eax
	add	4(%edi), %ebx
	add	8(%edi), %ecx
	add	12(%edi), %edx
	add	16(%edi), %esi
	mov	%eax, (%edi)
	mov	%ebx, 4(%edi)
	mov	%ecx, 8(%edi)
	mov	%edx, 12(%edi)
	mov	%esi, 16(%edi)
	mov	88(%esp), %edi
	mov	92(%esp), %ebp
	add	$64, %edi
	add	$-1, %ebp
	jnz	.Lupdate_next
	emms
.Lfinish:
	mov	64(%esp), %ebx
	mov	68(%esp), %esi
	mov	72(%esp), %edi
	mov	76(%esp), %ebp
	add	$80, %esp
	ret

	.align	2
sha1_final:
.sha1_final:
	add	$-16, %esp
	mov	%ebx, (%esp)
	mov	%esi, 4(%esp)
	mov	%edi, 8(%esp)
	mov	%ebp, 12(%esp)
	mov	20(%esp), %ebx
	mov	20(%ebx), %ecx
	and	$0x3F, %ecx
	movb	$0x80, 28(%ebx, %ecx)
	cmp	$56, %ecx
	lea	29(%ebx, %ecx), %edi
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
	call	sha1_transform
	lea	28(%ebx), %edi
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
	mov	20(%ebx), %eax
	mov	24(%ebx), %edx
	shld	$3, %eax, %edx
	shl	$3, %eax
	bswap	%edx
	bswap	%eax
	mov	%edx, (%edi)
	mov	%eax, 4(%edi)
	push	%ebx
	call	sha1_transform
	mov	(%ebx),   %eax
	mov	4(%ebx),  %ecx
	mov	8(%ebx),  %edx
	mov	12(%ebx), %esi
	mov	16(%ebx), %edi
	bswap	%eax
	bswap	%ecx
	bswap	%edx
	bswap	%esi
	bswap	%edi
	mov	%eax, (%ebx)
	mov	%ecx, 4(%ebx)
	mov	%edx, 8(%ebx)
	mov	%esi, 12(%ebx)
	mov	%edi, 16(%ebx)
	mov	12(%esp), %ebp
	mov	8(%esp), %edi
	mov	4(%esp), %esi
	mov	(%esp), %ebx
	add	$16, %esp
	ret

	.align	2
sha1_transform:
	add	$-80, %esp
	mov	%ebx, 64(%esp)
	mov	%esi, 68(%esp)
	mov	%edi, 72(%esp)
	mov	%ebp, 76(%esp)
	mov	84(%esp), %edi
	mov	(%edi),	  %eax
	mov	4(%edi),  %ebx
	mov	8(%edi),  %ecx
	mov	12(%edi), %edx
	mov	16(%edi), %esi
	mov	84(%esp), %edi
	add	$28, %edi
	SHA1_ROUND80
	mov	84(%esp), %edi
	add	%eax, (%edi)
	add	%ebx, 4(%edi)
	add	%ecx, 8(%edi)
	add	%edx, 12(%edi)
	add	%esi, 16(%edi)
	mov	64(%esp), %ebx
	mov	68(%esp), %esi
	mov	72(%esp), %edi
	mov	76(%esp), %ebp
	add	$80, %esp
	emms
	ret	$4
