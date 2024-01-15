# Copyright (c) 2023 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Scratchpad performs SM VREF OOR.
The safety mechanism SM_VREF_OOR detects mutual drift between the VREF1 and the VREF2 references.
It also detects gain errors of the V1ADC and V2ADC.

Internally, the ADBMS295x loops a divided version of the VREF2 voltage to the input multiplexers of the primary and secondary Voltage ADCs.
The BMC triggers the V1ADC and V2ADC periodically to measure that value and confirms that resulting two voltage values match mutually and are within the expected range.
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS2950 import ADBMS2950
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from pprint import pprint
from prettytable import PrettyTable
import numpy as np
from termcolor import colored
def main():
    # Create the interface. This is the standard interface. Others can be found in the Interfaces directory
    interface = USB_TO_SPI_BYTE('COM12', 115200, stdout=False)
    # Create the BMS Object
    bms = BMS(interface=interface)
    # Create the board list. Here you can add multiple devices in a daisychain
    board_list = [{'Device': ADBMS2950}]
    x = PrettyTable()
    n_rounds = 20
    out_of_bound_threshold = 12e-3
    dif_threshold = 6e-3

    cmd_list = [
        {'command': '$SPI_SET_FREQUENCY_kHz$', 'arguments': {'Frequency': 2000}},
        {'command': '$SPI_WAKEUP$', 'arguments': {'Wakeup Time': 500}},
        {"command": "SRST"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {'command': '$SPI_WAKEUP$', 'arguments': {'Wakeup Time': 500}},
        {"command": "ADV", "arguments": {'VCH': 8}},
        {"command": "PLV"},
    ]

    [cmd_list.extend([
        {"command": "RDV2D", "map_key": f"SM_VREF_OOR{n_round}"},
        {"command": "ADV", "arguments": {'VCH': 8}},
        {"command": "PLV"},
    ]) for n_round in range(n_rounds)]

    map_keys = [f"SM_VREF_OOR{n_round}" for n_round in range(n_rounds)]
    results = bms.run_generic_command_list(cmd_list, board_list, include_raw=False)
    interface.close()

    x.field_names = ['dif', 'VREF2A', 'VREF2B']

    dif_lst = []
    err_cnt = 0
    oor_cnt_a = []
    oor_cnt_b = []
    for map_key in map_keys:
        a = results[map_key][0]['VREF2A']
        b = results[map_key][0]['VREF2B']
        if (3 - a) >= out_of_bound_threshold:
            oor_cnt_a.append(a)
        if (3 - b) >= out_of_bound_threshold:
            oor_cnt_b.append(b)


        dif = a - b
        dif_lst.append(dif)
        if abs(dif) > dif_threshold:
            x.add_row([colored(dif, 'red'), a, b])
            err_cnt += 1
        else:
            x.add_row([colored(dif, 'green'), a, b])

    print(x)
    print("----------------------------------------------------------------------")
    if err_cnt > 0:
        print(colored(f"Threshold violation {err_cnt} time(s) | Variance: {np.var(dif_lst)} | Average {np.mean(dif_lst)}", 'red'))
    else:
        print(colored(f"Threshold violation {err_cnt} times | Variance: {np.var(dif_lst)} | Average {np.mean(dif_lst)}", 'green'))

    if len(oor_cnt_a) > 0:
        print(colored(f"VREF2A is out of bound", 'red'))
        print(oor_cnt_a)
        print(colored(f"a average_oor: {np.mean(oor_cnt_a)}", 'yellow'))
    else:
        print(colored("No out of bound errors detected on VREF2A", 'green'))

    if len(oor_cnt_b) > 0:
        print(colored(f"VREF2B is out of bound", 'red'))
        print(oor_cnt_b)
        print(colored(f"b average_oor: {np.mean(oor_cnt_b)}", 'yellow'))
    else:
        print(colored("No out of bound errors detected on VREF2B", 'green'))


if __name__ == "__main__":
    main()