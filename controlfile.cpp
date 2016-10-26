#include "controlfile.h"
#include <QtDebug>
#include <QFile>
#include <QString>
#include <QJsonDocument>

// return in ms
double getTimeTick(int hour, int minute, int second, int hundredMS)
{
    double ret = (hour * 60 * 60 + minute * 60 + second + hundredMS * 0.1) * 1000;
    return ret;
}

int CCtrlDeviceDocument::getCtrlCnt() const
{
    return m_root.size();
}

double CCtrlDeviceDocument::getTimeTickOfCtrlDeviceInfo(int index) const
{
    if(index >= m_root.size() && index < 0)
        return -1;

    QString thisone = m_root.at(index);
    QStringList timeAndData = thisone.split(":");
    QString time = timeAndData.at(0);

    int hour = (time.left(2)).toInt(NULL, 16);
    time = time.right(time.length()-2);

    int minute = (time.left(2)).toInt(NULL, 16);
    time = time.right(time.length()-2);

    int second = (time.left(2)).toInt(NULL, 16);
    time = time.right(time.length()-2);

    int hundredMS = (time.left(2)).toInt(NULL, 16);

    double tick = getTimeTick(hour, minute, second, hundredMS);
//    qDebug() << QString("time tick=%1").arg(tick);
    return tick;
}

CCtrlDeviceInfo CCtrlDeviceDocument::getCtrlDeviceInfo(int index) const
{
    QString val = m_root.at(index);
    QStringList timeAndData = val.split(":");
    QString data = timeAndData.at(1);

    QString translated;
    while(data.length()>=2){
        //  translate "31" -> 0x31 and store it in a BYTE
        QString two = data.left(2);
//        qDebug() << two << "-> " << two.toInt(NULL, 16);
        unsigned char achar = (unsigned char)(two.toInt(NULL, 16));
        translated.append(achar);
        data = data.right(data.length()-2);
    }

    CCtrlDeviceInfo ret;
    ret.init(translated);
    return ret;
}

void CCtrlDeviceDocument::updateCtrlDeviceInfo(int index, const CCtrlDeviceInfo& ctrlInfo)
{
}

void CCtrlDeviceDocument::insertCtrlDeviceInfo(int hour, int minute, int second, int hundredMS,
                                      const CCtrlDeviceInfo& ctrlInfo)
{
}

void CCtrlDeviceDocument::readControlFile(const QString& filepath)
{
    m_ctrlfilepath = filepath + ".ctrl";
    QFile file(m_ctrlfilepath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        return;
    }

    while (true) {
        QString val = file.readLine();
        if(val.isEmpty())
            break;
        m_root.append(val);
    }

    file.close();

    qDebug() << "read control file of " << filepath << QString(", size=%1").arg(m_root.size());
}

void CCtrlDeviceDocument::writeToFile()
{
}

QString CCtrlDeviceDocument::getCtrlFilePath()
{
    return m_ctrlfilepath;
}
