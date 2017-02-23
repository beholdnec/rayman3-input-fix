// Rayman3XInput_DirectInputDevice8A.hpp
// Nolan Check
// Created 2/25/2012

#ifndef _RAYMAN3XINPUT_DIRECTINPUTDEVICE8A_HPP
#define _RAYMAN3XINPUT_DIRECTINPUTDEVICE8A_HPP

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>

#include <list>
#include <map>
#include <memory>
#include <vector>

class Rayman3XInput_DirectInputDevice8A : public IDirectInputDevice8A
{

public:

	static HRESULT Create(Rayman3XInput_DirectInputDevice8A** ppvOut, int controller);
	
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDirectInputDevice8A
	STDMETHOD(GetCapabilities)(LPDIDEVCAPS lpDIDevCaps);
	STDMETHOD(EnumObjects)(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
	STDMETHOD(GetProperty)(REFGUID rguidProp, LPDIPROPHEADER pdiph);
	STDMETHOD(SetProperty)(REFGUID rguidProp, LPCDIPROPHEADER pdiph);
	STDMETHOD(Acquire)();
	STDMETHOD(Unacquire)();
	STDMETHOD(GetDeviceState)(DWORD cbData, LPVOID lpvData);
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD);
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT);
    STDMETHOD(SetEventNotification)(THIS_ HANDLE);
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD);
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD);
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA);
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD);
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID);
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN);
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA,LPVOID,DWORD);
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA,REFGUID);
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD);
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD);
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD);
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE);
    STDMETHOD(Poll)(THIS);
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD);
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD);
    STDMETHOD(WriteEffectToFile)(THIS_ LPCSTR,DWORD,LPDIFILEEFFECT,DWORD);
    STDMETHOD(BuildActionMap)(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD);
    STDMETHOD(SetActionMap)(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD);
    STDMETHOD(GetImageInfo)(THIS_ LPDIDEVICEIMAGEINFOHEADERA);

private:

	Rayman3XInput_DirectInputDevice8A();
	~Rayman3XInput_DirectInputDevice8A();

	HRESULT CreateInternal(int controller);

	ULONG m_refCount;
	int m_controller;
	XINPUT_STATE m_controllerState;

	class Control
	{
	public:
		virtual ~Control() { }
		virtual bool IsPresent() = 0;
		virtual GUID GetGUID() = 0;
		virtual bool IsTypeCompatible(DWORD dwType) = 0;
		virtual void GetRange(LPDIPROPRANGE prop) = 0;
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) = 0;
	};

	typedef std::shared_ptr<Control> ControlPtr;

	typedef std::list<ControlPtr> ControlList;
	typedef std::map<DWORD, ControlPtr> AssignedControlMap; // Key is data format offset
	ControlList m_availableControls;
	AssignedControlMap m_assignedControls;

	void ResetControls();
	void AssignZeroControl(DWORD offset, DWORD dwType);

};

#endif
