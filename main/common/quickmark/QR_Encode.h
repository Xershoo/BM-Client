// QR_Encode.h : CQR_Encode 
// Date 2006/05/17	Ver. 1.22	Psytec Inc.

#if !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_)
#define AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <QImage>
#include "./varname.h"
/////////////////////////////////////////////////////////////////////////////
// 

// 
#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3

// 
#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3

// 
#define QR_VRESION_S	0 // 1
#define QR_VRESION_M	1 // 10 
#define QR_VRESION_L	2 // 27 

#define MAX_ALLCODEWORD	 3706 // 
#define MAX_DATACODEWORD 2956 // 
#define MAX_CODEBLOCK	  153 // 
#define MAX_MODULESIZE	  177 // 

// 
#define QR_MARGIN	0


/////////////////////////////////////////////////////////////////////////////
typedef struct tagRS_BLOCKINFO
{
	int ncRSBlock;		// 
	int ncAllCodeWord;	// 
	int ncDataCodeWord;	// 

} RS_BLOCKINFO, *LPRS_BLOCKINFO;


/////////////////////////////////////////////////////////////////////////////
// QR

typedef struct tagQR_VERSIONINFO
{
	int nVersionNo;	   // 
	int ncAllCodeWord; // 

	// (0 = L, 1 = M, 2 = Q, 3 = H) 
	int ncDataCodeWord[4];	//

	int ncAlignPoint;	// 
	int nAlignPoint[6];	// 

	RS_BLOCKINFO RS_BlockInfo1[4]; // 
	RS_BLOCKINFO RS_BlockInfo2[4]; // 

} QR_VERSIONINFO, *LPQR_VERSIONINFO;


/////////////////////////////////////////////////////////////////////////////
// CQR_Encode 

class CQR_Encode
{
// 
public:
	CQR_Encode();
	~CQR_Encode();

public:
	int m_nLevel;		// 
	int m_nVersion;		// 
	bool m_bAutoExtent;	// 
	int m_nMaskingNo;	// 

public:
	int m_nSymbleSize;
	BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE]; // [x][y]
	// bit5:
	// bit4:
	// bit1:
	// bit0:
	// 20h

private:
	int m_ncDataCodeWordBit; // 
	BYTE m_byDataCodeWord[MAX_DATACODEWORD]; // 

	int m_ncDataBlock;
	BYTE m_byBlockMode[MAX_DATACODEWORD];
	int m_nBlockLength[MAX_DATACODEWORD];

	int m_ncAllCodeWord; // 
	BYTE m_byAllCodeWord[MAX_ALLCODEWORD]; // 
	BYTE m_byRSWork[MAX_CODEBLOCK]; // 

// データエンコード関連ファンクション
public:
    QImage QRImage(int nLevel, int nVersion, bool bAutoExtent, int nMaskingNo, LPCWSTR lpsSource, int ncSource = 0);
	bool EncodeData(int nLevel, int nVersion, bool bAutoExtent, int nMaskingNo, LPCWSTR lpsSource, int ncSource = 0);

private:
	int GetEncodeVersion(int nVersion, LPCWSTR lpsSource, int ncLength);
	bool EncodeSourceData(LPCWSTR lpsSource, int ncLength, int nVerGroup);

	int GetBitLength(BYTE nMode, int ncData, int nVerGroup);

	int SetBitStream(int nIndex, WORD wData, int ncData);

	bool IsNumeralData(unsigned char c);
	bool IsAlphabetData(unsigned char c);
	bool IsKanjiData(unsigned char c1, unsigned char c2);

	BYTE AlphabetToBinaly(unsigned char c);
	WORD KanjiToBinaly(WORD wc);

	void GetRSCodeWord(BYTE* lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);

// 
private:
	void FormatModule();

	void SetFunctionModule();
	void SetFinderPattern(int x, int y);
	void SetAlignmentPattern(int x, int y);
	void SetVersionPattern();
	void SetCodeWordPattern();
	void SetMaskingPattern(int nPatternNo);
	void SetFormatInfoPattern(int nPatternNo);
	int CountPenalty();
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_)
