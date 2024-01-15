# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Demo measures Cell for 100 loops with Lion rev C (ADBMS6830) or higher.
The results are shown with matplotlib in a graph.
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS6830 import ADBMS6830
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
import matplotlib.pyplot as plt
import Engine.plthelper as plthelper

def main():
    # Init the helper function that makes plots look nicer
    plthelper.init()
    interface = USB_TO_SPI_BYTE('COM29', 115200, stdout=False)
    bms = BMS(interface)
    board_list = [{'Device': ADBMS6830}]

    cells = range(1,17) # Start at cell 1 and end at cell 16
    base_keys = ['C'] # C=Cell, AC=Average Cell, FC=Filtered Cell, S=Spins
    loops = 100
    device = 0 # Only 1 device in the chain
    delay_between_measurements = 8 # in ms

    # Init the lion
    init_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "WRCFGA", "arguments": {"REFON": True, 'FC': 7}},
        # Activate ADC Cell and ADC Spins
        {"command": "ADCV", "arguments": {"CONT": True}},
        # Give some time to startup the adc's
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
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
        {"command": "RDFCA", "map_key": "val"},
        {"command": "RDFCB", "map_key": "val"},
        {"command": "RDFCC", "map_key": "val"},
        {"command": "RDFCD", "map_key": "val"},
        {"command": "RDFCE", "map_key": "val"},
        {"command": "RDFCF", "map_key": "val"},
        {"command": "RDACA", "map_key": "val"},
        {"command": "RDACB", "map_key": "val"},
        {"command": "RDACC", "map_key": "val"},
        {"command": "RDACD", "map_key": "val"},
        {"command": "RDACE", "map_key": "val"},
        {"command": "RDACF", "map_key": "val"},
        {"command": "RDSVA", "map_key": "val"},
        {"command": "RDSVB", "map_key": "val"},
        {"command": "RDSVC", "map_key": "val"},
        {"command": "RDSVD", "map_key": "val"},
        {"command": "RDSVE", "map_key": "val"},
        {"command": "RDSVF", "map_key": "val"},
        # Release registers
        {"command": "UNSNAP"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": delay_between_measurements}},
    ]

    # Init lion
    _ = bms.run_generic_command_list(init_list, board_list)
    # Loop lion
    results = bms.run_generic_command_list(loop_list, board_list, loop=loops)
    interface.close()

    # Handling data
    # Create keys for printing
    keys = []
    for base_key in base_keys:
        for i in cells:
            keys.append(base_key + str(i) + "V")

    # Loop over keys
        # Loop over loops to create a list for every cell.
        # Then plot this list of the cell
    for key in keys:
        cell_lst = []
        for loop in range(loops):
            val = results[loop]['val'][device][key]
            cell_lst.append(val)
        plt.plot(cell_lst, label=key)

    # Create labels, title, legend and activate the grid.
    plt.xlabel("Sample")
    plt.ylabel("Voltage")
    plt.title("Cell voltages")
    plt.grid("on")
    plt.legend()
    plt.show()

if __name__ == "__main__":
    main()