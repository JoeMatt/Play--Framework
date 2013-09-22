#pragma once

#include "ActiveXHost.h"
#include <ExDisp.h>
#include <MsHTML.h>
#include <set>

namespace Framework
{
	namespace Win32
	{
		class CWebBrowser : public CActiveXHost
		{
		public:
			enum
			{
				NOTIFICATION_BEFORENAVIGATE = 0xA000,
			};

			struct BEFORENAVIGATE_INFO : public NMHDR
			{
				std::tstring		navigateUrl;
				bool				cancel;
			};

										CWebBrowser();
										CWebBrowser(HWND, const RECT&);
			virtual						~CWebBrowser();

			CWebBrowser&				operator =(CWebBrowser&&);

			CComPtr<IHTMLDocument2>		GetDocument();

			void						Navigate(const TCHAR*);
			void						Stop();

		protected:
			void						Reset();
			void						MoveFrom(CWebBrowser&&);

		private:
			class CEventSink : public DWebBrowserEvents2
			{
			public:
										CEventSink(HWND);
				virtual					~CEventSink();

				//IUnknown
				STDMETHODIMP			QueryInterface(const IID&, void**);
				STDMETHODIMP_(ULONG)	AddRef();
				STDMETHODIMP_(ULONG)	Release();

				//IDispatch
				STDMETHODIMP			GetTypeInfoCount(UINT*);
				STDMETHODIMP			GetTypeInfo(UINT, LCID, ITypeInfo**);
				STDMETHODIMP			GetIDsOfNames(const IID&, LPOLESTR*, UINT, LCID, DISPID*);
				STDMETHODIMP			Invoke(DISPID, const IID&, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);

			private:
				ULONG					m_refCount;
				HWND					m_browserWnd;
			};

			static LRESULT CALLBACK				MsgFilterHook(int, WPARAM, LPARAM);
			static void							RegisterFilterHook(CWindow*);
			static void							UnregisterFilterHook(CWindow*);
			static void							ExchangeFilterHook(CWindow*, CWindow*);

			static ULONG __declspec(thread)		g_hookCount;
			static HHOOK __declspec(thread)		g_hookHandle;
			static std::set<CWindow*>			g_hookListeners;

			CComPtr<IWebBrowser2>				m_webBrowser;
			CComPtr<IConnectionPoint>			m_eventsConnectionPoint;
			CComPtr<CEventSink>					m_eventSink;
			DWORD								m_adviseCookie;
		};
	};
}