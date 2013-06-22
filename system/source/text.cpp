#include <vector>
#include <algorithm>

#include <stdarg.h>
#include <stdio.h>

#include <windows.h>

#include <vd2/system/vdtypes.h>
#include <vd2/system/text.h>
#include <vd2/system/tls.h>
#include <vd2/system/VDString.h>

const wchar_t *VDFastTextAToW(const char *s) {
	int cnt;

	if (!s)
		return NULL;

	cnt = MultiByteToWideChar(CP_ACP, 0, s, -1, g_tlsdata.fastBufW, sizeof g_tlsdata.fastBufW / sizeof(wchar_t));

	if (cnt)
		return g_tlsdata.fastBufW;

	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		cnt = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);

		if (cnt) {
			g_tlsdata.pFastBufAlloc = malloc(sizeof(wchar_t) * cnt);

			cnt = MultiByteToWideChar(CP_ACP, 0, s, -1, (LPWSTR)g_tlsdata.pFastBufAlloc, cnt);

			if (cnt)
				return (const wchar_t *)g_tlsdata.pFastBufAlloc;

			free(g_tlsdata.pFastBufAlloc);
			g_tlsdata.pFastBufAlloc = NULL;
		}
	}

	return L"";
}

const char *VDFastTextWToA(const wchar_t *s) {
	int cnt;

	if (!s)
		return NULL;

	cnt = WideCharToMultiByte(CP_ACP, 0, s, -1, g_tlsdata.fastBufA, sizeof g_tlsdata.fastBufA, NULL, 0);

	if (cnt)
		return g_tlsdata.fastBufA;

	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		cnt = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, 0);

		if (cnt) {
			LPSTR dst;

			if (dst = VDFastTextAllocA(cnt)) {
				cnt = WideCharToMultiByte(CP_ACP, 0, s, -1, dst, cnt, NULL, 0);

				if (cnt)
					return dst;

				VDFastTextFree();
			}
		}
	}

	return "";
}

const char *VDFastTextVprintfA(const char *format, va_list val) {
	int rv;
	const char *pRet = g_tlsdata.fastBufA;

	rv = _vsnprintf(g_tlsdata.fastBufA, sizeof g_tlsdata.fastBufA, format, val);

	if (rv < 0) {
		char *pRet2 = (char *)VDFastTextAllocA(sizeof(g_tlsdata.fastBufA) * 8);

		if (pRet2) {
			_vsnprintf(pRet2, sizeof(g_tlsdata.fastBufA) * 8, format, val);
			pRet = pRet2;
		}
	}

	return pRet;
}

const wchar_t *VDFastTextVprintfW(const wchar_t *format, va_list val) {
	int rv;
	const wchar_t *pRet = g_tlsdata.fastBufW;

	rv = _vsnwprintf(g_tlsdata.fastBufW, sizeof g_tlsdata.fastBufW / sizeof(wchar_t), format, val);

	if (rv < 0) {
		wchar_t *pRet2 = VDFastTextAllocW(sizeof(g_tlsdata.fastBufW) / sizeof(wchar_t) * 8);

		if (pRet2) {
			_vsnwprintf(pRet2, sizeof(g_tlsdata.fastBufA) * 8 / sizeof(wchar_t), format, val);
			pRet = pRet2;
		}
	}

	return pRet;
}

char *VDGetFastTextBufferA(int& maxchars) {
	maxchars = sizeof g_tlsdata.fastBufA;
	return g_tlsdata.fastBufA;
}

wchar_t *VDGetFastTextBufferW(int& maxchars) {
	maxchars = sizeof g_tlsdata.fastBufW / sizeof(wchar_t);
	return g_tlsdata.fastBufW;
}

char *VDFastTextAllocA(size_t bytes) {
	VDASSERT(!g_tlsdata.pFastBufAlloc);

	if (bytes <= sizeof g_tlsdata.fastBufA)
		return g_tlsdata.fastBufA;

	return (char *)(g_tlsdata.pFastBufAlloc = malloc(bytes));
}

wchar_t *VDFastTextAllocW(size_t chars) {
	VDASSERT(!g_tlsdata.pFastBufAlloc);

	if (chars <= sizeof g_tlsdata.fastBufW / sizeof(wchar_t))
		return g_tlsdata.fastBufW;

	return (wchar_t *)(g_tlsdata.pFastBufAlloc = malloc(chars * sizeof(wchar_t)));
}

void VDFastTextFree() {
	free(g_tlsdata.pFastBufAlloc);
	g_tlsdata.pFastBufAlloc = NULL;
}

int VDTextWToA(char *dst, int max_dst, const wchar_t *src, int max_src) {
	VDASSERTPTR(dst);
	VDASSERTPTR(src);
	VDASSERT(max_dst>0);

	*dst = 0;

	int len = WideCharToMultiByte(CP_ACP, 0, src, max_src, dst, max_dst, NULL, NULL);

	return max_src<0 && len>0 ? len-1 : len;
}

int VDTextAToW(wchar_t *dst, int max_dst, const char *src, int max_src) {
	VDASSERTPTR(dst);
	VDASSERTPTR(src);
	VDASSERT(max_dst>0);

	*dst = 0;

	int len = MultiByteToWideChar(CP_ACP, 0, src, max_src, dst, max_dst);

	return max_src<0 && len>0 ? len-1 : 0;
}

VDStringA VDTextWToA(const VDStringW& sw) {
	return VDTextWToA(sw.data(), sw.length());
}

VDStringA VDTextWToA(const wchar_t *src, int srclen) {
	VDStringA s;

	if (src) {
		int l = VDTextWToALength(src, srclen);

		if (l) {
			s.resize(l);
			VDTextWToA((char *)s.data(), l+1, src, srclen);
		}
	}

	return s;
}

VDStringW VDTextAToW(const VDStringA& s) {
	return VDTextAToW(s.data(), s.length());
}

VDStringW VDTextAToW(const char *src, int srclen) {
	VDStringW sw;

	if (src) {
		int l = VDTextAToWLength(src, srclen);

		if (l) {
			sw.resize(l);
			VDTextAToW(&sw[0], sw.length()+1, src, srclen);
		}
	}

	return sw;
}

int VDTextWToALength(const wchar_t *s, int length) {
	SetLastError(0);
	int rv = WideCharToMultiByte(CP_ACP, 0, s, length, NULL, 0, NULL, 0);

	if (length < 0 && rv>0)
		--rv;

	return rv;
}

int VDTextAToWLength(const char *s, int length) {
	SetLastError(0);
	int rv = MultiByteToWideChar(CP_ACP, 0, s, length, NULL, 0);

	if (length < 0 && rv > 0)
		--rv;

	return rv;
}

namespace {
	// UTF8:
	//      000000000xxxxxxx -> 0xxxxxxx
	//      00000yyyyyxxxxxx -> 110yyyyy 10xxxxxx
	//      zzzzyyyyyyxxxxxx -> 1110zzzz 10yyyyyy 10xxxxxx
	// uuuuuzzzzyyyyyyxxxxxx -> 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
	//               (UTF16) -> 110110wwwwzzzzyy (uuuuu = wwww+1)
	//                          110111yyyyxxxxxx
	int VDGetCharLengthInUTF8(wchar_t c) {
		if (c < 0x0080)			// 7 bits
			return 1;
		else if (c < 0x0800)	// 11 bits
			return 2;
		else if (c < 0x10000)	// 16 bits
			return 3;
		else if (c < 0x200000)	// 21 bits
			return 4;
		else {
			VDASSERT(false);
			return 1;			// Uh oh.  Well, we're screwed.
		}
	}

	bool VDIsUnicodeSurrogateFirst(wchar_t c) {
		return (c >= 0xD800 && c < 0xDC00); 
	}

	bool VDIsUnicodeSurrogateSecond(wchar_t c) {
		return (c >= 0xDC00 && c < 0xE000);
	}
};

VDStringA VDTextWToU8(const VDStringW& s) {
	return VDTextWToU8(s.data(), s.length());
}

VDStringA VDTextWToU8(const wchar_t *s, int length) {
	std::vector<char> temp;

	if (length<0) {
		const wchar_t *t = s;
		do {
			++length;
		} while(*t++);
	}

	while(length--) {
		wchar_t c = *s++;

		if (VDIsUnicodeSurrogateFirst(c)) {
			if (!length || !VDIsUnicodeSurrogateSecond(*s)) {
				VDASSERT(false);
				c = '?';
			} else {
				c = 0x10000 + ((c & 0x3ff)<<10) + (*s++ & 0x3ff);
				--length;
			}
		}

		if (c < 0x0080) {
			temp.push_back((char)c);
		} else {
			if (c < 0x0800)
				temp.push_back((char)(0xc0 + (c>>6)));
			else {
				if (c < 0x10000)
					temp.push_back((char)(0xe0 + (c>>12)));
				else {
					temp.push_back((char)(0xf0 + ((c>>18) & 0x07)));
					temp.push_back((char)(0x80 + ((c>>12) & 0x3f)));
				}
				temp.push_back((char)(0x80 + ((c>>6) & 0x3f)));
			}
			temp.push_back((char)(0x80 + (c & 0x3f)));
		}
	}

	VDStringA a(&temp[0], temp.size());

	return a;
}

VDStringW VDTextU8ToW(const VDStringA& s) {
	return VDTextU8ToW(s.data(), s.length());
}

VDStringW VDTextU8ToW(const char *s, int length) {
	std::vector<wchar_t> temp;

	if (length<0) {
		const char *t = s;
		VDASSERT(length == -1);
		do {
			++length;
		} while(*t++);
	}

	while(length--) {
		unsigned char c = (char)*s++;
		uint32	wc = c;			// we reconstruct UTF-32 first and then split to UTF-16 if necessary

		if (c >= 0x80) {
			int required_extra = 0;

			if (c < 0xc0 || c >= 0xf7) {
				VDASSERT(false);
				break;
			}

			while(c >= 0xc0) {
				c <<= 1;
				++required_extra;
			}

			wc = (c&0x3f) >> required_extra;

			do {
				char d;

				if (!length-- || (((d=*s++)&0xc0)!=0x80))
					goto bad_sequence_exit;

				wc = (wc<<6) + (d&0x3f);
			} while(--required_extra);
		}

		// Two cases here.  If we are using UTF-16, surrogates need to be split in half.  If we are using
		// UTF-32, surrogates need to be combined.

		if (sizeof(wchar_t) > 2) {
			if (VDIsUnicodeSurrogateSecond(wc)) {
				if (temp.empty() || !VDIsUnicodeSurrogateFirst(temp.back())) {
					VDASSERT(false);
					break;
				}

				temp.back() = 0x10000 + ((temp.back()&0x3ff) << 10) + (wc & 0x3ff);
				continue;
			}
		} else {
			if (wc >= 0x10000) {
				wc -= 0x10000;
				temp.push_back(0xD800 + ((wc & 0x3ff) >> 10));
				wc = 0xDC00 + (wc&0x3ff);
			}
		}
		temp.push_back(wc);
	}
bad_sequence_exit:

	VDStringW w(&temp[0], temp.size());

	return w;
}

///////////////////////////////////////////////////////////////////////////
//
//	VirtualDub's very own printf() functions.
//
//	VD[v|a]swprintf() differs from wsprintf() in the following ways:
//
//	* The output is a string.
//	* All parameters must be passed by pointer instead of by value.
//	* The 'll' modifier permits long long / __int64 integers.
//	* [n] allows picking parameters out of order.
//	* %lc/%ls forces Unicode; %hc/%hs forces ANSI.

VDStringW VDaswprintf(const wchar_t *format, int args, const void *const *argv) {
	const void *const *argv0 = argv;
	std::vector<wchar_t> out;
	wchar_t c;

	while(c = *format) {
		if (c != L'%') {
			const wchar_t *s = format;

			while(*s && *s != L'%')
				++s;

			int len = s - format;
			int clen = out.size();

			out.resize(clen + len);

			std::copy(format, s, &out[clen]);

			format = s;
		} else {
			++format;

			// check for %%

			if (*format == L'%') {
				++format;
				out.push_back(L'%');
				continue;
			}

			// Check for a renumbering identifier.

			if (*format == L'[') {
				++format;

				int newid = wcstol(format, const_cast<wchar_t **>(&format), 0);

				VDASSERT(newid >= 0 && newid < args);

				argv = argv0 + newid;

				VDASSERT(*format++ == L']');
			}

			// process flags

			struct {
				bool bLeftAlign:1,		// pad right with spaces (priority over zero pad)
					bZeroPad:1,			// pad left with zeroes
					bPositiveSign:1,	// prefix with + or -; priority over bPositiveBlank
					bPositiveBlank:1,	// prefix with space for nonnegative
					bPrefix:1;			// prefix with 0, 0x, 0X, or force decimal point
			} flags={false};
			int width = 0;
			int precision = -1;

			for(;;) {
				c = *format;

				if (c == L'0')
					flags.bZeroPad = true;
				else if (c == L' ')
					flags.bPositiveBlank = true;
				else if (c == L'#')
					flags.bPrefix = true;
				else if (c == L'-')
					flags.bLeftAlign = true;
				else if (c == L'+')
					flags.bPositiveSign = true;
				else
					break;

				++format;
			}

			// process width

			c = *format;
			if (c == L'*') {
				++format;
				width = *(int *)*argv++;
			} else if (iswdigit(c))
				width = (int)wcstol(format, const_cast<wchar_t **>(&format), 0);

			// process precision

			if (*format == L'.') {
				c = *++format;

				if (c == L'*') {
					++format;
					precision = *(int *)*argv++;
				} else if (iswdigit(c))
					precision = (int)wcstol(format, const_cast<wchar_t **>(&format), 0);
			}

			// process flags

			enum { kDefault, kLong, kLongLong, kShort } size = kDefault;

			c = *format;

			if (c == L'l') {
				++format;
				size = kLong;

				if (*format == L'l') {
					++format;
					size = kLongLong;
				}

			} else if (c == L'h') {
				++format;
				size = kShort;
			}

			// process format character

			wchar_t xf[32], buf[32], *pxf = xf, *pbuf0 = buf, *pbuf = buf;
			int zero_pad = 0;

			switch(*format++) {
			case L'd':
			case L'i':
			case L'o':
			case L'u':
			case L'x':
			case L'X':
				*pxf++ = '%';
				if (flags.bPrefix)
					*pxf++ = '#';
				if (flags.bPositiveBlank)
					*pxf++ = ' ';
				if (flags.bPositiveSign)
					*pxf++ = '+';

				switch(size) {
				case kShort:
					*pxf++ = 'h';
					*pxf++ = format[-1];
					*pxf = 0;
					pbuf += swprintf(pbuf, xf, *(const short *)*argv++);
					break;
				case kDefault:
					*pxf++ = format[-1];
					*pxf = 0;
					pbuf += swprintf(pbuf, xf, *(const int *)*argv++);
					break;
				case kLong:
					*pxf++ = 'l';
					*pxf++ = format[-1];
					*pxf = 0;
					pbuf += swprintf(pbuf, xf, *(const long *)*argv++);
					break;
				case kLongLong:
#if defined(_MSC_VER)
					*pxf++ = 'I';
					*pxf++ = '6';
					*pxf++ = '4';
#elif defined(__GNUC__)
					*pxf++ = 'l';
					*pxf++ = 'l';
#else
#error Please insert the appropriate 64-bit printf format for your platform.
#endif
					*pxf++ = format[-1];
					*pxf = 0;
					pbuf += swprintf(pbuf, xf, *(const int64 *)*argv++);
					break;
				default:
					VDNEVERHERE;
				}

				if (pbuf - pbuf0 < precision)
					zero_pad = precision - (pbuf - pbuf0);

				break;

			case L'c':
				if (size == kShort) {
					char buf[2] = {*(const char *)*argv++, 0};
					pbuf += VDTextAToW(pbuf, 4, buf);
				} else
					*pbuf++ = *(const wchar_t *)*argv++;
				break;

			case L's':
				if (size == kShort) {
					int maxlen;

					pbuf0 = VDGetFastTextBufferW(maxlen);

					pbuf = pbuf0 + VDTextAToW(pbuf0, maxlen, *(const char *const *)*argv++, precision);
				} else {
					pbuf = pbuf0 = *(wchar_t *const *)*argv++;

					while(*pbuf && precision) {
						++pbuf;
						--precision;
					}
				}
				break;

			case L'e':
			case L'E':
			case L'f':
			case L'F':
			case L'g':
			case L'G':
				// We place an artificial limit of 256 characters on the precision value.
				{
					int maxlen;

					if (precision > 256)
						precision = 256;

					pbuf0 = pbuf = VDGetFastTextBufferW(maxlen);

					*pxf++ = '%';
					if (flags.bPrefix)
						*pxf++ = '#';
					if (flags.bPositiveBlank)
						*pxf++ = ' ';
					if (flags.bPositiveSign)
						*pxf++ = '+';
					if (precision>=0) {
						*pxf++ = '.';
						*pxf++ = '*';
					}
					*pxf++ = format[-1];
					*pxf = 0;

					if (precision >= 0)
						pbuf += swprintf(pbuf, xf, precision, *(const double *)*argv++);
					else
						pbuf += swprintf(pbuf, xf, *(const double *)*argv++);
				}
				break;

			case L'n':	// no flags honored; precision ignored
				*(int *)(*argv++) = out.size();
				continue;
			case L'p':	// no flags honored; precision ignored
				pbuf += swprintf(pbuf, L"%p", *(void *const *)*argv++);
				break;

			case L'z':
				switch(*format++) {
				case L's':
					{
						int64 value;

						switch(size) {
						case kShort:	value = *(const short *)*argv++;	break;
						case kDefault:	value = *(const int *)*argv++;		break;
						case kLong:		value = *(const long *)*argv++;		break;
						case kLongLong:	value = *(const int64 *)*argv++;	break;
							break;
						default:
							VDNEVERHERE;
						}

						if (value < 0)
							*pbuf++ = L'-';
						else if (flags.bPositiveSign)
							*pbuf++ = L'+';
						else if (flags.bPositiveBlank)
							*pbuf++ = L' ';

						if (value < (VD64(10) << 10))
							pbuf += swprintf(pbuf, L"%d bytes", (int)value);
						else if (value < (VD64(10) << 20))
							pbuf += swprintf(pbuf, L"%d KB", (int)((sint32)value >> 10));
						else if (value < (VD64(10) << 30))
							pbuf += swprintf(pbuf, L"%d MB", (int)((sint32)value >> 20));
						else if (value < (VD64(10) << 40))
							pbuf += swprintf(pbuf, L"%d GB", (int)(value >> 30));
						else
							pbuf += swprintf(pbuf, L"%d TB", (int)(value >> 40));
					}

					break;
				}
				break;

			}

			int string_width = (pbuf - pbuf0) + zero_pad;
			int string_delta = width - string_width;

			if (!flags.bLeftAlign && string_delta > 0) {
				int siz = out.size();
				out.resize(siz + string_delta);
				std::fill(&out[siz], &out[siz+string_delta], flags.bZeroPad ? L'0' : L' ');
			}

			if (zero_pad) {
				int siz = out.size();
				out.resize(siz + zero_pad);
				std::fill(&out[siz], &out[siz+zero_pad], L'0');
			}

			{
				int siz = out.size();
				out.resize(siz + (pbuf - pbuf0));

				std::copy(pbuf0, pbuf, &out[siz]);
			}

			if (flags.bLeftAlign && string_delta > 0) {
				int siz = out.size();
				out.resize(siz + string_delta);
				std::fill(&out[siz], &out[siz+string_delta], L' ');
			}
		}
	}

	out.push_back(0);

	return VDStringW(&out[0]);
}

VDStringW VDvswprintf(const wchar_t *format, int args, va_list val) {
	if (args < 16) {
		const void *argv[16];

		for(int i=0; i<args; ++i)
			argv[i] = va_arg(val, const void *);

		va_end(val);

		return VDaswprintf(format, args, argv);
	} else {
		std::vector<const void *> argv(args);

		for(int i=0; i<args; ++i)
			argv.push_back(va_arg(val, const void *));

		va_end(val);

		return VDaswprintf(format, args, &argv[0]);
	}
}

VDStringW VDswprintf(const wchar_t *format, int args, ...) {
	va_list val;

	va_start(val, args);
	VDStringW r = VDvswprintf(format, args, val);
	va_end(val);

	return r;
}
