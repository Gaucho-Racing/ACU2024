# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Scratchpad measures the cell voltages and spin voltages with Lion16 rev C or higher (ADBMS6830).
However, before it does this, it sets the filter values for the iir filter different for different devices.
The first device has an FC of 0, the second one of 2 and the last device of 5. This example is to show how to send
different data to different devices.
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS6830 import ADBMS6830
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from prettytable import PrettyTable

def main():
    # Create the interface. This is the standard interface. Others can be found in the Interfaces directory
    interface = USB_TO_SPI_BYTE('COM7', 115200, stdout=True)
    # interface = Virtual_SPI('data.txt')
    # Create the BMS Object
    bms = BMS(interface)
    # Create the board list. Here you can add multiple devices in a daisychain. In this case there are three devices.
    board_list = [{'Device': ADBMS6830},{'Device': ADBMS6830}]
    # Here you can select the cells. Currently only reg A is being read. If you want to read all the cells, add also B to F
    cells = [1,2,3]
    # here you can select cells, filtered values. If you require Spins or Averaged values, select 'S' or 'AC' and add RDSVA or RDFCA
    register_list = ['C', 'FC']

    command_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "SRST", "map_key": "val"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        # Set different filter values for different devices
        {"command": "WRCFGA", "board_list": [{"REFON": True, 'FC': 0}, {"REFON": True, 'FC': 2}]},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 1}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "ADCV", "arguments": {"CONT": True, "RD": True}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "RDCVA", "map_key": "val"},
        {"command": "RDFCA", "map_key": "val"},
    ]

    # Run generic command_list
    results = bms.run_generic_command_list(command_list, board_list)
    # Close the interface
    interface.close()

    ##### Start processing output data
    # Create table
    x = PrettyTable()
    x.field_names = ['Register'] + [f'Lion{device}' for device in range(len(board_list))]

    # Add the data to the table
    for register in register_list:
        for cell_number in cells:
            values = []
            for device in range(len(board_list)):
                reg = f'{register}' + f'{cell_number}'
                values.append(results['val'][device][reg + 'V'])
            x.add_row([reg] + values)

    # Print the table
    print(x)

if __name__ == "__main__":
    main()