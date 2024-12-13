#ifndef DEVICEHELPER_H
#define DEVICEHELPER_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <Qtimer>
#include <QTextBlock>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QtMath>
#include <QStandardItemModel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDebug>
#include <QTranslator>
#include <QEvent>
#include <QDialog>
#include <QDialogButtonBox>
#include <QProcess>
#include <QScreen>
#include <QProgressBar>

QT_BEGIN_NAMESPACE
namespace Ui {
class DeviceHelper;
}
QT_END_NAMESPACE

class DeviceHelper : public QWidget
{
    Q_OBJECT

public:
    DeviceHelper(QWidget *parent = nullptr);
    ~DeviceHelper();

private slots:

    void on_btnopen_clicked();

    void on_charReadRadioButton_clicked();

    void on_btnrefresh_clicked();

    void on_btnrefresh_3_clicked();

    void on_btnrefresh_5_clicked();

    void updateOnline();

    void updateLocal();

    void checkVersion();

    QString checkVersionOl();

    void on_btnclearre_clicked();

    void checkAppUpdate();

    void updateDialog(QString appVersionol);

    void on_okButton_clicked();

    void updateAppcutable(QString appupdatepath, QString filename);

    void languageInit();

    void uiInit();

    void messlot();

    void upgrade();

    void doProcessReadyRead();

    void doProcessFinished();

    void doProcessDownloadProgress();

    void doAppReadyRead();

    void doAppFinished();

    void doAppDownloadProgress(qint64,qint64);

    void MySleep(int msec);

    void initFirmwareInfo(bool isStart);

    void on_btnrefresh_6_clicked();

    void changeEvent(QEvent e);

    void on_btn_appversion_clicked();

private:
    Ui::DeviceHelper *ui;
    QTimer *_scanPortTimer;
    QTimer *_readDataTimer;
    QTimer *_autoSendTimer;
    QLabel *appupdatelabel;
    QSerialPort m_serial;
    QString appVersion;
    QString appVersionol;
    QString appLastVersion;
    QString appFileNameol;
    QString appFileName;
    QString fileName;
    QString fileNameol;
    QString versionol;
    QString versionnow;
    QString lastLine;
    QString devicename;
    QString newportName;
    QStringList lastPortNames;
    QFile *myBINfile;
    QFile *myAPPfile;
    QNetworkAccessManager *downmanager;
    QNetworkReply *reply;
    QString downloadUrl;
    QString appDownloadUrl;
    QTranslator *_translator;
    QLocale ql;

    //更新弹窗的两个按钮和进度条
    QProgressBar *exeprogressBar;
    QPushButton *okButton;
    QPushButton *cancelButton;

    bool at = 1;
    bool appupdateol = 0;
    bool updateol = 0;
    bool checkversion = 0;
    bool checkversionol = 0;

    qint64 frame = 0; // 当前帧数
    qint64 total = 0; // 表示文件需要分成的总帧数
    qint64 fileLength = 0; // 文件的长度
    qint64 leftChar = 0;
    qint64 leftCharol = 0;
    qint64 rightChar = 0;
    qint64 rightCharol = 0;
    QByteArray totalArray; // 存储文件的所有数据
    QByteArray currentArray;
    const int FRAME_SIZE = 256;
    const int FRAME_MAX_INDEX = FRAME_SIZE - 1;
    const int FRAME_CRC_SIZE = 256 + 2 + 2;
    const int FRAME_ALL_SIZE = QString("AT+UPDATA -f12345 -n000 -d").toUtf8().size() + FRAME_CRC_SIZE;
};
#endif // DEVICEHELPER_H
