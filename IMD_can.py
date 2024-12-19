#priority is 6 for cyclic, 7 for single shot, see iso175 data sheet for how these are generated
def calc_can_id(pgn, priority=7):
    source_address = 244
    id = (priority << 26) + (pgn << 8) + source_address  
    print("id: " + hex(id))

calc_can_id(61184)

    