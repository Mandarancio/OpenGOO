#pragma once

#include "GameEngine/iresourcemanagerfactory.h"

class ResourceManagerFactory : public og::IResourceManagerFactory
{
    std::unique_ptr<og::IResourceManager> Create();
};
