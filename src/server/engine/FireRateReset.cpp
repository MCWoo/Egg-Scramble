#include "FireRateReset.h"

#include "Weapon.h"

FireRateReset::~FireRateReset()
{
    // Don't delete weapons??
}

void FireRateReset::ResetWeapons()
{
    auto it = m_usedWeapons.begin();
    // Check through list of used weapons
    while (it != m_usedWeapons.end())
    {
        // printf("checking weapon.  Current World Tick:  %u, nextFireTick %u \n", currentWorldTick,
        // (*it)->nextFireTick);

        // reset fire flag if it can be fired again
        if ((*it)->m_nextFireTick == m_currentWorldTick)
        {
            (*it)->m_fireFlag = 1;
            it = m_usedWeapons.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void FireRateReset::AddWeapon(Weapon* weapon)
{
    m_usedWeapons.push_back(weapon);
}
