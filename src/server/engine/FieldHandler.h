#pragma once
#include "Basic/Singleton.h"

#include <vector>

class FieldObject;

class FieldHandler : public Singleton<FieldHandler>
{
    friend class Singleton<FieldHandler>;

private:
    std::vector<FieldObject*> m_activeFields;
    FieldHandler() = default;

public:
    ~FieldHandler();

    void HandleFields();
    void AddField(FieldObject* newField);
};