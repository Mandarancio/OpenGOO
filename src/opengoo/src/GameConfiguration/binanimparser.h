#pragma once

#include <memory>

class QByteArray;
class QString;

struct AnimationData;

class BinAnimParser
{
public:
    static std::unique_ptr<AnimationData> Parse(const QString& aFileName);

    static std::unique_ptr<AnimationData> Parse(const QByteArray& aData);

    static std::unique_ptr<AnimationData> Parse(const char* aData, int aSize);
};
