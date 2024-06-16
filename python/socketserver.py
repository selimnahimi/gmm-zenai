import socket
import struct

PORT, HOST_IP = 8080, '127.0.0.1'
key = 4

VIEW_GRID_SIZE_X = 16
VIEW_GRID_SIZE_Y = 16
VIEW_GRID_SIZE_TOTAL = VIEW_GRID_SIZE_X * VIEW_GRID_SIZE_Y

# This comes from socket.hpp
FORMAT = {
    'endian': '>', # Big endian
    'map_name': 'f',
    'time_spent': 'I',
    'view_trace_distance_array': f'{VIEW_GRID_SIZE_TOTAL}d',
    'view_trace_classname_array': f'{VIEW_GRID_SIZE_TOTAL}f',
    'player_status': 'f',
    'equipped_weapon': 'f',
    'next_checkpoint_distance': 'd',
    'incoming_damage_direction': 'f',
    'incoming_damage_amount': 'f',
    'reward': 'i'
}

# With "".join syntax:
GAME_STATE_DATA_STRUCT_FORMAT = "".join([value for value in FORMAT.values()])

print("FORMAT: ", GAME_STATE_DATA_STRUCT_FORMAT)
print("SIZE: ", struct.calcsize(GAME_STATE_DATA_STRUCT_FORMAT))
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST_IP, PORT))
    s.listen()
    print("starting to listen")
    conn, addr = s.accept()
    with conn:
        print('Connected by', addr)
        conn.setblocking(True)
        buffer = b''
        while True:
            # receive data from client and unpack it
            data = conn.recv(struct.calcsize(GAME_STATE_DATA_STRUCT_FORMAT))
            if not data:
                break
            buffer += data
            print("buffer: ", buffer)
            while len(buffer) >= struct.calcsize(GAME_STATE_DATA_STRUCT_FORMAT):
                print("unpacking")
                game_state_data = struct.unpack(GAME_STATE_DATA_STRUCT_FORMAT, buffer[:struct.calcsize(GAME_STATE_DATA_STRUCT_FORMAT)])
                buffer = buffer[struct.calcsize(GAME_STATE_DATA_STRUCT_FORMAT):]
                print(game_state_data)
                print("Unpacked")