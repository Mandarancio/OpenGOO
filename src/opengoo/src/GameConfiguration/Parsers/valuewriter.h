#pragma once

#include <QDomAttr>
#include <QPointF>
#include <QColor>
#include <QSizeF>

#include "OGLib/optional.h"

struct ValueWriter
{
    static QPointF StringToPointF(const QString& aPosition)
    {
        QPointF pos;
        auto sl = aPosition.split(",");
        for (int i = 0; i < sl.size(); ++i)
        {
            auto val = sl.at(i).toDouble();
            switch (i)
            {
            case 0:
                pos.setX(val);
                break;
            case 1:
                pos.setY(val);
                return pos;
            }
        }

        return pos;
    }

    static QPointF StringToPointF(const QString& x, const QString& y)
    {
        return QPointF(x.toDouble(), y.toDouble());
    }

    static QPointF StringToPointF(const QDomAttr& aAttribute)
    {
        return StringToPointF(aAttribute.value());
    }

    static QPoint StringToPoint(const QString& aPosition)
    {
        QPoint pos;
        auto sl = aPosition.split(",");
        for (int i = 0; i < sl.size(); ++i)
        {
            auto val = sl.at(i).toInt();
            switch (i)
            {
            case 0:
                pos.setX(val);
                break;
            case 1:
                pos.setY(val);
                return pos;
            }
        }

        return pos;
    }

    static QColor StringToColor(const QString& color)
    {
        QStringList list = color.split(",");

        if (list.size() == 3)
        {
            return QColor(list.at(0).toInt(), list.at(1).toInt(), list.at(2).toInt());
        }
        else
        {
            return QColor();
        }
    }

    static QSizeF StringToSize(const QString width, const QString height)
    {
        return QSizeF(width.toDouble(), height.toDouble());
    }

    static bool StringToBool(const QString& value)
    {
        return (value == "true") ? true : false;
    }

    static void WriteValue(int& aData, const QDomAttr& aAttribute)
    {
        aData = aAttribute.value().toInt();
    }

    static void WriteValue(float& aData, const QDomAttr& aAttribute)
    {
        aData = aAttribute.value().toFloat();
    }

    static void WriteValue(QPointF& aData, const QDomAttr& aAttribute)
    {
        aData = StringToPointF(aAttribute.value());
    }

    static void WriteValue(QPoint& aData, const QDomAttr& aAttribute)
    {
        aData = StringToPoint(aAttribute.value());
    }

    static void WriteValue(QStringList& aData, const QDomAttr& aAttribute)
    {
        aData = aAttribute.value().split(',');
    }

    static void WriteValue(QString& aData, const QDomAttr& aAttribute)
    {
        aData = aAttribute.value();
    }

    static void WriteValue(std::vector<float>& aData, const QDomAttr& aAttribute)
    {
        foreach (const auto& val, aAttribute.value().split(','))
        {
            aData.push_back(val.toFloat());
        }
    }

    static void WriteValue(QColor& aData, const QDomAttr& aAttribute)
    {
        aData = StringToColor(aAttribute.value());
    }

    static void WriteValue(oglib::Optional<bool>& aData, const QDomAttr& aAttribute)
    {
        aData = StringToBool(aAttribute.value());
    }

    static void WriteValue(oglib::Optional<QPointF>& aData, const QDomAttr& aAttribute)
    {
        aData = StringToPointF(aAttribute.value());
    }

    static void WriteValue(oglib::Optional<QColor>& aData, const QDomAttr& aAttribute)
    {
        aData = StringToColor(aAttribute.value());
    }

    static void WriteValue(oglib::Optional<float>& aData, const QDomAttr& aAttribute)
    {
        aData = aAttribute.value().toFloat();
    }

    static void WriteValue(bool& aData, const QDomAttr& aAttribute)
    {
        aData = StringToBool(aAttribute.value());
    }

    static void WriteValue(std::pair<bool, float>& aData, const QDomAttr& aAttribute)
    {
        aData.first = true;
        WriteValue(aData.second, aAttribute);
    }

    static void WriteValue(std::pair<bool, QPointF>& aData, const QDomAttr& aAttribute)
    {
        aData.first = true;
        WriteValue(aData.second, aAttribute);
    }

    static void WriteValue(float& aData, const QString& aValue)
    {
        aData = aValue.toFloat();
    }
};

