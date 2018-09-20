
#include "security.h"


namespace gnet
{
	Security::Map& Security::GetMap()
	{
		// ������GetMap���棬ȷ���ڵ���GetMapʱ��static �����϶�����ʼ����
		// ���������������cpp�У�ȷ��share.lib���Ա���ȷ���ӡ�
		static Security::Map map;
		return map;
	}

	int Random::fd = Random::Init();
	static Random          __Random(RANDOM);
	static NullSecurity    __NullSecurity_stub(NULLSECURITY);
	static ARCFourSecurity __ARCFourSecurity_stub(ARCFOURSECURITY);
	static MD5Hash         __MD5Hash_stub(MD5HASH);
	static HMAC_MD5Hash    __HMAC_MD5Hash_stub(HMAC_MD5HASH);
	static CompressARCFourSecurity  __CompressARCFourSecurity_stub(COMPRESSARCFOURSECURITY);
	static DecompressARCFourSecurity  __DecompressARCFourSecurity_stub(DECOMPRESSARCFOURSECURITY);
	static SHA1Hash         __SHA1Hash_stub(SHA1HASH);
}