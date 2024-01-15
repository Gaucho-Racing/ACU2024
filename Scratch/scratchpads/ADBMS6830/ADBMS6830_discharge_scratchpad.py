# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Scratchpad performs discharge on different cells on lion (ADBMS6830). In order to detect this, the S-Pins compared to the Cells have been
compared. If everything works well, you can see the green values on the cells that are discharging.

The S-Pins are affected by the discharge while the Cells are not. Due to timings, it is possible the S-Pin does not detect
it right away. Therefore, it does two measurements. By aware that batteries sometimes need a little time to get back to their
original value. You might see this in the results.

Also, be aware that this might not work on the batterysim DC2472A. The current the discharge takes, can brown out lion and can
show strange results. Use real batteries or give proper current sources.

If performed correctly, you should see:
number 0-1, no discharge (also on number 2)
number 3, discharge on cell 1
number 4-5, discharge on cell 2
number 6-7, discharge on cell 3
number 8-9, discharge on cell 4
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS6830 import ADBMS6830
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from prettytable import PrettyTable
from termcolor import colored
import os
os.system('color')

def main():
    interface = USB_TO_SPI_BYTE('COM29', 115200, stdout=False)
    bms = BMS(interface)
    board_list = [{'Device': ADBMS6830}]
    # Create pretty table for printing
    x = PrettyTable()

    """
    Make a list that reads the spins. This automaticly expands to the amount of counts you give it
    
    :param count: Giving the count of the reading. So if you want to map it to val_X, give X
    :type count: Int, required
    :return read_spins: List that can be added to the command list if splatted.
    """
    def read_spins(count):
        read_spins = [
            {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
            {"command": "RDCVA", "map_key": "val_" + str(count)},
            {"command": "RDCVB", "map_key": "val_" + str(count)},
            {"command": "RDCVC", "map_key": "val_" + str(count)},
            {"command": "RDCVD", "map_key": "val_" + str(count)},
            {"command": "RDCVE", "map_key": "val_" + str(count)},
            {"command": "RDCVF", "map_key": "val_" + str(count)},
            {"command": "RDSVA", "map_key": "val_" + str(count)},
            {"command": "RDSVB", "map_key": "val_" + str(count)},
            {"command": "RDSVC", "map_key": "val_" + str(count)},
            {"command": "RDSVD", "map_key": "val_" + str(count)},
            {"command": "RDSVE", "map_key": "val_" + str(count)},
            {"command": "RDSVF", "map_key": "val_" + str(count)},
        ]
        return read_spins

    command_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "WRCFGA", "arguments": {"REFON": True}},
        {"command": "RDCFGA"},
        {"command": "ADCV", "arguments": {"CONT": True, "RD": True}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 50}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        *read_spins(0),
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        *read_spins(1),
        # Discharge cell 1
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "WRCFGB", "arguments": {"DCC": 0x001}},
        *read_spins(2),
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        *read_spins(3),
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        # Discharge cell 2
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "WRCFGB", "arguments": {"DCC": 0x002}},
        *read_spins(4),
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        *read_spins(5),
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        # Discharge cell 3
        {"command": "WRCFGB", "arguments": {"DCC": 0x0004}},
        *read_spins(6),
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        *read_spins(7),
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        # Discharge cell 4
        {"command": "WRCFGB", "arguments": {"DCC": 0x0008}},
        *read_spins(8),
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        *read_spins(9),
    ]

    # Run the script
    result = bms.run_generic_command_list(command_list, board_list)
    # Close the interface
    interface.close()

    # Create Keys that you want to read
    keys = []
    for key in range(1,17):
        keys.append('S%dV' %key)
    # Add Cells for comparison
    keys = ['C1V', 'C2V'] + keys

    x.field_names = ['number'] + keys
    # Get all the values in the proper format
    for val in range(0,10):
        line = []
        # Take average of Cell 1 and Cell 2 (more is possible, but for now this seemed to be enough)
        avg = 0.5 * result['val_%d' %val][0]['C1V'] + 0.5 * result['val_%d' %val][0]['C2V']
        for key in keys:
            voltage = result['val_%d' %val][0][key]
            # If difference between cell and spins is bigger then 1 volt, make green
            # else, keep the original color
            if(abs(avg - voltage) > 1):
                line.append(colored(('%.04f' %voltage), 'green'))
            else:
                line.append('%.04f' % voltage)
        # Add this line to the pretty table
        x.add_row([val] + line)

    # Print the pretty table
    print(x)

