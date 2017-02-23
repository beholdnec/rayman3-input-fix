// Rayman3Vibration_DirectInputEffect.cpp
// Nolan Check
// Created 2/27/2012

#include "Rayman3Vibration_DirectInputEffect.hpp"

#include <Xinput.h>

#include "WindowsUtils.hpp"

HRESULT Rayman3Vibration_DirectInputEffect::Create(Rayman3Vibration_DirectInputEffect** out,
	LPCDIEFFECT lpeff, int controller)
{
	HRESULT hr;

	*out = NULL;

	Rayman3Vibration_DirectInputEffect* result = new Rayman3Vibration_DirectInputEffect;
	if (result)
	{
		hr = result->CreateInternal(lpeff, controller);
		if (FAILED(hr)) {
			result->Release();
		} else {
			*out = result;
		}
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}

	return hr;
}

Rayman3Vibration_DirectInputEffect::Rayman3Vibration_DirectInputEffect()
	: m_refCount(1)
{ }

Rayman3Vibration_DirectInputEffect::~Rayman3Vibration_DirectInputEffect()
{ }

HRESULT Rayman3Vibration_DirectInputEffect::CreateInternal(LPCDIEFFECT lpeff, int controller)
{
	m_controller = controller;

	if (lpeff)
	{
		m_diEffect = *lpeff;
	}
	else
	{
		// Default effect
		memset(&m_diEffect, 0, sizeof(DIEFFECT));
		m_diEffect.dwSize = sizeof(DIEFFECT);
		m_diEffect.dwDuration = 500000; // Half a second
		m_diEffect.dwGain = 10000;
	}

	return DI_OK;
}

HRESULT Rayman3Vibration_DirectInputEffect::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	IUnknown* pUnk = NULL;
	if (riid == IID_IUnknown) {
		pUnk = this;
	} else if (riid == IID_IDirectInputEffect) {
		pUnk = this;
	}

	*ppvObj = pUnk;
	if (pUnk)
	{
		pUnk->AddRef();
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

ULONG Rayman3Vibration_DirectInputEffect::AddRef(THIS)
{
	return InterlockedIncrement(&m_refCount);
}

ULONG Rayman3Vibration_DirectInputEffect::Release(THIS)
{
	ULONG c = InterlockedDecrement(&m_refCount);
	if (c == 0)
	{
		delete this;
	}
	return c;
}

HRESULT Rayman3Vibration_DirectInputEffect::Initialize(THIS_ HINSTANCE,DWORD,REFGUID)
{
	Debug("R3V: Initialize called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3Vibration_DirectInputEffect::GetEffectGuid(THIS_ LPGUID)
{
	Debug("R3V: GetEffectGuid called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3Vibration_DirectInputEffect::GetParameters(THIS_ LPDIEFFECT,DWORD)
{
	Debug("R3V: GetParameters called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3Vibration_DirectInputEffect::SetParameters(THIS_ LPCDIEFFECT,DWORD)
{
	Debug("R3V: SetParameters called\n");
	return E_NOTIMPL;
}

HRESULT Rayman3Vibration_DirectInputEffect::Start(DWORD dwIterations, DWORD dwFlags)
{
	Debug("R3V: Start called\n");
	Debug("Iterations: %d\n", dwIterations);

	// TODO: Pay attention to other effect parameters, like duration.
	// It may be necessary to control vibration on another thread.
	XINPUT_VIBRATION xv;
	xv.wLeftMotorSpeed = (WORD)Lerp(0.0f, 10000.0f, 0.0f, 65535.0f, (float)m_diEffect.dwGain);
	xv.wRightMotorSpeed = 0;
	XInputSetState(m_controller, &xv);

	return DI_OK;
}

HRESULT Rayman3Vibration_DirectInputEffect::Stop(THIS)
{
	Debug("R3V: Stop called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3Vibration_DirectInputEffect::GetEffectStatus(THIS_ LPDWORD)
{
	Debug("R3V: GetEffectStatus called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3Vibration_DirectInputEffect::Download(THIS)
{
	Debug("R3V: Download called\n");
	return E_NOTIMPL;
}

HRESULT Rayman3Vibration_DirectInputEffect::Unload()
{
	Debug("R3V: Unload called\n");

	XINPUT_VIBRATION xv;
	xv.wLeftMotorSpeed = 0;
	xv.wRightMotorSpeed = 0;
	XInputSetState(m_controller, &xv);

	return DI_OK;
}

HRESULT Rayman3Vibration_DirectInputEffect::Escape(THIS_ LPDIEFFESCAPE)
{
	Debug("R3V: Escape called\n");
	return E_NOTIMPL;
}
