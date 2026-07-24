#pragma once
#include "pch.h"
#include "TrayIcon.h"
#include "LightSwitchStateManager.h"
#include <memory>
class MainWindow {
public:
    MainWindow(); ~MainWindow();
    bool Initialize(HINSTANCE h, int nCmd);
    void Shutdown(); void Show(); void Hide(); bool IsVisible() const;
    void RefreshUI();
    HWND GetHwnd() const { return m_hwnd; }
    TrayIcon& GetTrayIcon() { return m_trayIcon; }
    LightSwitchStateManager& GetStateManager() { return *m_stateManager; }
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT HandleMessage(HWND, UINT, WPARAM, LPARAM);
private:
    HWND m_hwnd = nullptr; HINSTANCE m_hInst = nullptr;
    TrayIcon m_trayIcon;
    std::unique_ptr<LightSwitchStateManager> m_stateManager;
    HWND m_hScheduleMode=nullptr, m_hChangeSystem=nullptr, m_hChangeApps=nullptr;
    HWND m_hLightTime=nullptr, m_hDarkTime=nullptr, m_hLatitude=nullptr, m_hLongitude=nullptr, m_hStatus=nullptr;
    void CreateControls();
    void OnScheduleModeChanged(); void OnCheckboxChanged(); void OnTimeChanged();
    void OnCoordinatesChanged(); void OnToggleLight(); void OnToggleDark();
    void UpdateStatusBar(const std::wstring& t);
    static int MinutesToHM(int m, wchar_t* buf, size_t sz);
    static int HMToMinutes(const wchar_t* t);
    // 经纬度编辑框子类化：拦截回车键立即触发坐标变更
    static LRESULT CALLBACK CoordEditProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
};
extern MainWindow* g_pMainWindow;
