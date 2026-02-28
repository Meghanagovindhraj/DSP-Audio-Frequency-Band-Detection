# 🎵 Real-Time Audio Frequency Band Detection using TMS320C6713 DSP
---

## 🛠 Technologies Used

- Embedded C
- TMS320C6713 DSP
- AIC23 Codec
- Code Composer Studio (CCS)
- Radix-2 FFT Algorithm
- McBSP Interface

---

## ⚙ System Specifications

- Sampling Rate: 8 kHz  
- FFT Size: 256  
- Frequency Resolution: 31.25 Hz  
- Processor: Floating-Point DSP  

---

## 📂 Files in the Project

- `main.c` – Complete FFT implementation and LED logic  
- `image.png` – Block diagram  
- `DSP report-1.pdf` – Detailed project report  

---

## 🧠 Working Principle

1. Audio is sampled at 8 kHz using AIC23 codec.
2. 256 samples are collected.
3. Bit-reversal reordering is performed.
4. 256-point Radix-2 FFT is computed.
5. Magnitude spectrum is calculated.
6. Energy is computed for:
   - Low (<300 Hz)
   - Mid (300–2000 Hz)
   - High (>2000 Hz)
7. LED indicates dominant frequency band.

---

## ▶ How to Run

1. Connect audio source to Line-In of DSP board.
2. Open Code Composer Studio.
3. Build the project.
4. Load program into DSP.
5. Run and observe LED indication.

---

## 📌 Applications

- Baby Cry Detection
- Audio Spectrum Analyzer
- Sound Monitoring System

---

## 🚀 Future Improvements

- Add Hamming window before FFT
- Use interrupt-based sampling
- Integrate ML-based sound classification
