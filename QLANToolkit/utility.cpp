#include "utility.h"
#include <QDir>
#include <QTime>

void Utility::CreateFilePath(QString FileName)
{

    QDir Dir(FileName);
    QStringList Dirs=Dir.absolutePath().split("/");

    QString DirPath;
    for(int i=0;i<Dirs.size()-1;++i)
    {
        DirPath+=(Dirs[i]+"/");

        QDir CurDir(DirPath);
        if(!CurDir.exists())
        {
           CurDir.mkdir(".");
        }


    }
}

QString Utility::GetSizeDescription(quint64 Size)
{

        float Num = Size;
        QStringList List;
        List << "KB" << "MB" << "GB" << "TB";

        QStringListIterator Iter(List);
        QString Unit("B");

         while(Num >= 1024.0 && Iter.hasNext())
          {
             Unit = Iter.next();
             Num /= 1024.0;
         }

         return QString().setNum(Num,'f',2)+" "+Unit;

}

int Utility::RandomInteger(int Min, int Max)
{
   // qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    return qrand()%(Max-Min)+Min;
}
