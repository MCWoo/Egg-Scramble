#include "Network/GameData.h"

#include <memory>   // unique_ptr
#include <vector>

class World;

typedef std::vector<PosInfo> pos_list;

class Engine
{
public:
    Engine();
    ~Engine();

    World* GetWorld() { return m_world.get(); };
    // generate map
    void InitWorld(int num_players);

    // Spawns at random positions
    void SpawnRandomPlayer(int player, int team, int skin);
    void SpawnRandomFlag();

    int RandomNum(int mod);

    // spawn once all players are "ready"
    void SendPreSpawn(int n);
    bool HasInitialSpawned() { return initialSpawned; };
    void SetInitialSpawned(bool b) { initialSpawned = b; }
    // move player
    // attack player

private:
    bool initialSpawned = false;
    std::unique_ptr<World> m_world; // used to communicate with clients

    /* maybe put the world representation in this class so we can
    manipulate it and use ServerGame to send packets to clients*/
};