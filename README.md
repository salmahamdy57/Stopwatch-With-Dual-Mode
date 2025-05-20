# Stopwatch-With-Dual-Mode

### Project Overview 🎛️📟
This project implements a versatile digital stopwatch on an ATmega32 microcontroller, featuring two operational modes: Increment (count‑up) and Decrement (count‑down). Time is displayed via six multiplexed seven‑segment displays, and user interactions are handled through dedicated push buttons and visual/audible indicators.

### Objective 🎯
- Design a dual‑mode stopwatch using Timer1 in CTC mode on ATmega32.  
- Utilize multiplexed seven‑segment displays driven by 7447 BCD decoders for time visualization.  
- Implement external interrupts for Reset, Pause, and Resume functions.  
- Provide user‑driven time adjustments and mode toggling via push buttons.  
- Trigger an alarm buzzer when countdown reaches zero.

### Hardware Components 🛠️
- **Microcontroller**: ATmega32 @ 16 MHz  
- **Displays**: 6 x 7‑segment (common anode) multiplexed with NPN transistors  
- **Decoder**: 7447 BCD‑to‑seven‑segment IC  
- **Push Buttons**:  
  - Reset (INT0 / PD2)  
  - Pause (INT1 / PD3)  
  - Resume (INT2 / PB2)  
  - Mode Toggle (PB7)  
  - Hour: Increment (PB1), Decrement (PB0)  
  - Minute: Increment (PB4), Decrement (PB3)  
  - Second: Increment (PB6), Decrement (PB5)  
- **LED Indicators**:  
  - Increment Mode (PD4, Red)  
  - Decrement Mode (PD5, Yellow)  
- **Alarm/Buzzer**: PD0  

### Features 🚀
1. **Increment Mode** (Default)  
   - Counts upward from 00:00:00 (hh:mm:ss).  
   - Red LED ON.  
2. **Decrement Mode**  
   - Pause timer, toggle mode, adjust start time, then resume.  
   - Yellow LED ON.  
   - Buzzer sounds at zero.  
3. **Reset**  
   - External INT0 resets time to 00:00:00.  
4. **Pause/Resume**  
   - INT1 pauses, INT2 resumes timer.  
5. **Time Adjustment**  
   - Hour, minute, second increment/decrement via dedicated buttons.

### Operation Flow 🔄
1. **Power Up**: Timer1 starts in Increment mode.  
2. **Counting**  
   - `TIMER1_COMPA_vect` ISR updates time every second.  
   - Multiplexed display refreshes digits rapidly in main loop.  
3. **User Inputs**  
   - **INT0** (falling): Reset time.  
   - **INT1** (rising): Pause.  
   - **INT2** (falling): Resume.  
   - **Mode Button**: Toggle Increment/Countdown.  
   - **Adjust Buttons**: Modify hours, minutes, seconds in Countdown mode.  
4. **Alarm**  
   - Timer stops and PD0 toggles buzzer at 0:0:0 in Decrement mode.
