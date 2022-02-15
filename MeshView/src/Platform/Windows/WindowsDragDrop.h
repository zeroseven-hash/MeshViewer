#pragma once

#include<CoreModel/FileDialogs.h>

#include<functional>
//windows
#include <oleidl.h>
#include<shellapi.h>

namespace MV
{
	class WindwosDropTarget :public IDropTarget
	{
	public:
		WindwosDropTarget() = default;
		WindwosDropTarget(const std::function<void(Event&)>& callbackfn)
			:m_EventCallBackFn(callbackfn){}


		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) { return S_OK; }

		virtual ULONG STDMETHODCALLTYPE AddRef(void) { return 0; }

		virtual ULONG STDMETHODCALLTYPE Release(void) { return 0; }

		virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			if (pdwEffect == NULL)
				return E_INVALIDARG;

			*pdwEffect |= DROPEFFECT_COPY;

			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			if (pdwEffect == NULL)
				return E_INVALIDARG;

			*pdwEffect |= DROPEFFECT_COPY;

			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE DragLeave(void)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
		
		void SetCallBackFunc(const std::function<void(Event&)>& callbackfn) { m_EventCallBackFn = callbackfn; }
	private:
		std::function<void(Event&)> m_EventCallBackFn;
	};
}
