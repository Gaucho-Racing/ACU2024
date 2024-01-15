from Engine.BMS import BMS
from Engine.Devices.ADBMS6830 import ADBMS6830
# from Engine.Devices.ADBMS2950 import ADBMS2950
from Engine.Interfaces.Virtual_SPI import Virtual_SPI
from pprint import pprint

num_monitors = 40

def error_check():
    
    pass

def main():
    interface = Virtual_SPI('data.txt')
    bms = BMS(interface)
    board_list = []
    for i in range(num_monitors):
        board_list.append({'Device': ADBMS6830})
    cells = range(1,17) #Start at cell 1 and end at cell 16
    
    init_list = [
        
    ]
    
    loop_list = [
        
    ]
    
    
    board_list = [{'Device': ADBMS6830},{'Device': ADBMS6830}]
    
if __name__ == "__main__":
    main() 