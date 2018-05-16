#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QStringListIterator>

namespace Utility
{
    QString GetSizeDescription(quint64 Size);


    int RandomInteger(int Min,int Max);

    void CreateFilePath(QString FileName);

}

#endif // UTILITY_H
