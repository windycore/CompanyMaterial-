[ActiveSupport MAP]
Device = LCMXO2-1200HC;
Package = TQFP144;
Performance = 4;
LUTS_avail = 1280;
LUTS_used = 1027;
FF_avail = 1388;
FF_used = 600;
INPUT_LVTTL33 = 32;
OUTPUT_LVTTL33 = 30;
IO_avail = 108;
IO_used = 62;
EBR_avail = 7;
EBR_used = 3;
; Begin EBR Section
Instance_Name = fifo_u1/fifo64x8_0_0;
Type = FIFO8KB;
Width = 8;
REGMODE = NOREG;
RESETMODE = ASYNC;
ASYNC_RESET_RELEASE = SYNC;
GSR = DISABLED;
Instance_Name = mcu2pc_uart_top1/mcu2pc_fifo/fifo64x8_0_0;
Type = FIFO8KB;
Width = 8;
REGMODE = NOREG;
RESETMODE = ASYNC;
ASYNC_RESET_RELEASE = SYNC;
GSR = DISABLED;
Instance_Name = pc2mcu_uart_top1/pc2mcu_fifo/fifo64x8_0_0;
Type = FIFO8KB;
Width = 8;
REGMODE = NOREG;
RESETMODE = ASYNC;
ASYNC_RESET_RELEASE = SYNC;
GSR = DISABLED;
; End EBR Section
