# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This scratchpad shows how to do a basic setup for the LPCM mode with 2 ADBMS6830 in a daisychain configuration.
A diamond serves as the timeout monitor and can be checked through its I/O pins.
LPCM mode runs with a 1s HB rate for 5 seconds total. OV/UV/DV thresholds can be adjusted to simulate errors.
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS6830 import ADBMS6830
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from pprint import pprint

def main():
    # Create the interface. This is the standard interface. Others can be found in the Interfaces directory
    interface = USB_TO_SPI_BYTE('COM4', 115200, stdout=True)
    # Create the BMS Object
    bms = BMS(interface)
    # Create the board list. Here you can add multiple devices in a daisychain
    board_list = [{'Device': ADBMS6830}, {'Device': ADBMS6830}]

    command_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 200}},
        {"command": "SRST"},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 200}},
        {"command": "CLRFLAG"},     # needed, as status flags also raise LPCM flags
        {"command": "CLRCMFLAG"},   # optional, as relevant LPCM flags reset to 0
        {"command": "RDCMFLAG", "map_key": "CMFLAG_before"},

        {"command": "WRCMCFG",
         "board_list": [{"CMC_MAN": 0, "CMC_MPER": 7, "CMC_BTM": 0, "CMC_NDEV": 0x44, "CMM_C": 0x00000, "CMM_G": 0x000, "CMC_DIR": 1},
                        {"CMC_MAN": 1, "CMC_MPER": 7, "CMC_BTM": 0, "CMC_NDEV": 0x44, "CMM_C": 0x00000, "CMM_G": 0x000,
                         "CMC_DIR": 1}]},
        {"command": "WRCMCELLT", "arguments": {"CMT_CUV": -3, "CMT_COV": 6, "CMT_CDV": 4}},  # Cell OV/UV/DV thresholds
        {"command": "WRCMGPIOT", "arguments": {"CMT_GUV": -3, "CMT_GOV": 6, "CMT_GDV": 4}},

        {"command": "CMEN"},

        {"command": "$GPIO_WRITE$", "arguments": {"GPIO Pin": 11, "GPIO Value": 0}},            # MSTR enable
        {"command": "$GPIO_READ$", "arguments": {"GPIO Pin": 106}, "map_key": "INTR_read1"},    # INTR readout

        {"command": "$DELAY_MS$", "arguments": {"Delay": 5000}},    # LPCM running in background

        {"command": "$GPIO_READ$", "arguments": {"GPIO Pin": 106}, "map_key": "INTR_read2"},    # INTR readout
        {"command": "$GPIO_WRITE$", "arguments": {"GPIO Pin": 11, "GPIO Value": 1}},            # MSTR disable

        # send multiple CMDIS to make sure it reaches all Devices
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 200}},
        {"command": "CMDIS"},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 200}},
        {"command": "CMDIS"},

        {"command": "RDCMFLAG", "map_key": "CMFLAG_after"},  # Readout of LPCM Flags afterwards
    ]

    # Run generic command_list
    results = bms.run_generic_command_list(command_list, board_list)

    # Close the interface
    interface.close()
    # Pretty print the results
    pprint(results)

    if not bool(results["INTR_read2"][0]["GPIO Value"]):
        print("LPCM finished without errors")
    else:
        print("LPCM finished with errors")


if __name__ == "__main__":
    main()