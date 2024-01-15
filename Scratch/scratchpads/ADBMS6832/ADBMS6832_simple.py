# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Scratchpad measures the cell voltages and spin voltages with Lion16 rev C or higher.
The results are printed with pretty print.
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS6832 import ADBMS6832
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from pprint import pprint

def main():
    # Create the interface. This is the standard interface. Others can be found in the Interfaces directory
    interface = USB_TO_SPI_BYTE('COM38', 115200, stdout=False)
    # Create the BMS Object
    bms = BMS(interface)
    # Create the board list. Here you can add multiple devices in a daisychain
    board_list = [{'Device': ADBMS6832}]

    command_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "WRCFGA", "arguments": {"REFON": True}},
        {"command": "ADCV", "arguments": {"CONT": True}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "SNAP"},
        {"command": "RDCVA", "map_key": "Cells"},
        {"command": "RDCVB", "map_key": "Cells"},
        {"command": "RDCVC", "map_key": "Cells"},
        {"command": "RDCVD", "map_key": "Cells"},
        {"command": "RDCVE", "map_key": "Cells"},
        {"command": "RDCVF", "map_key": "Cells"},
        {"command": "UNSNAP"},
        {"command": "ADSV"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 9}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "RDSVA", "map_key": "Spins"},
        {"command": "RDSVB", "map_key": "Spins"},
        {"command": "RDSVC", "map_key": "Spins"},
        {"command": "RDSVD", "map_key": "Spins"},
        {"command": "RDSVE", "map_key": "Spins"},
        {"command": "RDSVF", "map_key": "Spins"},
    ]

    # Run generic command_list
    results = bms.run_generic_command_list(command_list, board_list)
    # Close the interface
    interface.close()
    # Pretty print the results
    pprint(results)

if __name__ == "__main__":
    main()