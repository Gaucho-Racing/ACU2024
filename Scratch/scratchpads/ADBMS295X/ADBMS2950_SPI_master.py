# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This scratchpad shows how the different SPI configurations on Tiger (ADBMS2950) rev C or higher. It is possible to select:
- 3 wire spi
- 4 wire spi
- n wire spi (adressing n devices (demonstated as SPI_5W))
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS2950 import ADBMS2950
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from pprint import pprint

def main():
    interface = USB_TO_SPI_BYTE('COM29', 115200, stdout=False)
    bms = BMS(interface)
    board_list = [{'Device': ADBMS2950}]

    # Sending data in 3 wire SPI
    SPI_3W = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        # Activate all required GP(I)O ports and 3 wire SPI
        {"command": "WRCFGA", "arguments": {"SPI3W": True}},
        {"command": "WRCFGB", "arguments": {"GPIO1C": False, "GPIO2C": True,"GPIO3C": True, "GPIO4C": True}},
        {"command": "WRCOMM",
         "arguments": {"ICOM0": 8,
                       "D0": 142,
                       "FCOM0": 0,
                       "ICOM1": 8,
                       "D1": 143,
                       "FCOM1": 0,
                       "ICOM2": 8,
                       "D2": 144,
                       "FCOM2":0}},
        {"command": "STCOMM"},
        {"command": "$SPI_CLOCK_OUT$", "arguments": {"Num Bytes": 3}},
        # Read return data
        {"command": "RDCOMM", "map_key": "data_0"},
        {"command": "WRCOMM",
         "arguments": {"ICOM0": 8,
                       "D0": 255,
                       "FCOM0": 0,
                       "ICOM1": 8,
                       "D1": 255,
                       "FCOM1": 0,
                       "ICOM2": 8,
                       "D2": 255,
                       "FCOM2": 9}},
        {"command": "STCOMM"},
        {"command": "$SPI_CLOCK_OUT$", "arguments": {"Num Bytes": 3}},
        # After spi_clock_out it is required to have one more command. It doesn't have to do anything
        # Read return data
        {"command": "RDCOMM", "map_key": "data_1"},
    ]

    # Sending data in 4 wire spi.
    SPI_4W = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        # Activate all required GP(I)O ports
        {"command": "WRCFGA", "arguments": {"SPI3W": False}},
        {"command": "WRCFGB", "arguments": {"GPIO1C": True, "GPIO2C": True,"GPIO3C": True, "GPIO4C": True}},
        {"command": "WRCOMM",
         "arguments": {"ICOM0": 8,
                       "D0": 142,
                       "FCOM0": 0,
                       "ICOM1": 8,
                       "D1": 143,
                       "FCOM1": 0,
                       "ICOM2": 8,
                       "D2": 144,
                       "FCOM2":0}},
        {"command": "STCOMM"},
        {"command": "$SPI_CLOCK_OUT$", "arguments": {"Num Bytes": 3}},
        # Read return data
        {"command": "RDCOMM", "map_key": "data_0"},
        {"command": "WRCOMM",
         "arguments": {"ICOM0": 8,
                       "D0": 145,
                       "FCOM0": 0,
                       "ICOM1": 8,
                       "D1": 146,
                       "FCOM1": 0,
                       "ICOM2": 8,
                       "D2": 147,
                       "FCOM2": 9}},
        {"command": "STCOMM"},
        {"command": "$SPI_CLOCK_OUT$", "arguments": {"Num Bytes": 3}},
        # Read return data
        {"command": "RDCOMM", "map_key": "data_1"},
    ]

    # It is also possible to use GPO pins to address more SPI devices. In this case the CS has to be enabled manually
    # For example, by sending the command:  {"command": "WRCFGA", "arguments": {"GPO5C": False}}
    SPI_5W = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        # Activate all required GP(I)O ports
        {"command": "WRCFGA", "arguments": {"SPI3W": False}},
        {"command": "WRCFGB", "arguments": {"GPIO1C": True, "GPIO2C": True, "GPIO3C": True, "GPIO4C": True, "SPI3W": False}},
        # CS is low
        {"command": "WRCFGA", "arguments": {"GPO5C": False}},
        {"command": "WRCOMM",
         "arguments": {"ICOM0": 9,
                       "D0": 142,
                       "FCOM0": 9,
                       "ICOM1": 9,
                       "D1": 143,
                       "FCOM1": 9,
                       "ICOM2": 9,
                       "D2": 144,
                       "FCOM2":9}},
        {"command": "STCOMM"},
        {"command": "$SPI_CLOCK_OUT$", "arguments": {"Num Bytes": 3}},
        # Read return data
        {"command": "RDCOMM", "map_key": "data_0"},
        {"command": "WRCOMM",
         "arguments": {"ICOM0": 9,
                       "D0": 145,
                       "FCOM0": 9,
                       "ICOM1": 9,
                       "D1": 146,
                       "FCOM1": 9,
                       "ICOM2": 9,
                       "D2": 147,
                       "FCOM2": 9}},
        {"command": "STCOMM"},
        {"command": "$SPI_CLOCK_OUT$", "arguments": {"Num Bytes": 3}},
        # CS is high
        {"command": "WRCFGA", "arguments": {"GPO5C": True}},
        # Read return data
        {"command": "RDCOMM", "map_key": "data_1"},
    ]

    # Choose SPI_3W, SPI_4W or SPI_5W
    read_results = bms.run_generic_command_list(SPI_3W, board_list)
    interface.close()

    # # Print the data
    map_keys = ['data_0', 'data_1']
    keys = ['D0','D1','D2']
    for map_key in map_keys:
        print(map_key)
        for key in keys:
            print("%3s = %s" %(key, read_results[map_key][0][key]))

if __name__ == "__main__":
    main()