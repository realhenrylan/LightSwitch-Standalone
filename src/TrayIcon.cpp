#include "pch.h"
#include "TrayIcon.h"
static HICON CreateColorBlockIcon(bool light) {
    constexpr int S=16; COLORREF c = light ? RGB(255,200,50) : RGB(50,50,120);
    HDC sdc = GetDC(nullptr), mdc = CreateCompatibleDC(sdc);
    if (!mdc) { ReleaseDC(nullptr,sdc); return nullptr; }
    HBITMAP hCol = CreateCompatibleBitmap(sdc,S,S), hMsk = CreateBitmap(S,S,1,1,nullptr);
    if (!hCol||!hMsk) { if(hCol)DeleteObject(hCol); if(hMsk)DeleteObject(hMsk); DeleteDC(mdc); ReleaseDC(nullptr,sdc); return nullptr; }
    HGDIOBJ o = SelectObject(mdc,hCol); RECT rc={0,0,S,S}; HBRUSH br=CreateSolidBrush(c); FillRect(mdc,&rc,br); DeleteObject(br);
    HPEN p=CreatePen(PS_SOLID,1,RGB(128,128,128)); SelectObject(mdc,p); SelectObject(mdc,GetStockObject(NULL_BRUSH)); Rectangle(mdc,0,0,S,S); DeleteObject(p);
    SelectObject(mdc,o);
    HDC mdc2 = CreateCompatibleDC(sdc); HGDIOBJ o2 = SelectObject(mdc2,hMsk); FillRect(mdc2,&rc,(HBRUSH)GetStockObject(WHITE_BRUSH)); SelectObject(mdc2,o2); DeleteDC(mdc2);
    ICONINFO ii={}; ii.fIcon=TRUE; ii.hbmMask=hMsk; ii.hbmColor=hCol;
    HICON hi = CreateIconIndirect(&ii);
    DeleteObject(hCol); DeleteObject(hMsk); DeleteDC(mdc); ReleaseDC(nullptr,sdc);
    return hi;
}
TrayIcon::TrayIcon() {}
TrayIcon::~TrayIcon() { Destroy(); }
bool TrayIcon::Create(HWND h,HINSTANCE hi,UINT id,UINT cb,LPCWSTR tip,bool light) {
    if(m_created) return true;
    m_hwnd=h; m_hInst=hi; m_uID=id; m_uCallbackMsg=cb;
    m_hIconLight=CreateColorBlockIcon(true); m_hIconDark=CreateColorBlockIcon(false);
    m_iconNeedsDestroy=true;
    ZeroMemory(&m_nid,sizeof(m_nid)); m_nid.cbSize=sizeof(m_nid);
    m_nid.hWnd=h; m_nid.uID=id; m_nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_SHOWTIP;
    m_nid.uCallbackMessage=cb; m_nid.hIcon=light?m_hIconLight:m_hIconDark;
    wcsncpy_s(m_nid.szTip,tip,_TRUNCATE); m_nid.uVersion=NOTIFYICON_VERSION_4;
    if(!Shell_NotifyIconW(NIM_ADD,&m_nid)) return false;
    Shell_NotifyIconW(NIM_SETVERSION,&m_nid); m_created=true; return true;
}
void TrayIcon::Destroy() {
    if(m_created) { Shell_NotifyIconW(NIM_DELETE,&m_nid); m_created=false; }
    if(m_hIconLight&&m_iconNeedsDestroy){DestroyIcon(m_hIconLight);m_hIconLight=nullptr;}
    if(m_hIconDark&&m_iconNeedsDestroy){DestroyIcon(m_hIconDark);m_hIconDark=nullptr;}
}
void TrayIcon::SetIcon(bool light) { if(!m_created) return; m_nid.hIcon=light?m_hIconLight:m_hIconDark; Shell_NotifyIconW(NIM_MODIFY,&m_nid); }
void TrayIcon::SetTooltip(LPCWSTR t) { if(!m_created) return; wcsncpy_s(m_nid.szTip,t,_TRUNCATE); Shell_NotifyIconW(NIM_MODIFY,&m_nid); }
void TrayIcon::ShowContextMenu(HWND h) {
    POINT pt; GetCursorPos(&pt);
    HMENU hm=CreatePopupMenu(); if(!hm) return;
    AppendMenuW(hm,MF_STRING,IDM_TRAY_TOGGLE,L"切换主题");
    AppendMenuW(hm,MF_STRING,IDM_TRAY_TOGGLE_SCHEDULE,L"启用/禁用自动调度");
    AppendMenuW(hm,MF_SEPARATOR,0,0);
    AppendMenuW(hm,MF_STRING,IDM_TRAY_OPEN_SETTINGS,L"打开设置");
    AppendMenuW(hm,MF_STRING,IDM_TRAY_EXIT,L"退出");
    SetForegroundWindow(h); TrackPopupMenu(hm,TPM_RIGHTALIGN|TPM_BOTTOMALIGN,pt.x,pt.y,0,h,0); DestroyMenu(hm);
}
