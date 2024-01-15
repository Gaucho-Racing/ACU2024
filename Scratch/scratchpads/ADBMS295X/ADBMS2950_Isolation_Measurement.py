# Copyright (c) 2023 Analog Devices, Inc. All Rights Reserved.
# This software is proprietary to Analog Devices, Inc. and its licensors.

"""
This Scratchpad performs isolation measurement. Make sure you follow the connection like in the powerpoint.
To measure isolation, you need at least 64 volts. Otherwise, the measurement will not succeed.

HV1 -> Bat+
Shunt- -> Bat-
HVISo1 -> via SWitch to Chassis GND

To simulate a fault, connect a resistor (for example 200K) between Chassis GND and Bat-. This will model an isolation fault.
If you want to test this quick and dirty, connect a resistor between HVIso1 and Shunt-
"""

from Engine.BMS import BMS
from Engine.Devices.ADBMS2950 import ADBMS2950
from Engine.Interfaces.USB_TO_SPI_BYTE import USB_TO_SPI_BYTE

class resistor():
    def __init__(self, resistor):
        self.R = resistor
        self.Y = 1 / resistor

# Define resistors
a = resistor(2.25e6)
b = resistor(2.25e6)
c = resistor(4.5e6)
d = resistor(12e3)
e = resistor(3.6e6)
f = resistor(9.1e3)
p = resistor(a.R + b.R)
n = resistor(c.R + d.R)

# Define calc functions
def calc_vx(vb2):
    return vb2 / d.R * (d.R + c.R)

def calc_vbat(vb1):
    return vb1 / f.R * (f.R + e.R)

def calc_iso_resistance(input_dict):
    Vbat1 = input_dict['vb1 q_off']
    Vbat2 = input_dict['vb1 q_on']
    Vx1 = input_dict['vb2 q_off']
    Vx2 = input_dict['vb2 q_on']

    # Prevents division by zero error
    if Vx1 == 0:
        Vx1 = 1e-9
    if Vx2 == 0:
        Vx2 = 1e-9

    print(f"VB1: {Vbat1}")
    print(f"VB2: {Vbat1}")
    print(f"Vx1: {Vx1}")
    print(f"Vx2: {Vx2}")

    Yiso_t = b.Y - p.Y + ((Vbat1 / Vx1) * p.Y)
    Yiso_b = 1 - ((Vbat1 / Vbat2) * (Vx2 / Vx1))
    Yt = (Yiso_t / Yiso_b) - n.Y - p.Y

    Yisop_t = b.Y + p.Y * ((Vbat1 / Vx1) - 1)
    Yisop_b = (Vbat2 / Vx2) - (Vbat1 / Vx1)
    Yisop = Yisop_t / Yisop_b

    res = {'Yt': Yt, 'Rt': 1 / Yt, 'Yp': Yisop, 'Rp': 1 / Yisop}

    return res

def main():
    interface = USB_TO_SPI_BYTE('COM5', 115200, stdout=False)
    bms = BMS(interface)
    board_list = [{'Device': ADBMS2950}]

    # Define Q settings
    Q2_ON = {
        "command": "WRCFGA", "arguments":
            {
                "GPO1C": True,
                "GPO2C": True,
                "GPO3C": True,
                "GPO1OD": False,
                "GPO2OD": False,
                "GPO3OD": False,
            }
    }

    Q2_OFF = {
        "command": "WRCFGA", "arguments":
            {
                "GPO1C": True,
                "GPO2C": True,
                "GPO3C": False,
                "GPO1OD": False,
                "GPO2OD": False,
                "GPO3OD": False,
            }
    }

    # Init list
    init_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "SRST"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 5}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "WRCFGA", "arguments": {
            "GPO1C": True,
            "GPO2C": True,
            "GPO3C": True,
            "GPO1OD": False,
            "GPO2OD": False,
            "GPO3OD": False,
            'VB1MUX': 0,
            'VB2MUX': 0,
            'ACCI': 4}},
        {"command": "ADI1", "arguments": {"OPT": 8, "RD": True}},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 15}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "SNAP"},
        {"command": "RDVB", "map_key": "val"},
        {"command": "RDVBACC", "map_key": "val"},
        {"command": "RDI", "map_key": "val"},
        {"command": "UNSNAP"},
    ]
    # Repeating command_list
    command_list = [
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        Q2_OFF,
        {"command": "$DELAY_MS$", "arguments": {"Delay": 5}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "SNAP"},
        {"command": "RDVB", "map_key": "Q2_OFF"},
        {"command": "RDVBACC", "map_key": "Q2_OFF"},
        {"command": "RDI", "map_key": "Q2_OFF"},
        {"command": "UNSNAP"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 5}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        Q2_ON,
        {"command": "$DELAY_MS$", "arguments": {"Delay": 5}},
        {"command": "$SPI_WAKEUP$", "arguments": {"Wakeup Time": 400}},
        {"command": "SNAP"},
        {"command": "RDVB", "map_key": "Q2_ON"},
        {"command": "RDVBACC", "map_key": "Q2_ON"},
        {"command": "RDI", "map_key": "Q2_ON"},
        {"command": "UNSNAP"},
        {"command": "$DELAY_MS$", "arguments": {"Delay": 5}},
    ]

    # Run initialization
    _ = bms.run_generic_command_list(init_list, board_list)

    # Run isolation measurement
    results = bms.run_generic_command_list(command_list, board_list)
    interface.close()

    # Process results
    vb1_on = results['Q2_ON'][0]['VB1']
    vb2_on = results['Q2_ON'][0]['VB2']
    vb1_off = results['Q2_OFF'][0]['VB1']
    vb2_off = results['Q2_OFF'][0]['VB2']

    # Save results in dict
    res = {'vb1 q_on': calc_vx(vb1_on), 'vb2 q_on': calc_vbat(vb2_on), 'vb1 q_off': calc_vbat(vb1_off),
           'vb2 q_off': calc_vbat(vb2_off)}

    # Calculate isolation
    resistance = calc_iso_resistance(res)
    print(f"Rtotal = {round(resistance['Rt'])} Ohm")


if __name__ == "__main__":
    main()