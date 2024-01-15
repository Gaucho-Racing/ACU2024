# Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Scratchpad shows how to activate the crash signal on Tiger (ADBMS2950) rev C or higher.
Basicly it boils down to changing the threshold on OCx and reset OCEN. In other words,
WRCFGB(OCxTH= 0 for crash signal, or some other value if you want to change it)
WRCFGA(OCEN= 0 to stop the OCEN)
WRCFGA(OCEN= 1 to start the OCEN)
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS2950 import ADBMS2950
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE
from pprint import pprint

def main():
    interface = USB_TO_SPI_BYTE('COM29', 115200, stdout=False)
    bms = BMS(interface)
    board_list = [{'Device': ADBMS2950}]

    OCL = 0
    OCH = 3
    # OCMODE: 1 = PWM
    # OCMODE: 3 = Static
    OCMODE = 1

    OC_test = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 10}},
        {"command": "RDFLAG", "map_key": "aa"},
        {"command": "RDSTAT", "map_key": "aa"},
        {"command": "RDCFGA", "map_key": "aa"},
        {"command": "RDCFGB", "map_key": "aa"},
        {"command": "CLRFLAG", "arguments": {
            "OCAL": 1,
            "OCBL": 1,
            "OC1L": 1,
            "OC2L": 1,
            "OC3L": 1,
            "OCAGD": 1,
            "OCBGD": 1,
            "OCMM": 1,
        }},
        {"command": "RDFLAG", "map_key": "bb"},
        {"command": "RDSTAT", "map_key": "bb"},
        {"command": "RDCFGA", "map_key": "bb"},
        {"command": "RDCFGB", "map_key": "bb"},
        {"command": "WRCFGB", "arguments": {
            "OCOD": 0,
            "OCAX": 0,
            "OCBX": 0,
            "OCMODE": OCMODE,
            "OCDGT": 3,
            "OC1TH": OCH,
            "OC2TH": OCH,
            "OC3TH": OCH,
        }},
        {"command": "WRCFGA", "arguments": {"OCEN": 0}},
        {"command": "WRCFGA", "arguments": {"OCEN": 1}},
        {"command": "RDFLAG", "map_key": "cc"},
        {"command": "RDSTAT", "map_key": "cc"},
        {"command": "RDCFGA", "map_key": "cc"},
        {"command": "RDCFGB", "map_key": "cc"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 512}},
        {"command": "$SPI_WAKEUP$", "arguments": {"wakeup_time": 400}},
        {"command": "RDFLAG", "map_key": "dd"},
        {"command": "RDSTAT", "map_key": "dd"},
        {"command": "RDCFGA", "map_key": "dd"},
        {"command": "RDCFGB", "map_key": "dd"},
        {"command": "WRCFGB", "arguments": {
            "OCOD": 0,
            "OCAX": 0,
            "OCBX": 0,
            "OCMODE": OCMODE,
            "OCDGT": 3,
            "OC1TH": OCL,
            "OC2TH": OCL,
            "OC3TH": OCL,
        }},
        {"command": "WRCFGA", "arguments": {"OCEN": 0}},
        {"command": "WRCFGA", "arguments": {"OCEN": 1}},
        {"command": "RDFLAG", "map_key": "ee"},
        {"command": "RDSTAT", "map_key": "ee"},
        {"command": "RDCFGA", "map_key": "ee"},
        {"command": "RDCFGB", "map_key": "ee"},
        {"command": "RDFLAG", "map_key": "ff"},
        {"command": "RDSTAT", "map_key": "ff"},
        {"command": "RDCFGA", "map_key": "ff"},
        {"command": "RDCFGB", "map_key": "ff"},
        {"command": "$DELAY_MS$", "arguments": {"delay": 100}},
        {"command": "$SPI_WAKEUP$", "arguments": {"wakeup_time": 400}},
        {"command": "SRST"},
        {"command": "$DELAY_MS$", "arguments": {"delay": 10}},
        {"command": "$SPI_WAKEUP$", "arguments": {"wakeup_time": 400}},
        {"command": "RDCFGB", "map_key": "TRASH"},
    ]

    time_list = ['aa','bb','cc','dd','ee','ff']
    keys = ["OCAP","OCBP","OCAL","OCBL"]

    results = bms.run_generic_command_list(OC_test, board_list)
    pprint(results)

    for t in time_list:
        print("--------")
        print(t)
        print(" ")
        for key in keys:
            print("%5s | %3s" %(key,results[t][0][key]))

    interface.close()

if __name__ == "__main__":
    main()