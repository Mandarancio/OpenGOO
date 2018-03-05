#pragma once

template <typename T> struct Enumerator
{
    static T ToEnum(const QString& aType);
};
