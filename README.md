# ⏱️ 7‑Segment Stopwatch (BlackBoard, C on ARM)

A personal embedded project implementing a **start/stop/reset stopwatch** on the BlackBoard’s **4‑digit 7‑segment display** using memory‑mapped I/O in C. Buttons control the state machine; the display updates once per second.

> Uses memory addresses for the 7‑seg controller and button GPIO (mapped via the board’s BSP/IP). fileciteturn3file0  
> Follows the assignment’s behavior: BTN0 **Start**, BTN1 **Stop**, BTN2 **Reset** (BCD/hex counter acceptable). fileciteturn3file1

---

## 🧭 Features
- 4‑digit **hex stopwatch** (wraps at `0xFFFF`)
- **BTN0** = start / **BTN1** = stop / **BTN2** = reset to 0 fileciteturn3file1  
- Simple button **edge detection** to avoid retriggering
- Clean **digit packing** into the 7‑seg data register with DP off (bit7=1 per digit) fileciteturn3file0

---

## 🔩 Hardware / Memory Map
- **7‑seg control** register @ `0x43C10000`  
- **7‑seg data**    register @ `0x43C10004`  
- **Buttons (GPIO)** base @ `0x41200000` (lower 4 bits) fileciteturn3file0

The control register enables **hex mode** when written with `1`; each digit is placed in one byte of `SEG_DATA` with its MSB set to keep the decimal point **off**. fileciteturn3file0

---

## 📸 Demo / Bench
Place photos under `Images/` and reference them like:
```markdown
![Running on BlackBoard](Images/blackboard_stopwatch.jpg)
![7‑segment close‑up](Images/seg_close.jpg)
```

---

## 🚀 Build & Run (Vitis/XSCT quick outline)
1. Create a **Standalone** C application targeting the BlackBoard BSP.  
2. Add `src/stopwatch.c` (from below) to the project.  
3. Build → Program FPGA (if required) → Run on Hardware (Launch on ARM).

> Timing uses `sleep(1)` from the BSP; adjust if your platform’s tick differs. fileciteturn3file0

---

## 🧠 Core Code (minimal)
The snippet below shows the register map, digit pack, and main loop with button controls. Adapt file names as needed.

```c
#include <stdint.h>
#include "sleep.h"
#include <stdio.h>

#define SEG_CTL    (*(volatile uint32_t*)0x43C10000)
#define SEG_DATA   (*(volatile uint32_t*)0x43C10004)
#define ButtonData (*(volatile uint32_t*)0x41200000)  // BTN[3:0]

static inline void display_hex(uint16_t n) {
    // Enable HEX mode
    SEG_CTL = 1;
    uint8_t d0 =  n        & 0xF;
    uint8_t d1 = (n >> 4)  & 0xF;
    uint8_t d2 = (n >> 8)  & 0xF;
    uint8_t d3 = (n >> 12) & 0xF;
    // MSB=1 turns decimal point OFF on each digit
    uint32_t packed = (0x80u | d0) |
                      ((0x80u | d1) << 8)  |
                      ((0x80u | d2) << 16) |
                      ((0x80u | d3) << 24);
    SEG_DATA = packed;
}

int main(void) {
    uint16_t counter = 0;
    int running = 0;
    uint32_t prev = 0;

    while (1) {
        uint32_t btn = ButtonData & 0xF;
        uint32_t rising = (~prev) & btn;  // edge detect

        if (rising & 0x1) running = 1;    // BTN0 start
        if (rising & 0x2) running = 0;    // BTN1 stop
        if (rising & 0x4) counter = 0;    // BTN2 reset

        if (running) { counter++; sleep(1); } // ~1 Hz tick
        display_hex(counter);
        prev = btn;
    }
}
```
_Source implementation patterns and addresses from my lab code/report._ fileciteturn3file0turn3file1

---

## 🧪 Notes & Choices
- **Hex vs BCD**: Assignment allows either; code uses **hex** to keep logic minimal. For BCD, add digit carry at 9→0 per nibble. fileciteturn3file1
- **Debounce**: Edge detect mitigates long presses; for noisy buttons consider timers or sampling windows.
- **DP control**: Decimal points held **off** (bit7=1 per digit). fileciteturn3file0

---

## 📁 Suggested Repo Layout
```
seven-seg-stopwatch/
├─ README.md
├─ src/
│  └─ stopwatch.c
├─ Images/
│  ├─ blackboard_stopwatch.jpg
│  └─ seg_close.jpg
└─ Docs/
   └─ report.pdf
```

---

## ✅ Future Enhancements
- Switch to **BCD** and add a fixed DP for mm:ss.  
- Add **hold-to-reset** behavior, or long‑press detection.  
- Replace `sleep(1)` with a **timer interrupt** for precise 1 Hz.  
- Add **lap** capture on a spare button; multiplex to show lap vs live.

---

## 🙋 Author
Rabeea Fatima — Embedded Systems / Microprocessors
