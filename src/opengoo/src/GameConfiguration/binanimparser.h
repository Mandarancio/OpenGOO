#pragma once

#include <memory>

class QByteArray;
class QString;

struct AnimationData;

class BinAnimParser
{
public:
    static std::unique_ptr<AnimationData> Parse(const QString& aFileName);

    template<typename T>
    static std::unique_ptr<AnimationData> Parse(const QByteArray& aData);
};
