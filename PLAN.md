# LightSwitch Standalone — 实施计划（v2）

> 基于 [Microsoft PowerToys](https://github.com/microsoft/PowerToys) LightSwitch 模块源码直接复用，从零构建独立可执行程序。
> 设计目标：**极轻量 + 事件驱动 + 极简 GUI**

---

## 一、项目目标

在不安装/依赖 PowerToys 的前提下，复用其 LightSwitch 模块源码，构建一个 Windows 自动深浅色主题切换工具：

- **固定时间段切换**（如 08:00 亮 / 20:00 暗）
- **日出日落自动计算**（基于经纬度）
- **跟随 Windows Night Light**（联动夜间模式）
- **一键手动切换**
- **系统主题与应用主题独立控制**
- **极简 GUI 界面**（Win32 原生窗口，< 10 KB 额外开销）
- **事件驱动架构**（零轮询，CPU 占用 ≈ 0%）

---

...（完整内容见仓库文件）