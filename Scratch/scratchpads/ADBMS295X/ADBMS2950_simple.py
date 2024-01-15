# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Scratchpad measures the current, the battery voltages and all the aux. voltages with Tiger rev C (or higher)(ADBMS2950).
The results are printed with pretty print.
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS2950 import ADBMS2950
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from pprint import pprint

def main():
    # Create the interface. This is the standard interface. Others can be found in the Interfaces directory
    interface = USB_TO_SPI_BYTE('/dev/ttyACM0', 115200, stdout=False)
    # Create the BMS Object
    bms = BMS(interface=interface)
    # Create the board list. Here you can add multiple devices in a daisychain
    board_list = [{'Device': ADBMS2950}]

    command_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "SRST"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 2}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "RDCFGA", "arguments": {}, "map_key": "CFGA"},
        {"command": "ADI1", "arguments": {"RD": True, "OPT": 8}},
        {"command": "WRCFGA", "arguments": {
            "GPO1C": True,
            "GPO2C": True,
            "GPO3C": False,
            "GPO1OD": False,
            "GPO2OD": False,
            "GPO3OD": False,
            'VB1MUX': 0,
            'VB2MUX': 0}
         },
        {"command": "$DELAY_MS$", "arguments": {"Delay": 5}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "SNAP"},
        {"command": "RDI", "map_key": "Current"},
        {"command": "RDVB", "map_key": "VBAT"},
        {"command": "UNSNAP"},
        {"command": "ADV", "arguments": {"VCH": 9}},
        {"command": "ADX"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "RDV1A", "map_key": "V1A"},
        {"command": "RDV1B", "map_key": "V1B"},
        {"command": "RDV1C", "map_key": "V1C"},
        {"command": "RDV1D", "map_key": "V1D"},
        {"command": "RDV2A", "map_key": "V2A"},
        {"command": "RDV2B", "map_key": "V2B"},
        {"command": "RDV2C", "map_key": "V2C"},
        {"command": "RDV2D", "map_key": "V2D"},
        {"command": "RDV2E", "map_key": "V2E"},
        {"command": "RDXA", "map_key": "AUX"},
        {"command": "RDXB", "map_key": "AUX"},
        {"command": "RDXC", "map_key": "AUX"},
        {"command": "RDSTAT", "map_key": "STAT"},
        {"command": "RDFLAG", "map_key": "FLAG"},
        {"command": "RDCFGB", "arguments": {}, "map_key": "CFGB"},
    ]

    # Run generic command_list
    results = bms.run_generic_command_list(command_list, board_list, include_raw=False)
    # Close the interface
    interface.close()
    # Pretty print the results
    pprint(results)

if __name__ == "__main__":
    main()