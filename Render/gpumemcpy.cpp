#include "stdafx.h"
#include "gpumemcpy.h"
#include <emmintrin.h>
#include <smmintrin.h>
void* gpu_memcpy(void* d, const void* s, size_t size)
{
	static const size_t regsInLoop = sizeof(size_t) * 2; // 8 or 16

	if (d == NULL|| s == NULL) 
		return NULL;

	// If memory is not aligned, use memcpy
	bool isAligned = (((size_t)(s) | (size_t)(d)) & 0xF) == 0;
	if (!isAligned)
	{
		return memcpy(d, s, size);
	}

	__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
#ifdef _M_X64
	__m128i xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
#endif

	size_t reminder = size & (regsInLoop * sizeof(xmm0) - 1); // Copy 128 or 256 bytes every loop
	size_t end = 0;

	__m128i* pTrg = (__m128i*)d;
	__m128i* pTrgEnd = pTrg + ((size - reminder) >> 4);
	__m128i* pSrc = (__m128i*)s;

	// Make sure source is synced - doesn't hurt if not needed.
	_mm_sfence();

	while (pTrg < pTrgEnd)
	{
		// _mm_stream_load_si128 emits the Streaming SIMD Extensions 4 (SSE4.1) instruction MOVNTDQA
		// Fastest method for copying GPU RAM. Available since Penryn (45nm Core 2 Duo/Quad)
		xmm0  = _mm_stream_load_si128(pSrc);
		xmm1  = _mm_stream_load_si128(pSrc + 1);
		xmm2  = _mm_stream_load_si128(pSrc + 2);
		xmm3  = _mm_stream_load_si128(pSrc + 3);
		xmm4  = _mm_stream_load_si128(pSrc + 4);
		xmm5  = _mm_stream_load_si128(pSrc + 5);
		xmm6  = _mm_stream_load_si128(pSrc + 6);
		xmm7  = _mm_stream_load_si128(pSrc + 7);
#ifdef _M_X64 // Use all 16 xmm registers
		xmm8  = _mm_stream_load_si128(pSrc + 8);
		xmm9  = _mm_stream_load_si128(pSrc + 9);
		xmm10 = _mm_stream_load_si128(pSrc + 10);
		xmm11 = _mm_stream_load_si128(pSrc + 11);
		xmm12 = _mm_stream_load_si128(pSrc + 12);
		xmm13 = _mm_stream_load_si128(pSrc + 13);
		xmm14 = _mm_stream_load_si128(pSrc + 14);
		xmm15 = _mm_stream_load_si128(pSrc + 15);
#endif
		pSrc += regsInLoop;
		// _mm_store_si128 emit the SSE2 intruction MOVDQA (aligned store)
		_mm_store_si128(pTrg     , xmm0);
		_mm_store_si128(pTrg +  1, xmm1);
		_mm_store_si128(pTrg +  2, xmm2);
		_mm_store_si128(pTrg +  3, xmm3);
		_mm_store_si128(pTrg +  4, xmm4);
		_mm_store_si128(pTrg +  5, xmm5);
		_mm_store_si128(pTrg +  6, xmm6);
		_mm_store_si128(pTrg +  7, xmm7);
#ifdef _M_X64 // Use all 16 xmm registers
		_mm_store_si128(pTrg +  8, xmm8);
		_mm_store_si128(pTrg +  9, xmm9);
		_mm_store_si128(pTrg + 10, xmm10);
		_mm_store_si128(pTrg + 11, xmm11);
		_mm_store_si128(pTrg + 12, xmm12);
		_mm_store_si128(pTrg + 13, xmm13);
		_mm_store_si128(pTrg + 14, xmm14);
		_mm_store_si128(pTrg + 15, xmm15);
#endif
		pTrg += regsInLoop;
	}

	// Copy in 16 byte steps
	if (reminder >= 16)
	{
		size = reminder;
		reminder = size & 15;
		end = size >> 4;
		for (size_t i = 0; i < end; ++i)
		{
			pTrg[i] = _mm_stream_load_si128(pSrc + i);
		}
	}

	// Copy last bytes - shouldn't happen as strides are modulu 16
	if (reminder)
	{
		__m128i temp = _mm_stream_load_si128(pSrc + end);

		char* ps = (char*)(&temp);
		char* pt = (char*)(pTrg + end);

		for (size_t i = 0; i < reminder; ++i)
		{
			pt[i] = ps[i];
		}
	}
	return d;
}
void CopyFrameNV12_fallback(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch)
{
	const size_t size = imageHeight * pitch;
	memcpy(pY, pSourceData, size);
	memcpy(pUV, pSourceData + (surfaceHeight * pitch), size >> 1);
}

void CopyFrameNV12_fallback_MT(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch)
{
	int i;
	const size_t halfSize = (imageHeight * pitch) >> 1;
	for(i=0;i<3;i++) {
		if (i < 2)
			memcpy(pY + (halfSize * i), pSourceData + (halfSize * i), halfSize);
		else
			memcpy(pUV, pSourceData + (surfaceHeight * pitch), halfSize);
	}
}

void CopyFrameNV12_SSE4(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch)
{
	const size_t size = imageHeight * pitch;
	gpu_memcpy(pY, pSourceData, size);
	gpu_memcpy(pUV, pSourceData + (surfaceHeight * pitch), size >> 1);
}

void CopyFrameNV12_SSE4_MT(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch)
{
	int i;
	const size_t halfSize = (imageHeight * pitch) >> 1;
	for(i=0;i<3;i++)  {
		if (i < 2){
			gpu_memcpy(pY + (halfSize * i), pSourceData + (halfSize * i), halfSize);
		}
		else{
			gpu_memcpy(pUV, pSourceData + (surfaceHeight * pitch), halfSize);
		}
	}
}
static void (*fpCopyFrameNV12)(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch) = 0;


#define VEND_ID_ATI     0x1002
#define VEND_ID_NVIDIA  0x10DE
#define VEND_ID_INTEL   0x8086
void IniCopyFrameNV12(int m_dwVendorId,int cpu_flags )
{
	if (fpCopyFrameNV12 == 0) {
		if (cpu_flags &  0x0100) {
			if (m_dwVendorId == VEND_ID_INTEL)
				fpCopyFrameNV12 = CopyFrameNV12_SSE4_MT;
			else
				fpCopyFrameNV12 = CopyFrameNV12_SSE4;
		} else {
			if (m_dwVendorId == VEND_ID_INTEL)
				fpCopyFrameNV12 = CopyFrameNV12_fallback_MT;
			else
				fpCopyFrameNV12 = CopyFrameNV12_fallback;
		}
	}/**/
}
void CopyFrameNV12(const BYTE *pSourceData, BYTE *pY, BYTE *pUV, size_t surfaceHeight, size_t imageHeight, size_t pitch)
{
   fpCopyFrameNV12(pSourceData,pY,pUV,surfaceHeight,imageHeight,pitch);
}