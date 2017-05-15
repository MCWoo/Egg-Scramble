#pragma once

#include <cstdint>

class Serializable
{
    virtual void Serialize(std::uint8_t* buffer) = 0;
    virtual void Deserialize(std::uint8_t* buffer) = 0;
};