//	VDXFrame - Helper library for VirtualDub plugins
//	Copyright (C) 2008 Avery Lee
//
//	The plugin headers in the VirtualDub plugin SDK are licensed differently
//	differently than VirtualDub and the Plugin SDK themselves.  This
//	particular file is thus licensed as follows (the "zlib" license):
//
//	This software is provided 'as-is', without any express or implied
//	warranty.  In no event will the authors be held liable for any
//	damages arising from the use of this software.
//
//	Permission is granted to anyone to use this software for any purpose,
//	including commercial applications, and to alter it and redistribute it
//	freely, subject to the following restrictions:
//
//	1.	The origin of this software must not be misrepresented; you must
//		not claim that you wrote the original software. If you use this
//		software in a product, an acknowledgment in the product
//		documentation would be appreciated but is not required.
//	2.	Altered source versions must be plainly marked as such, and must
//		not be misrepresented as being the original software.
//	3.	This notice may not be removed or altered from any source
//		distribution.

#include "stdafx.h"
#include <vd2/VDXFrame/VideoFilter.h>

///////////////////////////////////////////////////////////////////////////

VDXVideoFilter::VDXVideoFilter() {
}

VDXVideoFilter::~VDXVideoFilter() {
}

void VDXVideoFilter::SetHooks(VDXFilterActivation *fa, const VDXFilterFunctions *ff) {
	this->fa = fa;
	this->ff = ff;
}

///////////////////////////////////////////////////////////////////////////

bool VDXVideoFilter::Init() {
	return true;
}

void VDXVideoFilter::Start() {
}

void VDXVideoFilter::End() {
}

bool VDXVideoFilter::Configure(VDXHWND hwnd) {
	return hwnd != NULL;
}

void VDXVideoFilter::GetSettingString(char *buf, int maxlen) {
}

void VDXVideoFilter::GetScriptString(char *buf, int maxlen) {
}

int VDXVideoFilter::Serialize(char *buf, int maxbuf) {
	return 0;
}

int VDXVideoFilter::Deserialize(const char *buf, int maxbuf) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////

void __cdecl VDXVideoFilter::FilterDeinit   (VDXFilterActivation *fa, const VDXFilterFunctions *ff) {
	(*reinterpret_cast<VDXVideoFilter **>(fa->filter_data))->~VDXVideoFilter();
}

int  __cdecl VDXVideoFilter::FilterRun      (const VDXFilterActivation *fa, const VDXFilterFunctions *ff) {
	VDXVideoFilter *pThis = *reinterpret_cast<VDXVideoFilter **>(fa->filter_data);

	pThis->fa		= const_cast<VDXFilterActivation *>(fa);

	pThis->Run();
	return 0;
}

long __cdecl VDXVideoFilter::FilterParam    (VDXFilterActivation *fa, const VDXFilterFunctions *ff) {
	VDXVideoFilter *pThis = *reinterpret_cast<VDXVideoFilter **>(fa->filter_data);

	pThis->fa		= fa;

	return pThis->GetParams();
}

int  __cdecl VDXVideoFilter::FilterConfig   (VDXFilterActivation *fa, const VDXFilterFunctions *ff, VDXHWND hwnd) {
	VDXVideoFilter *pThis = *reinterpret_cast<VDXVideoFilter **>(fa->filter_data);

	pThis->fa		= fa;

	return !pThis->Configure(hwnd);
}

int  __cdecl VDXVideoFilter::FilterStart    (VDXFilterActivation *fa, const VDXFilterFunctions *ff) {
	VDXVideoFilter *pThis = *reinterpret_cast<VDXVideoFilter **>(fa->filter_data);

	pThis->fa		= fa;

	pThis->Start();
	return 0;
}

int  __cdecl VDXVideoFilter::FilterEnd      (VDXFilterActivation *fa, const VDXFilterFunctions *ff) {
	VDXVideoFilter *pThis = *reinterpret_cast<VDXVideoFilter **>(fa->filter_data);

	pThis->fa		= fa;

	pThis->End();
	return 0;
}

void __cdecl VDXVideoFilter::FilterString   (const VDXFilterActivation *fa, const VDXFilterFunctions *ff, char *buf) {
	VDXVideoFilter *pThis = *reinterpret_cast<VDXVideoFilter **>(fa->filter_data);

	pThis->fa		= const_cast<VDXFilterActivation *>(fa);

	pThis->GetScriptString(buf, 80);
}

bool __cdecl VDXVideoFilter::FilterScriptStr(VDXFilterActivation *fa, const VDXFilterFunctions *ff, char *buf, int buflen) {
	VDXVideoFilter *pThis = *reinterpret_cast<VDXVideoFilter **>(fa->filter_data);

	pThis->fa		= fa;

	pThis->GetScriptString(buf, buflen);

	return true;
}

void __cdecl VDXVideoFilter::FilterString2  (const VDXFilterActivation *fa, const VDXFilterFunctions *ff, char *buf, int maxlen) {
	VDXVideoFilter *pThis = *reinterpret_cast<VDXVideoFilter **>(fa->filter_data);

	pThis->fa		= const_cast<VDXFilterActivation *>(fa);

	pThis->GetSettingString(buf, maxlen);
}

int  __cdecl VDXVideoFilter::FilterSerialize    (VDXFilterActivation *fa, const VDXFilterFunctions *ff, char *buf, int maxbuf) {
	VDXVideoFilter *pThis = *reinterpret_cast<VDXVideoFilter **>(fa->filter_data);

	pThis->fa		= fa;

	return pThis->Serialize(buf, maxbuf);
}

void __cdecl VDXVideoFilter::FilterDeserialize  (VDXFilterActivation *fa, const VDXFilterFunctions *ff, const char *buf, int maxbuf) {
	VDXVideoFilter *pThis = *reinterpret_cast<VDXVideoFilter **>(fa->filter_data);

	pThis->fa		= fa;

	pThis->Deserialize(buf, maxbuf);
}

void VDXVideoFilter::SafePrintf(char *buf, int maxbuf, const char *format, ...) {
	if (maxbuf <= 0)
		return;

	va_list val;
	va_start(val, format);
	if ((unsigned)_vsnprintf(buf, maxbuf, format, val) >= (unsigned)maxbuf)
		buf[maxbuf - 1] = 0;
	va_end(val);
}

const VDXScriptFunctionDef VDXVideoFilter::sScriptMethods[1]={0};
