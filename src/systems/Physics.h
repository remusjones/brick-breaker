//
// Created by Remus on 29/06/2025.
//

#pragma once
#include "System.h"

class Physics : public System
{
public:
    explicit Physics(const std::string_view& name) : System(name) {}

    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
};
