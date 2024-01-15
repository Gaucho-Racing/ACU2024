# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This scratchpad shows how to control I2C master on tiger rev C or higher (ADBMS2950). Be aware that this is using the clock of the host (Iso)SPI controller.
Meaning, if you have a 2 MBit/S clock on SPI, your I2C master will most likely be on a frequency that is not in the specs.

The concept of the scratchpad is really easy. Write data to WRCOMM, transfer with STCOMM, spi_clock_out for writing data and then
reading the data back by WRCOMM (write read address), STCOMM, spi_clock_out, RDCOMM, STCOMM, spi_clock_out
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS2950 import ADBMS2950
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from pprint import pprint

def main():
    interface = USB_TO_SPI_BYTE('COM23', 115200, stdout=False)
    bms = BMS(interface)
    board_list = [{'Device': ADBMS2950}]

    # Write 142 to the EEPROM memory
    write_data_i2c = [
        {"command": "$SPI_SET_FREQUENCY_kHz$", "arguments": {"Frequency": 1000}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "WRCFGA", "arguments": {"GPIO3C": True, "GPIO4C": True}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 5}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "WRCOMM",
         "arguments": {"ICOM0": 6,
                       "D0": 160,
                       "FCOM0": 8,
                       "ICOM1": 0,
                       "D1": 0,
                       "FCOM1": 8,
                       "ICOM2": 0,
                       "D2": 142,
                       "FCOM2": 9}},
        {"command": "STCOMM"},
        {"command": "$SPI_CLOCK_OUT$", "arguments": {"Num Bytes": 3}},
        # After spi_clock_out it is required to have one more command. It doesn't have to do anything
        {"command": "RDCFGA", "arguments": {}, "map_key": "TRASH"}
    ]

    read_data_i2c = [
        {"command": "$DELAY_MS$", "arguments": {"Delay": 7}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "WRCOMM",
         "arguments": {"ICOM0": 6,
                       "D0": 160,
                       "FCOM0": 8,
                       "ICOM1": 0,
                       "D1": 0,
                       "FCOM1": 8,
                       "ICOM2": 6,
                       "D2": 161,
                       "FCOM2": 8}},
        {"command": "STCOMM"},
        {"command": "$SPI_CLOCK_OUT$", "arguments": {"Num Bytes": 3}},
        {"command": "WRCOMM",
         "arguments": {"ICOM0": 0,
                       "D0": 255,
                       "FCOM0": 9,
                       "ICOM1": 7,
                       "D1": 0,
                       "FCOM1": 9,
                       "ICOM2": 7,
                       "D2": 0,
                       "FCOM2": 9}},
        {"command": "STCOMM"},
        {"command": "$SPI_CLOCK_OUT$", "arguments": {"Num Bytes": 3}},
        {"command": "RDCOMM", "map_key": "TEST_DATA"},
        # After spi_clock_out it is required to have one more command. It doesn't have to do anything
        {"command": "RDCFGA", "arguments": {}, "map_key": "TRASH"}
    ]
    write_results = bms.run_generic_command_list(write_data_i2c, board_list)
    read_results = bms.run_generic_command_list(read_data_i2c, board_list)

    # Print the data
    keys = ['D2', 'D1', 'D0']
    for key in keys:
        print("%3s = %s" %(key, read_results["TEST_DATA"][0][key]))

    interface.close()

if __name__ == "__main__":
    main()