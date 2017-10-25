#include "resourcemanagerfactory.h"
#include "og_resourcemanager.h"

std::unique_ptr<og::IResourceManager> ResourceManagerFactory::Create()
{
    return std::unique_ptr<og::IResourceManager>(new OGResourceManager);
}
