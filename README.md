# **Underground-Garage-IoT-Monitor**

**基于STM32与机智云的智能车库环境监控系统**  

### 🚀 项目概述

通过STM32微控制器与物联网技术，实时监测地下车库环境参数（温湿度、PM2.5、有害气体、烟雾浓度），结合机智云平台与手机APP实现远程监控与通风控制，预防环境污染风险。  

### 🌟 核心功能

- **多参数监测**  
  - DHT11传感器 → 温湿度  
  - ZH-01激光模块 → PM2.5浓度  
  - MQ135/MQ2传感器 → 空气质量/烟雾检测  
- **物联网联动**  
  - ESP8266 WiFi模块上传数据至机智云平台  
  - 手机APP实时查看数据与报警通知  
- **智能控制**  
  - 阈值触发通风设备（如风机）  
  - 历史数据存储与分析  

### 🛠️ 技术栈

| 类别       | 组件/技术                        |
| ---------- | -------------------------------- |
| **硬件**   | STM32F103C8T6、ESP8266           |
| **传感器** | DHT11、ZH-01、MQ135、MQ2         |
| **云平台** | 机智云（Gizwits）                |
| **客户端** | Android/iOS APP（基于机智云SDK） |

### 📂 仓库结构  
```  
├─.vscode
├─DebugConfig
├─Gizwits
├─Hardware
├─Library
├─Listings
├─Objects
├─Start
├─System
│  ├─delay
│  ├─RTC
│  ├─sys
│  ├─timer
│  └─usart
├─User
├─Utils
└─机智云原始开发包
    └─MCU_Common_source
        ├─Gizwits
        ├─User
        └─Utils
```

### 📌 应用场景  
- 地下停车场  
- 仓储环境监控  
- 工业封闭空间  
