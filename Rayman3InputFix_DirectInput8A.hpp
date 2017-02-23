// Rayman3InputFix_DirectInput8A.hpp
// Nolan Check
// Created 2/25/2012

#ifndef _RAYMAN3INPUTFIX_DIRECTINPUT8A_HPP
#define _RAYMAN3INPUTFIX_DIRECTINPUT8A_HPP

#include <InitGuid.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class Rayman3InputFix_DirectInput8A : public IDirectInput8A
{

public:

	static HRESULT Create(REFIID riid, void** ppvObject, HINSTANCE hInstance);

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDirectInput8A
	STDMETHOD(CreateDevice)(REFGUID rguid, LPDIRECTINPUTDEVICE8A* lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
	STDMETHOD(EnumDevices)(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
	STDMETHOD(GetDeviceStatus)(REFGUID rguidInstance);
	STDMETHOD(RunControlPanel)(HWND hwndOwner, DWORD dwFlags);
	STDMETHOD(Initialize)(HINSTANCE hinst, DWORD dwVersion);
	STDMETHOD(FindDevice)(REFGUID rguidClass, LPCSTR ptszName, LPGUID pguidInstance);
	STDMETHOD(EnumDevicesBySemantics)(LPCSTR ptszUserName, LPDIACTIONFORMATA lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCBA lpCallback, LPVOID pvRef, DWORD dwFlags);
	STDMETHOD(ConfigureDevices)(LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMSA lpdiCDParams, DWORD dwFlags, LPVOID pvRefData);

private:

	Rayman3InputFix_DirectInput8A();
	~Rayman3InputFix_DirectInput8A();

	HRESULT CreateInternal(HINSTANCE hInstance);

	ULONG m_refCount;
	HMODULE m_realDInput8Dll;
	IDirectInput8A* m_realDirectInput;

};

#endif
