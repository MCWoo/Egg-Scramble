#pragma once
#include "Basic/Singleton.h"

#include <vector>

class Weapon;

class FireRateReset : public Singleton<FireRateReset>
{
    friend class Singleton<FireRateReset>;

private:
    std::vector<Weapon*> m_usedWeapons;
    FireRateReset() = default;

public:
    ~FireRateReset();

    void ResetWeapons();
    void AddWeapon(Weapon* weapon);

    unsigned int m_currentWorldTick = 0;
};