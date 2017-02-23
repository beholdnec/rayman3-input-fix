// Main.cpp
// Nolan Check
// Created 2/25/2012

#include "Rayman3InputFix_DirectInput8A.hpp"
#include "WindowsUtils.hpp"

HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion,
	REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
	HRESULT hr;

  Debug("DirectInput8Create called.\n");

	if (riidltf == IID_IDirectInput8A)
	{
		hr = Rayman3InputFix_DirectInput8A::Create(riidltf, ppvOut, hinst);
		if (SUCCEEDED(hr))
		{
			IDirectInput8A* p = (IDirectInput8A*)*ppvOut;
			hr = p->Initialize(hinst, dwVersion);
		}
	}
	else
	{
		hr = DIERR_INVALIDPARAM;
	}

	return hr;
}
