TEST = """
    # Z-flag: 1
    add r0, r1     
"""

EXPECTED_RESULTS = {
    "r0": 0,
    "r1": 0,
    "sregZ": 1,
    "sregV": 0,
}