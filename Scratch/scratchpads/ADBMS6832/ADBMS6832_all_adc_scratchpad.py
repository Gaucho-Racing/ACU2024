# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Scratchpad measures Cell, Average Cell, Filtered Cell and SPINS for n-loops with Lion18 (ADBMS6832).
The results are printed in a csv file.
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS6832 import ADBMS6832
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
import csv

def main():
    # Open the interface
    interface = USB_TO_SPI_BYTE('COM29', 115200, stdout=False)
    # Create BMW Object
    bms = BMS(interface)
    # Add device to board list. For multiple boards use: [{'Device': ADBMS6832},{'Device': ADBMS6832}]
    board_list = [{'Device': ADBMS6832}]

    # Parameters for printing
    path_to_file = "data.csv"
    cells = range(1,19) #Start at cell 1 and end at cell 18
    base_keys = ['C', 'AC', 'FC', 'S'] #Cell, Average Cell, Filtered Cell, Spins
    loops = 50
    device = 0 #Only 1 device in the chain
    delay_between_measurements = 10 #in ms
    # Init the lion

    init_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "WRCFGA", "arguments": {"REFON": True}},
        # Activate ADC Cell and ADC Spins
        {"command": "ADCV", "arguments": {"CONT": True}},
        {"command": "ADSV", "arguments": {"CONT": True}},
        # Give some time to startup the adc's
        {"command": "$DELAY_MS$", "arguments": {"Delay": 50}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
    ]

    # The list that will be looped
    loop_list = [
        # Stop updating registers
        {"command": "SNAP"},
        # Read Cells
        {"command": "RDCVA", "map_key": "val"},
        {"command": "RDCVB", "map_key": "val"},
        {"command": "RDCVC", "map_key": "val"},
        {"command": "RDCVD", "map_key": "val"},
        {"command": "RDCVE", "map_key": "val"},
        {"command": "RDCVF", "map_key": "val"},
        # Read CAVG
        {"command": "RDACA", "map_key": "val"},
        {"command": "RDACB", "map_key": "val"},
        {"command": "RDACC", "map_key": "val"},
        {"command": "RDACD", "map_key": "val"},
        {"command": "RDACE", "map_key": "val"},
        {"command": "RDACF", "map_key": "val"},
        # Read CFIL
        {"command": "RDFCA", "map_key": "val"},
        {"command": "RDFCB", "map_key": "val"},
        {"command": "RDFCC", "map_key": "val"},
        {"command": "RDFCD", "map_key": "val"},
        {"command": "RDFCE", "map_key": "val"},
        {"command": "RDFCF", "map_key": "val"},
        # Read Cells
        {"command": "RDCVA", "map_key": "val"},
        {"command": "RDCVB", "map_key": "val"},
        {"command": "RDCVC", "map_key": "val"},
        {"command": "RDCVD", "map_key": "val"},
        {"command": "RDCVE", "map_key": "val"},
        {"command": "RDCVF", "map_key": "val"},
        # Read Spins
        {"command": "RDSVA", "map_key": "val"},
        {"command": "RDSVB", "map_key": "val"},
        {"command": "RDSVC", "map_key": "val"},
        {"command": "RDSVD", "map_key": "val"},
        {"command": "RDSVE", "map_key": "val"},
        {"command": "RDSVF", "map_key": "val"},
        # Release registers
        {"command": "UNSNAP"},
        # {"command": "$DELAY_MS$", "arguments": {"delay": delay_between_measurements}},
    ]
    # Init lion
    init = bms.run_generic_command_list(init_list, board_list)
    # Loop lion
    results = bms.run_generic_command_list(loop_list, board_list, loop=loops)
    interface.close()

    # Handling data
    # Create keys for printing
    keys = []
    for base_key in base_keys:
        for i in cells:
            keys.append(base_key + str(i) + "V")

    # Open file
    f = open(path_to_file, 'w', newline='')

    # Define CSV Writer and create headers
    writer = csv.DictWriter(f, fieldnames=keys, delimiter=',')
    writer.writeheader()

    # Start writing to csv and print
    for i in range(loops):
        print("\r\nRound: %02d" %i)
        rresults = {}
        for key in keys:
            val = results[i]['val'][device][key]
            print("%s  |  %.4f" %(key, val))
            a = "%.4f" %val
            rresults[key] = a
        writer.writerow(rresults)

if __name__ == "__main__":
    main()