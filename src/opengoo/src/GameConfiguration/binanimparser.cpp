#include "binanimparser.h"

#include <QFile>
#include <QString>
#include <QDebug>

#include "animationdata.h"

class DataReader
{
public:
    template<typename T>
    static void Read(const char* aData, T* aOut)
    {
        memcpy(aOut, aData, sizeof(T));
    }

    template<typename T>
    static void Read(const char* const aData, std::vector<T>* aOut)
    {
        auto data = aData;
        for (size_t i = 0; i < aOut->size(); ++i)
        {
            Read(data, &aOut->data()[i]);
            data = Next<T>(data);
        }
    }

private:
    template<typename T>
    static const char* Next(const char* aData)
    {
        return aData + sizeof(T);
    }
};

struct BinImageAnimation
{
    int hasColor;
    int hasAlpha;
    int hasSound;
    int hasTransform;
    int numTransforms;
    int numFrames;

    int transformTypesOffset;
    int frameTimesOffset;
    int xformFramesOffset;
    int alphaFramesOffset;
    int colorFramesOffset;
    int soundFramesOffset;
    int stringTableOffset;
};

template<typename T>
inline int VectorSize(const std::vector<T>& aData)
{
    return sizeof(T) * aData.size();
}

std::unique_ptr<AnimationData> BinAnimParser::Parse(const char* aData, int aSize)
{
    BinImageAnimation anim;

    auto Checker = [](const char* aEnd)
    {
        return [aEnd](const char* aBegin, std::ptrdiff_t aSize)
        {
            if (aSize > (aEnd - aBegin))
            {
                throw std::runtime_error("source is smaller than destination");
            }
        };
    };

    auto CheckSourceSize = Checker(aData + aSize);

    auto FrameReader = [aData, CheckSourceSize](int aNumFrames)
    {
        return [aData, aNumFrames, CheckSourceSize](int aOffset, std::vector<AnimationData::KeyFrame>* aFrame)
        {
            std::vector<int> framePointer(aNumFrames);
            {
                auto const begin = aData + aOffset;
                CheckSourceSize(begin, VectorSize(framePointer));
                DataReader::Read(begin, &framePointer);
            }

            aFrame->resize(aNumFrames);

            for (int i = 0; i < aNumFrames; ++i)
            {
                auto const begin = aData + framePointer[i];
                CheckSourceSize(begin, sizeof(aFrame[i]));
                DataReader::Read(begin, &(*aFrame)[i]);
            }
        };
    };

    CheckSourceSize(aData, sizeof(anim));
    DataReader::Read(aData, &anim);

    std::unique_ptr<AnimationData> animData(new AnimationData);

    animData->frameTime.resize(anim.numFrames);
    {
        auto const begin = aData + anim.frameTimesOffset;
        CheckSourceSize(begin, VectorSize(animData->frameTime));
        DataReader::Read(begin, &animData->frameTime);
    }

    auto ReadFrames = FrameReader(anim.numFrames);

    if (anim.hasTransform)
    {
        {
            std::vector<int> transformTypes(anim.numTransforms);
            {
                auto const begin = aData + anim.transformTypesOffset;
                CheckSourceSize(begin, VectorSize(transformTypes));
                DataReader::Read(begin, &transformTypes);
            }

            animData->transformFrame.resize(anim.numTransforms);
            for (int i = 0; i < anim.numTransforms; ++i)
            {
                animData->transformFrame[i].type = (AnimationData::TransformType)transformTypes[i];
            }
        }

        std::vector<int> frameOffset(anim.numTransforms);
        {
            auto const begin = aData + anim.xformFramesOffset;
            CheckSourceSize(begin, VectorSize(frameOffset));
            DataReader::Read(begin, &frameOffset);
        }

        for (int i = 0; i < anim.numTransforms; ++i)
        {
            ReadFrames(frameOffset[i], &animData->transformFrame[i].frame);
        }
    }

    if (anim.hasAlpha)
    {
        ReadFrames(anim.alphaFramesOffset, &animData->alphFrame);
    }

    if (anim.hasColor)
    {
        ReadFrames(anim.colorFramesOffset, &animData->colorFrame);
    }

    if (anim.hasSound)
    {
        ReadFrames(anim.soundFramesOffset, &animData->soundFrame);
    }

    if (anim.stringTableOffset != 0)
    {
        animData->stringTable.assign(aData + anim.stringTableOffset);
    }

    return animData;
}

std::unique_ptr<AnimationData> BinAnimParser::Parse(const QByteArray& aData)
{
    return Parse(aData.data(), aData.size());
}

std::unique_ptr<AnimationData> BinAnimParser::Parse(const QString& aFileName)
{
    QFile file(aFileName);
    if (!file.open(QFile::ReadOnly))
    {
        return nullptr;
    }

    return Parse(file.readAll());
}
