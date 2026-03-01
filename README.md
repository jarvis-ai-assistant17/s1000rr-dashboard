# BMW S1000RR Style Motorcycle Dashboard

ESP32-S3-LCD-4.3 (Waveshare) 摩托車儀表板專案，模仿 BMW S1000RR 的儀表設計風格。

## 硬體需求

- **開發板：** Waveshare ESP32-S3-LCD-4.3
- **螢幕：** 4.3 吋 LCD（800×480 解析度）
- **晶片：** ESP32-S3

## 功能

### 顯示資料

- ✅ **轉速表 (RPM)**：0-15000 RPM，中央大圓弧儀表
- ✅ **時速**：0-300 km/h，大數字顯示
- ✅ **水溫**：引擎冷卻水溫度（°C）
- ✅ **進氣溫度**：進氣歧管溫度（°C）
- ✅ **油門角度 (TPS)**：節氣門開度百分比 (0-100%)

### 設計風格

- **深色背景**（黑/深灰）
- **紅色警告區**（轉速紅線區 12000+ RPM、高水溫 >95°C）
- **藍色/白色主要資訊**
- **現代運動風格**

## 編譯與上傳

### 方法 1：PlatformIO CLI

```bash
# 安裝相依套件
pio lib install

# 編譯
pio run

# 上傳到開發板
pio run --target upload

# 開啟 Serial Monitor
pio device monitor
```

### 方法 2：PlatformIO IDE (VS Code)

1. 在 VS Code 安裝 **PlatformIO IDE** 擴充套件
2. 開啟專案資料夾
3. 點擊下方狀態列的 **Build** 按鈕（✓）
4. 點擊 **Upload** 按鈕（→）

### 方法 3：Arduino IDE

1. 複製 `src/main.cpp` 內容到 Arduino IDE
2. 安裝必要函式庫：
   - **LVGL** (v8.3.11)
   - **TFT_eSPI** (v2.5.43)
3. 配置 TFT_eSPI（編輯 `User_Setup.h`）
4. 選擇開發板：**ESP32-S3 Dev Module**
5. 編譯並上傳

## 模擬數據

目前使用**模擬數據產生器**，數值會自動變化。

### 替換成真實感測器

在 `update_dashboard()` 函數中，將模擬代碼替換成真實感測器輸入：

```cpp
// 範例：從類比輸入讀取油門角度
throttle = map(analogRead(THROTTLE_PIN), 0, 4095, 0, 100);

// 範例：從 CAN Bus 讀取 RPM
rpm = canbus.getRPM();
```

## 專案結構

```
s1000rr-dashboard/
├── platformio.ini          # PlatformIO 配置
├── src/
│   └── main.cpp           # 主程式
└── README.md              # 說明文件
```

## 待辦事項

- [ ] TFT_eSPI 針對 Waveshare ESP32-S3-LCD-4.3 的詳細配置
- [ ] 整合真實感測器輸入（CAN Bus / GPIO）
- [ ] 增加檔位顯示
- [ ] 增加騎乘模式選擇
- [ ] 儲存/讀取最高速度記錄

## 授權

MIT License

---

**開發者：** Kuma 🐻  
**日期：** 2026-03-01
