# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Scratchpad demonstrates the COMM_BK functionality with 2x ADBMS6832 in a daisychain configuration.
A readout (RDSTATA) and clear (CLRAUX) of the VREF2 voltage is used as an example.
In addition the CCNT (command count) is printed to help understanding of which messages get registered.
The results are printed with pretty print.
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS6832 import ADBMS6832
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from pprint import pprint

def main():
    # Create the interface. This is the standard interface. Others can be found in the Interfaces directory
    interface = USB_TO_SPI_BYTE('COM4', 115200, stdout=False)
    # Create the BMS Object
    bms = BMS(interface)
    # Create the board list. Here you can add multiple devices in a daisychain
    board_list = [{'Device': ADBMS6832}, {'Device': ADBMS6832}]

    command_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "SRST"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 2}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},

        {"command": "ADAX", "arguments": {"CH": 16}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 5}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 20}},
        {"command": "RDSTATA", "map_key": "Cells1_RST"},
        {"command": "WRCFGA", "board_list": [{"COMM_BK": False}, {"COMM_BK": False}]},
        {"command": "CLRAUX"},
        {"command": "WRCFGA", "board_list": [{"COMM_BK": False}, {"COMM_BK": False}]},
        {"command": "RDSTATA", "map_key": "Cells1_CLR"},

        {"command": "SRST"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 2}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 20}},

        {"command": "ADAX", "arguments": {"CH": 16}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 5}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 20}},
        {"command": "RDSTATA", "map_key": "Cells2_RST"},
        {"command": "WRCFGA", "board_list": [{"COMM_BK": True}, {"COMM_BK": False}]},
        {"command": "CLRAUX"},
        {"command": "WRCFGA", "board_list": [{"COMM_BK": False}, {"COMM_BK": False}]},
        {"command": "RDSTATA", "map_key": "Cells2_CLR"},

        {"command": "WRCFGA", "board_list": [{"COMM_BK": False}, {"COMM_BK": False}]},
    ]

    # Run generic command_list
    results = bms.run_generic_command_list(command_list, board_list)
    # Close the interface
    interface.close()
    # Pretty print the results
    pprint(results)

    print("1. Without COMM_BK")
    print("DUT1 VREF2: ", results["Cells1_RST"][0]["VREF2"], " CCNT:", results["Cells1_RST"][0]["CCNT"])
    print("DUT2 VREF2: ", results["Cells1_RST"][1]["VREF2"], " CCNT:", results["Cells1_RST"][1]["CCNT"])
    print("CLRAUX")
    print("DUT1 VREF2: ", results["Cells1_CLR"][0]["VREF2"], " CCNT:", results["Cells1_CLR"][0]["CCNT"])
    print("DUT2 VREF2: ", results["Cells1_CLR"][1]["VREF2"], " CCNT:", results["Cells1_CLR"][1]["CCNT"])

    print("")

    print("2. With COMM_BK:")
    print("DUT1 VREF2: ", results["Cells2_RST"][0]["VREF2"], " CCNT:", results["Cells2_RST"][0]["CCNT"])
    print("DUT2 VREF2: ", results["Cells2_RST"][1]["VREF2"], " CCNT:", results["Cells2_RST"][1]["CCNT"])
    print("CLRAUX")
    print("DUT1 VREF2: ", results["Cells2_CLR"][0]["VREF2"], " CCNT:", results["Cells2_CLR"][0]["CCNT"])
    print("DUT2 VREF2: ", results["Cells2_CLR"][1]["VREF2"], " CCNT:", results["Cells2_CLR"][1]["CCNT"])



if __name__ == "__main__":
    main()