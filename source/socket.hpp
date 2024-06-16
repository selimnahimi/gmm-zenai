#ifndef SOCKET_H_
#define SOCKET_H_
#define VIEW_GRID_SIZE_X 16
#define VIEW_GRID_SIZE_Y 16
namespace serversock
{
    struct gameStateData
    {
        float map_name;
        double time_spent;

        double view_trace_distance_array[VIEW_GRID_SIZE_X * VIEW_GRID_SIZE_Y];
        float view_trace_classname_array[VIEW_GRID_SIZE_X * VIEW_GRID_SIZE_Y];

        float player_status;
        float equipped_weapon;
        double next_checkpoint_distance;
        float incoming_damage_direction;
        float incoming_damage_amount;

        int reward;
    };

    struct gameActionData
    {
        float look_direction;
        float look_strength;
        float move_direction;
        bool attack1;
        bool attack2;
        bool reload;
        bool interact;
        bool jump;
        bool crouch;
        bool sprint;
    };

    void createConnection();
    int readValues(gameActionData *a);
    void writeValues(gameStateData *a);
}
#endif