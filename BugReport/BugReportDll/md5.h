// md5.h
//
// date     : 2004-6-20
//
//////////////////////////////////////////////////////////////////////

#ifndef MD5_H__
#define MD5_H__

class md5_t
{
public:

	void init();
	void update(const unsigned char* data, unsigned int len);
	void final(unsigned char digest[16]);

private:

	static void transform(unsigned long state[4], const unsigned char buffer[64]);
	
	unsigned long m_state[4];
	unsigned long m_count[2];
	unsigned char m_buffer[64];

public:

	static void build(unsigned char hash[16], const void* buf, unsigned long len);
};

#endif
