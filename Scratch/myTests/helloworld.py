from Engine.BMS import BMS
from Engine.Devices.ADBMS2950 import ADBMS2950
from Engine.Interfaces.Virtual_SPI import Virtual_SPI
from pprint import pprint

def error_check():
    
    pass

def main():
    interface = Virtual_SPI('data.txt')
    bms = BMS(interface)
    board_list = [{'Device': ADBMS2950}]
    
if __name__ == "__main__":
    main() 