#pragma once

#include "og_xmlconfig.h"

struct ConfigLoader
{
    template<typename T>
    static void Load(T& a_data, const QString& a_filename)
    {
        OGXmlConfig<TagParser<typename T::element_type>> conf;
        if (!conf.Load(a_filename))
        {
            return;
        }

        a_data.reset(conf.Parse().release());
    }
};
