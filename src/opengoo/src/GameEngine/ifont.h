#pragma once

namespace og
{
struct IFont
{
    virtual ~IFont()
    {
    }
};

typedef std::shared_ptr<IFont> IFontSPtr;
}
