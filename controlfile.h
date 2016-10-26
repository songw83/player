#ifndef CONTROLFILE_H
#define CONTROLFILE_H

#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>


#define DOC_TIMESTAMP "timestamp"
#define DOC_INFO "info"
#define DOC_ALL "all"


extern double getTimeTick(int hour, int minute, int second, int hundredMS);


//  这是一个时间戳所对应的信息，但是不包括时间戳
class CCtrlDeviceInfo
{
public:
    CCtrlDeviceInfo()
    {

    }

    ~CCtrlDeviceInfo()
    {

    }

    void init(const QString& ctrl)
    {
        m_ctrl = ctrl;
    }

    QString get() const
    {
        return m_ctrl;
    }

private:
    QString m_ctrl;
};

class CCtrlDeviceDocument
{
public:
    CCtrlDeviceDocument()
    {

    }

    ~CCtrlDeviceDocument()
    {

    }

    //  以下是标准接口
    virtual int getCtrlCnt() const;
    virtual double getTimeTickOfCtrlDeviceInfo(int index) const;
    virtual CCtrlDeviceInfo getCtrlDeviceInfo(int index) const;

    //  update existing one without modify its timestamp
    virtual void updateCtrlDeviceInfo(int index, const CCtrlDeviceInfo& ctrlInfo);

    //  insert a new one with its timestamp
    //  会根据时间戳来排序
    virtual void insertCtrlDeviceInfo(int hour, int minute, int second, int hundredMS,
                                      const CCtrlDeviceInfo& ctrlInfo);

    //  读文件
    virtual void readControlFile(const QString& filepath);

    //  写文件
    virtual void writeToFile();

    virtual QString getCtrlFilePath();
private:
    QStringList m_root;
    QString m_ctrlfilepath;
};


#endif // CONTROLFILE_H
