# SD Card Write Example — Riverdi 5" (STM32U5)

This is a test/demo application showing SD card write functionality on the Riverdi 5" display module (STM32U5-based).

## What it does

The screen shows a static "SD Card Test" label. There is no visual status indicator — the result of the write can be verified by checking the SD card on a PC.

When an SD card is present at boot (or inserted while the module is running), the firmware:

1. Waits 500 ms for the card to settle
2. Mounts the FAT32 filesystem (formats the card if no filesystem is found)
3. Writes `TEST.TXT` to the root of the card
4. Unmounts the filesystem

## TEST.TXT contents

```
SD card write test - OK
To verify the write works: delete this file and power-cycle the board.
If the file reappears, SD card write is working correctly.
```

The file creation date visible in Windows Explorer (file Properties) reflects the RTC time at the moment of writing. The RTC is set once on the first boot from the build-time macros (`__DATE__` / `__TIME__`) and stored in a backup register. If a backup battery is present, the timestamp advances across resets.

## How to re-trigger the write

This is a one-shot demo — one write per power cycle:

- **Card in before/during boot:** the module writes immediately after startup.
- **Card inserted after boot:** the module detects it and writes once.
- **Removing and reinserting the card** does not trigger a new write.

**To write again:** insert the card and reset (or power-cycle) the module. To verify: delete `TEST.TXT` from the card on a PC, insert the card, reset the module, and confirm the file reappears.

## Technical notes

- MCU: STM32U5
- SD interface: SDMMC, 4-bit bus
- FatFS + FreeRTOS CMSIS-RTOS v2
- TouchGFX: static screen, no SD status integration
- Auto-format: if no FAT32 filesystem is detected on the card, the firmware formats it automatically before writing
