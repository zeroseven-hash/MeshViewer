#include"pch.h"
#include"WindowsDragDrop.h"

#include"CoreModel/Log.h"
namespace MV
{
	static void Wchar_tToString(std::string& target, wchar_t* wchar)
	{
		wchar_t* wText = wchar;
		DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
		char* psText; // psText为char*的临时数组，作为赋值给std::string的中间变量
		psText = new char[dwNum];
		WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用
		target = psText;// std::string赋值
		delete[]psText;// psText的清除
	}
	HRESULT STDMETHODCALLTYPE WindwosDropTarget::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
	{
		if (pdwEffect == NULL)
			return E_INVALIDARG;

		*pdwEffect |= DROPEFFECT_COPY;

		FORMATETC format;
		STGMEDIUM medium;
		format.cfFormat = CF_HDROP;
		format.ptd = NULL;
		format.dwAspect = DVASPECT_CONTENT;
		format.lindex = -1;
		format.tymed = TYMED_HGLOBAL;
		medium.tymed = TYMED_HGLOBAL;
		HRESULT res = pDataObj->GetData(&format, &medium);
		HDROP drop = (HDROP)medium.hGlobal;
		wchar_t* fileName = NULL;
		//See https://docs.microsoft.com/en-us/windows/desktop/api/shellapi/nf-shellapi-dragqueryfilew
		UINT filePathesCount = DragQueryFile(drop, 0xFFFFFFFF, NULL, 512);//If "0xFFFFFFFF" as the second parameter: return the count of files dropped
		UINT longestFileNameLength = 0;


		WindowDropEvent event;
		for (UINT i = 0; i < filePathesCount; ++i)
		{
			//If NULL as the third parameter: return the length of the path, not counting the trailing '0'
			UINT fileNameLength = DragQueryFile(drop, i, NULL, 512) + 1;
			if (fileNameLength > longestFileNameLength)
			{
				if (fileName != NULL)
				{
					free(fileName);
				}
				longestFileNameLength = fileNameLength;
				fileName = (wchar_t*)malloc(longestFileNameLength * sizeof(*fileName));
			}
			DragQueryFile(drop, i, fileName, fileNameLength);

			std::string file;
			Wchar_tToString(file, fileName);
			event.PushFile(std::move(file));	
		}
		m_EventCallBackFn(event);

#ifdef DEBUG
		MESH_TRACE("{0}", event.ToString());
#endif
		
		if (fileName != NULL)
		{
			free(fileName);
		}
		ReleaseStgMedium(&medium);

		return S_OK;
	}
}