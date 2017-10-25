#pragma once

#include "iresourcemanager.h"

namespace og
{
struct IResourceManagerFactory
{
    virtual ~IResourceManagerFactory()
    {
    }

    virtual std::unique_ptr<IResourceManager> Create() = 0;
};
}
