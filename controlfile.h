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


//  ����һ��ʱ�������Ӧ����Ϣ�����ǲ�����ʱ���
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

    //  �����Ǳ�׼�ӿ�
    virtual int getCtrlCnt() const;
    virtual double getTimeTickOfCtrlDeviceInfo(int index) const;
    virtual CCtrlDeviceInfo getCtrlDeviceInfo(int index) const;

    //  update existing one without modify its timestamp
    virtual void updateCtrlDeviceInfo(int index, const CCtrlDeviceInfo& ctrlInfo);

    //  insert a new one with its timestamp
    //  �����ʱ���������
    virtual void insertCtrlDeviceInfo(int hour, int minute, int second, int hundredMS,
                                      const CCtrlDeviceInfo& ctrlInfo);

    //  ���ļ�
    virtual void readControlFile(const QString& filepath);

    //  д�ļ�
    virtual void writeToFile();

    virtual QString getCtrlFilePath();
private:
    QStringList m_root;
    QString m_ctrlfilepath;
};


#endif // CONTROLFILE_H
