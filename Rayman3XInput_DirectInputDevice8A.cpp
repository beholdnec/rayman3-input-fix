// Rayman3XInput_DirectInputDevice8A.cpp
// Nolan Check
// Created 2/25/2012

#include "Rayman3XInput_DirectInputDevice8A.hpp"

#include "Rayman3Vibration_DirectInputEffect.hpp"
#include "WindowsUtils.hpp"

#include <Xinput.h>

HRESULT Rayman3XInput_DirectInputDevice8A::Create(Rayman3XInput_DirectInputDevice8A** ppvOut, int controller)
{
	HRESULT hr;

	*ppvOut = NULL;

	Rayman3XInput_DirectInputDevice8A* result = new Rayman3XInput_DirectInputDevice8A;
	if (result)
	{
		hr = result->CreateInternal(controller);
		if (FAILED(hr)) {
			result->Release();
		} else {
			*ppvOut = result;
		}
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}

	return hr;
}

Rayman3XInput_DirectInputDevice8A::Rayman3XInput_DirectInputDevice8A()
	: m_refCount(1)
{
	ResetControls();
}

Rayman3XInput_DirectInputDevice8A::~Rayman3XInput_DirectInputDevice8A()
{ }

HRESULT Rayman3XInput_DirectInputDevice8A::CreateInternal(int controller)
{
	m_controller = controller;
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::QueryInterface(REFIID riid, void** ppvObject)
{
	IUnknown* pUnk = NULL;
	if (riid == IID_IUnknown) {
		pUnk = this;
	} else if (riid == IID_IDirectInputDevice8A) {
		pUnk = this;
	}

	*ppvObject = pUnk;
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

ULONG Rayman3XInput_DirectInputDevice8A::AddRef()
{
	return InterlockedIncrement(&m_refCount);
}

ULONG Rayman3XInput_DirectInputDevice8A::Release()
{
	ULONG c = InterlockedDecrement(&m_refCount);
	if (c == 0)
	{
		delete this;
	}
	return c;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetCapabilities(LPDIDEVCAPS lpDIDevCaps)
{
	Debug("R3XI: GetCapabilities called\n");

	if (lpDIDevCaps->dwSize != sizeof(DIDEVCAPS)) {
		return DIERR_NOTINITIALIZED;
	}

	lpDIDevCaps->dwFlags = DIDC_ATTACHED | DIDC_EMULATED | DIDC_POLLEDDATAFORMAT
		| DIDC_POLLEDDEVICE | DIDC_FORCEFEEDBACK;
	lpDIDevCaps->dwDevType = DI8DEVTYPE_GAMEPAD | (DI8DEVTYPEGAMEPAD_STANDARD << 8);
	lpDIDevCaps->dwAxes = 4;
	lpDIDevCaps->dwButtons = 12; // The left and right triggers are treated as buttons
	lpDIDevCaps->dwPOVs = 1;
	lpDIDevCaps->dwFFSamplePeriod = 0;
	lpDIDevCaps->dwFFMinTimeResolution = 0;
	lpDIDevCaps->dwFirmwareRevision = 0;
	lpDIDevCaps->dwHardwareRevision = 0;
	lpDIDevCaps->dwFFDriverVersion = 0;

	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Debug("R3XI: EnumObjects called\n");
	return E_NOTIMPL;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	Debug("R3XI: GetProperty called\n");
	if (&rguidProp == &DIPROP_RANGE)
	{
		if (pdiph->dwSize != sizeof(DIPROPRANGE) ||
			pdiph->dwHeaderSize != sizeof(DIPROPHEADER) ||
			pdiph->dwHow != DIPH_BYOFFSET)
		{
			Debug("Invalid DIPROP_RANGE pdiph\n");
			return DIERR_INVALIDPARAM;
		}

		Debug("Querying DIPROP_RANGE for object at offset %d\n", pdiph->dwObj);

		AssignedControlMap::const_iterator it = m_assignedControls.find(pdiph->dwObj);
		if (it == m_assignedControls.end())
		{
			Debug("No assigned control at offset\n");
			return DIERR_OBJECTNOTFOUND;
		}

		LPDIPROPRANGE prop = (LPDIPROPRANGE)pdiph;
		if (it->second->IsPresent())
		{
			it->second->GetRange(prop);
			return DI_OK;
		}
		else
		{
			prop->lMin = 0;
			prop->lMax = 0;
			return DIERR_OBJECTNOTFOUND;
		}
	}
	else
	{
		Debug("Unsupported property %p\n", &rguidProp);
		return DIERR_UNSUPPORTED;
	}
}

HRESULT Rayman3XInput_DirectInputDevice8A::SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	Debug("R3XI: SetProperty called\n");
	if (&rguidProp == &DIPROP_DEADZONE)
	{
		if (pdiph->dwSize != sizeof(DIPROPDWORD) ||
			pdiph->dwHeaderSize != sizeof(DIPROPHEADER) ||
			pdiph->dwObj != 0 ||
			pdiph->dwHow != DIPH_DEVICE)
		{
			Debug("Invalid DIPROP_DEADZONE pdiph\n");
			return DIERR_INVALIDPARAM;
		}

		LPDIPROPDWORD prop = (LPDIPROPDWORD)pdiph;
		Debug("DIPROP_DEADZONE set to %d\n", prop->dwData);
		return DI_OK;
	}
	else
	{
		Debug("Unsupported property %p\n", &rguidProp);
		return DIERR_UNSUPPORTED;
	}
}

HRESULT Rayman3XInput_DirectInputDevice8A::Acquire()
{
	Debug("R3XI: Acquire called\n");
	XInputGetState(m_controller, &m_controllerState);
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::Unacquire()
{
	Debug("R3XI: Unacquire called\n");
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	//Debug("R3XI: GetDeviceState called\n");

	BYTE* data = (BYTE*)lpvData;
	for (AssignedControlMap::const_iterator it = m_assignedControls.begin();
		it != m_assignedControls.end(); ++it)
	{
		it->second->GetState(&data[it->first], m_controllerState);
	}

	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetDeviceData(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD)
{
	Debug("R3XI: GetDeviceData called\n");
	return E_NOTIMPL;
}

HRESULT Rayman3XInput_DirectInputDevice8A::SetDataFormat(LPCDIDATAFORMAT lpdf)
{
	Debug("R3XI: SetDataFormat called\n");

	ResetControls();

	if (lpdf->dwSize != sizeof(DIDATAFORMAT) ||
		lpdf->dwObjSize != sizeof(DIOBJECTDATAFORMAT))
	{
		Debug("SetDataFormat called with invalid lpdf\n");
		return DIERR_INVALIDPARAM;
	}

	for (DWORD i = 0; i < lpdf->dwNumObjs; ++i)
	{
		LPDIOBJECTDATAFORMAT odf = &lpdf->rgodf[i];

		Debug("Object %d:\n", i);

		if (odf->pguid == NULL) {
			Debug("  GUID: NULL\n");
		} else {
			OLECHAR* guidStr;
			StringFromCLSID(*odf->pguid, &guidStr);
			Debug("  GUID: %S\n", guidStr);
			CoTaskMemFree(guidStr);
		}

		Debug("  Offset: %d\n", odf->dwOfs);
		Debug("  Type: 0x%.08X\n", odf->dwType);
		Debug("  Flags: 0x%.08X\n", odf->dwFlags);

		if (odf->pguid != NULL)
		{
			// Find first available control with matching GUID
			ControlList::const_iterator it;
			for (it = m_availableControls.begin(); it != m_availableControls.end(); ++it)
			{
				if ((*it)->GetGUID() == *odf->pguid) {
					break;
				}
			}

			if (it != m_availableControls.end() &&
				(*it)->IsTypeCompatible(DIDFT_GETTYPE(odf->dwType)))
			{
				// Assign the control to the offset
				Debug("Control assigned\n");
				m_assignedControls[odf->dwOfs] = *it;
				m_availableControls.erase(it);
			}
			else
			{
				// No matching control found. Assign a "zero" control.
				Debug("No control assigned; assigning zero control\n");
				AssignZeroControl(odf->dwOfs, DIDFT_GETTYPE(odf->dwType));
			}
		}
		else
		{
			// Find first available control compatible with type
			ControlList::const_iterator it;
			for (it = m_availableControls.begin(); it != m_availableControls.end(); ++it)
			{
				if ((*it)->IsTypeCompatible(DIDFT_GETTYPE(odf->dwType))) {
					break;
				}
			}

			if (it != m_availableControls.end())
			{
				Debug("Control assigned\n");
				// Assign the control to the offset
				m_assignedControls[odf->dwOfs] = *it;
				m_availableControls.erase(it);
			}
			else
			{
				Debug("No control assigned; assigning zero control\n");
				// No matching control found. Assign a "zero" control.
				AssignZeroControl(odf->dwOfs, DIDFT_GETTYPE(odf->dwType));
			}
		}
	}

	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::SetEventNotification(THIS_ HANDLE)
{
	Debug("R3XI: SetEventNotification called\n");
	return E_NOTIMPL;
}

HRESULT Rayman3XInput_DirectInputDevice8A::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	Debug("R3XI: SetCooperativeLevel called\n");
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetObjectInfo(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD)
{
	Debug("R3XI: GetObjectInfo called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::GetDeviceInfo(THIS_ LPDIDEVICEINSTANCEA)
{
	Debug("R3XI: GetDeviceInfo called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::RunControlPanel(THIS_ HWND,DWORD)
{
	Debug("R3XI: RunControlPanel called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::Initialize(THIS_ HINSTANCE,DWORD,REFGUID)
{
	Debug("R3XI: Initialize called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT* ppdeff, LPUNKNOWN punkOuter)
{
	Debug("R3XI: CreateEffect called\n");

	OLECHAR* guidStr;
	StringFromCLSID(rguid, &guidStr);
	Debug("Effect GUID: %S\n", guidStr);
	CoTaskMemFree(guidStr);

	if (rguid == GUID_ConstantForce)
	{
		Rayman3Vibration_DirectInputEffect* effect = NULL;
		HRESULT hr = Rayman3Vibration_DirectInputEffect::Create(&effect, lpeff, m_controller);
		*ppdeff = effect;
		return hr;
	}
	else
	{
		Debug("Unknown effect GUID\n");
		return DIERR_INVALIDPARAM;
	}
}

HRESULT Rayman3XInput_DirectInputDevice8A::EnumEffects(LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType)
{
	Debug("R3XI: EnumEffects called\n");
	if (dwEffType == DIEFT_ALL || dwEffType == DIEFT_CONSTANTFORCE)
	{
		Debug("Enumerated constant force effect\n");
		DIEFFECTINFOA ei = { 0 };
		ei.dwSize = sizeof(DIEFFECTINFOA);
		ei.guid = GUID_ConstantForce;
		ei.dwEffType = DIEFT_CONSTANTFORCE;
		ei.dwStaticParams = DIEP_GAIN;
		ei.dwDynamicParams = DIEP_GAIN;
		strcpy_s(ei.tszName, "Vibration");
		lpCallback(&ei, pvRef);
	}
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetEffectInfo(THIS_ LPDIEFFECTINFOA,REFGUID)
{
	Debug("R3XI: GetEffectInfo called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::GetForceFeedbackState(THIS_ LPDWORD)
{
	Debug("R3XI: GetForceFeedbackState called\n");
	return E_NOTIMPL;
}

HRESULT Rayman3XInput_DirectInputDevice8A::SendForceFeedbackCommand(DWORD dwFlags)
{
	Debug("R3XI: SendForceFeedbackCommand called\n");
	if (dwFlags == DISFFC_PAUSE)
	{
		XINPUT_VIBRATION xv;
		xv.wLeftMotorSpeed = 0;
		xv.wRightMotorSpeed = 0;
		XInputSetState(m_controller, &xv);
	}
	else if (dwFlags == DISFFC_CONTINUE)
	{
		// TODO: Implement
	} else {
		Debug("Unknown force feedback command 0x%X\n", dwFlags);
	}
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::EnumCreatedEffectObjects(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD)
{
	Debug("R3XI: EnumCreatedEffectObjects called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::Escape(THIS_ LPDIEFFESCAPE)
{
	Debug("R3XI: Escape called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::Poll(THIS)
{
	//Debug("R3XI: Poll called\n");
	if (XInputGetState(m_controller, &m_controllerState) == ERROR_SUCCESS) {
		return DI_OK;
	} else {
		return DIERR_INPUTLOST;
	}
}
HRESULT Rayman3XInput_DirectInputDevice8A::SendDeviceData(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD)
{
	Debug("R3XI: SendDeviceData called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::EnumEffectsInFile(THIS_ LPCSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD)
{
	Debug("R3XI: EnumEffectsInFile called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::WriteEffectToFile(THIS_ LPCSTR,DWORD,LPDIFILEEFFECT,DWORD)
{
	Debug("R3XI: WriteEffectToFile called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::BuildActionMap(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD)
{
	Debug("R3XI: BuildActionMap called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::SetActionMap(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD)
{
	Debug("R3XI: SetActionMap called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::GetImageInfo(THIS_ LPDIDEVICEIMAGEINFOHEADERA)
{
	Debug("R3XI: GetImageInfo called\n");
	return E_NOTIMPL;
}

void Rayman3XInput_DirectInputDevice8A::ResetControls()
{
	m_availableControls.clear();
	m_assignedControls.clear();

	class ThumbLXControl : public Control
	{
		virtual bool IsPresent() { return true; }
		virtual GUID GetGUID() {
			return GUID_XAxis;
		}
		virtual bool IsTypeCompatible(DWORD dwType) {
			return dwType == DIDFT_AXIS;
		}
		virtual void GetRange(LPDIPROPRANGE prop) {
			prop->lMin = -32768;
			prop->lMax = 32767;
		}
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) {
			*(LONG*)dst = state.Gamepad.sThumbLX;
		}
	};

	class ThumbLYControl : public Control
	{
		virtual bool IsPresent() { return true; }
		virtual GUID GetGUID() {
			return GUID_YAxis;
		}
		virtual bool IsTypeCompatible(DWORD dwType) {
			return dwType == DIDFT_AXIS;
		}
		virtual void GetRange(LPDIPROPRANGE prop) {
			prop->lMin = -32767;
			prop->lMax = 32768;
		}
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) {
			// Axis is inverted from what DirectInput expects
			*(LONG*)dst = -state.Gamepad.sThumbLY;
		}
	};

	class ThumbRXControl : public Control
	{
		virtual bool IsPresent() { return true; }
		virtual GUID GetGUID() {
			return GUID_RxAxis;
		}
		virtual bool IsTypeCompatible(DWORD dwType) {
			return dwType == DIDFT_AXIS;
		}
		virtual void GetRange(LPDIPROPRANGE prop) {
			prop->lMin = -32768;
			prop->lMax = 32767;
		}
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) {
			*(LONG*)dst = state.Gamepad.sThumbRX;
		}
	};

	class ThumbRYControl : public Control
	{
		virtual bool IsPresent() { return true; }
		virtual GUID GetGUID() {
			return GUID_RyAxis;
		}
		virtual bool IsTypeCompatible(DWORD dwType) {
			return dwType == DIDFT_AXIS;
		}
		virtual void GetRange(LPDIPROPRANGE prop) {
			prop->lMin = -32767;
			prop->lMax = 32768;
		}
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) {
			// Axis is inverted from what DirectInput expects
			*(LONG*)dst = -state.Gamepad.sThumbRY;
		}
	};

	class ButtonControl : public Control
	{
	public:
		virtual bool IsPresent() { return true; }
		ButtonControl(WORD buttonMask)
			: m_buttonMask(buttonMask)
		{ }
		virtual GUID GetGUID() {
			return GUID_Button;
		}
		virtual bool IsTypeCompatible(DWORD dwType) {
			return dwType == DIDFT_BUTTON;
		}
		virtual void GetRange(LPDIPROPRANGE prop) {
			prop->lMin = 0;
			prop->lMax = 0x80;
		}
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) {
			*(BYTE*)dst = (state.Gamepad.wButtons & m_buttonMask) ? 0x80 : 0;
		}
	private:
		WORD m_buttonMask;
	};

	class LTriggerControl : public Control
	{
		virtual bool IsPresent() { return true; }
		virtual GUID GetGUID() {
			return GUID_Button;
		}
		virtual bool IsTypeCompatible(DWORD dwType) {
			return dwType == DIDFT_BUTTON;
		}
		virtual void GetRange(LPDIPROPRANGE prop) {
			prop->lMin = 0;
			prop->lMax = 0x80;
		}
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) {
			*(BYTE*)dst = (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? 0x80 : 0;
		}
	};

	class RTriggerControl : public Control
	{
		virtual bool IsPresent() { return true; }
		virtual GUID GetGUID() {
			return GUID_Button;
		}
		virtual bool IsTypeCompatible(DWORD dwType) {
			return dwType == DIDFT_BUTTON;
		}
		virtual void GetRange(LPDIPROPRANGE prop) {
			prop->lMin = 0;
			prop->lMax = 0x80;
		}
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) {
			*(BYTE*)dst = (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? 0x80 : 0;
		}
	};

	m_availableControls.push_back(std::shared_ptr<Control>(new ThumbLXControl));
	m_availableControls.push_back(std::shared_ptr<Control>(new ThumbLYControl));
	m_availableControls.push_back(std::shared_ptr<Control>(new ThumbRXControl));
	m_availableControls.push_back(std::shared_ptr<Control>(new ThumbRYControl));
	// List button controls in their numbered order
	m_availableControls.push_back(std::shared_ptr<Control>(new ButtonControl(
		XINPUT_GAMEPAD_A)));
	m_availableControls.push_back(std::shared_ptr<Control>(new ButtonControl(
		XINPUT_GAMEPAD_B)));
	m_availableControls.push_back(std::shared_ptr<Control>(new ButtonControl(
		XINPUT_GAMEPAD_X)));
	m_availableControls.push_back(std::shared_ptr<Control>(new ButtonControl(
		XINPUT_GAMEPAD_Y)));
	m_availableControls.push_back(std::shared_ptr<Control>(new ButtonControl(
		XINPUT_GAMEPAD_LEFT_SHOULDER)));
	m_availableControls.push_back(std::shared_ptr<Control>(new ButtonControl(
		XINPUT_GAMEPAD_RIGHT_SHOULDER)));
	m_availableControls.push_back(std::shared_ptr<Control>(new ButtonControl(
		XINPUT_GAMEPAD_BACK)));
	m_availableControls.push_back(std::shared_ptr<Control>(new ButtonControl(
		XINPUT_GAMEPAD_START)));
	m_availableControls.push_back(std::shared_ptr<Control>(new ButtonControl(
		XINPUT_GAMEPAD_LEFT_THUMB)));
	m_availableControls.push_back(std::shared_ptr<Control>(new ButtonControl(
		XINPUT_GAMEPAD_RIGHT_THUMB)));
	m_availableControls.push_back(std::shared_ptr<Control>(new LTriggerControl));
	m_availableControls.push_back(std::shared_ptr<Control>(new RTriggerControl));
}

void Rayman3XInput_DirectInputDevice8A::AssignZeroControl(DWORD offset, DWORD dwType)
{
	class ZeroAxisControl : public Control
	{
	public:
		virtual bool IsPresent() { return false; }
		virtual GUID GetGUID() {
			return GUID_ZAxis;
		}
		virtual bool IsTypeCompatible(DWORD dwType) {
			return dwType == DIDFT_AXIS;
		}
		virtual void GetRange(LPDIPROPRANGE prop) {
			prop->lMin = -1000;
			prop->lMax = 1000;
		}
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) {
			*(DWORD*)dst = 0;
		}
	};

	class ZeroButtonControl : public Control
	{
	public:
		virtual bool IsPresent() { return false; }
		virtual GUID GetGUID() {
			return GUID_Button;
		}
		virtual bool IsTypeCompatible(DWORD dwType) {
			return dwType == DIDFT_BUTTON;
		}
		virtual void GetRange(LPDIPROPRANGE prop) {
			prop->lMin = 0;
			prop->lMax = 1;
		}
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) {
			*(BYTE*)dst = 0;
		}
	};

	class ZeroPovControl : public Control
	{
	public:
		virtual bool IsPresent() { return false; }
		virtual GUID GetGUID() {
			return GUID_POV;
		}
		virtual bool IsTypeCompatible(DWORD dwType) {
			return dwType == DIDFT_POV;
		}
		virtual void GetRange(LPDIPROPRANGE prop) {
			// Value is -1 for center or position in hundredths of a degree
			// clockwise from north (0 to 35999)
			prop->lMin = -1;
			prop->lMax = 35999;
		}
		virtual void GetState(LPVOID dst, const XINPUT_STATE& state) {
			*(DWORD*)dst = -1; // -1 indicates center position
		}
	};

	switch (dwType)
	{
	case DIDFT_AXIS:
		m_assignedControls[offset] = std::shared_ptr<Control>(new ZeroAxisControl);
		break;
	case DIDFT_BUTTON:
		m_assignedControls[offset] = std::shared_ptr<Control>(new ZeroButtonControl);
		break;
	case DIDFT_POV:
		m_assignedControls[offset] = std::shared_ptr<Control>(new ZeroPovControl);
		break;
	default:
		Debug("Don't know how to assign zero control to type 0x%X\n", dwType);
		break;
	}
}
