#priority is 6 for cyclic, 7 for single shot, see iso175 data sheet for how these are generated
def calc_can_id(pgn, priority=7):
    source_address = 244
    id = (priority << 26) + (pgn << 8) + source_address
    return id
    print("id: " + hex(id).upper())

#for decimal: int("<num>"", 16)

id = calc_can_id(61184)
print("id (hex): " + hex(id).upper() + "\n id (dec): " + str(id))