// Rayman3Vibration_DirectInputEffect.hpp
// Nolan Check
// Created 2/27/2012

#ifndef _RAYMAN3VIBRATION_DIRECTINPUTEFFECT_HPP
#define _RAYMAN3VIBRATION_DIRECTINPUTEFFECT_HPP

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class Rayman3Vibration_DirectInputEffect : public IDirectInputEffect
{

public:

	static HRESULT Create(Rayman3Vibration_DirectInputEffect** result, LPCDIEFFECT lpeff,
		int controller);

    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
    STDMETHOD_(ULONG,AddRef)(THIS);
    STDMETHOD_(ULONG,Release)(THIS);

    /*** IDirectInputEffect methods ***/
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID);
    STDMETHOD(GetEffectGuid)(THIS_ LPGUID);
    STDMETHOD(GetParameters)(THIS_ LPDIEFFECT,DWORD);
    STDMETHOD(SetParameters)(THIS_ LPCDIEFFECT,DWORD);
    STDMETHOD(Start)(THIS_ DWORD,DWORD);
    STDMETHOD(Stop)(THIS);
    STDMETHOD(GetEffectStatus)(THIS_ LPDWORD);
    STDMETHOD(Download)(THIS);
    STDMETHOD(Unload)(THIS);
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE);

private:

	Rayman3Vibration_DirectInputEffect();
	~Rayman3Vibration_DirectInputEffect();

	HRESULT CreateInternal(LPCDIEFFECT lpeff, int controller);

	ULONG m_refCount;
	DIEFFECT m_diEffect;
	int m_controller;

};

#endif
