# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Scratchpad measures VB1, VB2 with ADBMS2950 rev C or higher.
It uses both the normal and the accumalated registers and runs over multiple loops.
The values are printed in a table with the aid of prettytable.
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS2950 import ADBMS2950
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from prettytable import PrettyTable

# The raw value
acci_raw = 4
# The total amount of loops is acci * loops
acci = 8
loops = 10
def calc_vb(vb1, vb2):
    return vb1/12 * (12 + 3 * 1500), vb2/9.1 * (9.1 + 3 * 1200)

def main():
    # Create the interface. This is the standard interface. Others can be found in the Interfaces directory
    interface = USB_TO_SPI_BYTE('COM23', 115200, stdout=False)
    # Create the BMS Object
    bms = BMS(interface=interface)
    # Create the board list. Here you can add multiple devices in a daisychain
    board_list = [{'Device': ADBMS2950}]

    # The initialisation list
    init_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "SRST"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 25}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "ADI1", "arguments": {"RD": True, "OPT": 8}},
        {"command": "WRCFGA", "arguments": {
            # Open measurement fets and disable connection to chassis ground
            "GPO1C": True,
            "GPO2C": True,
            "GPO3C": False,
            "GPO1OD": False,
            "GPO2OD": False,
            "GPO3OD": False,
            'VB1MUX': 0,
            'VB2MUX': 0,
            'ACCI': acci_raw}
         },
        {"command": "$DELAY_MS$", "arguments": {"Delay": 25}},
    ]

    ####################################
    # Create short and long loop
    ####################################
    command_list = []
    # The ACC loop
    slow_loop = [
        {"command": "SNAP"},
        {"command": "RDVBACC", "map_key": "val_0"},
        {"command": "UNSNAP"},
    ]
    # Add slow loop as the first measurement
    command_list = command_list + slow_loop
    for n in range(acci):
        map_key = 'val_' + str(n)
        fast_loop = [
            {"command": "SNAP"},
            {"command": "RDVB", "map_key": map_key},
            {"command": "UNSNAP"},
            {"command": "$DELAY_MS$", "arguments": {"Delay": 1}},
        ]
        command_list = command_list + fast_loop

    # Run init command_list
    _ = bms.run_generic_command_list(init_list, board_list)
    # Run the rest of the command_list to get the measurements
    results = bms.run_generic_command_list(command_list, board_list, loop=loops)
    # Close the interface
    interface.close()

    ####################################
    # Prepare for print the results
    ####################################
    x = PrettyTable()
    x.field_names = ['VB1ACC', 'VB2ACC','VB1ACC/ACCI','VB2ACC/ACCI','VB1', 'VB2']
    for n in range(loops):
        vb1, vb2 = calc_vb(results[n]['val_0'][0]['VB1'],results[n]['val_0'][0]['VB2'])
        vb1acc, vb2acc = calc_vb(results[n]['val_0'][0]['VB1ACC'], results[n]['val_0'][0]['VB2ACC'])

        x.add_row([vb1acc, vb2acc, vb1acc/acci, vb2acc/acci, vb1, vb2])
        for m in range(acci):
            map_key = 'val_' + str(m)
            vb1, vb2 = calc_vb(results[n][map_key][0]['VB1'], results[n][map_key][0]['VB2'])
            x.add_row(['','','','',vb1, vb2])

    # Print the table
    print(x)


if __name__ == "__main__":
    main()