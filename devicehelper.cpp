#include "devicehelper.h"
#include "ui_devicehelper.h"

DeviceHelper::DeviceHelper(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DeviceHelper)
{
    ui->setupUi(this);
    _translator = new QTranslator(this);
    languageInit();
    uiInit();
    checkAppUpdate();
}

void DeviceHelper::checkAppUpdate()
{
    qint64 leftAppVersionol, rightAppVersionol, leftAppVersion, rightAppVersion;
    QString url = QString("http://www.skydroid.xin/download/app/app-Remote-control-mcu-update.json");
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *downreply = manager->get(QNetworkRequest(QUrl(url)));
    QByteArray responseData;
    QEventLoop eventLoop;
    connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();       //block until finish
    responseData = downreply->readAll();
    // reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (!doc.isArray()) {
        qWarning() << "JSON document is not an array.";
        return;
    }
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue &value : jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            if (obj.contains("name") && obj["name"].toString() == "DeviceHelper_WIN")
            {
                appVersionol = QString::number(obj["versionCode"].toDouble(), 'f', 1);
                appDownloadUrl = obj["downloadUrl"].toString();
            }
        }
    }

    leftAppVersionol = QString(appVersionol.at(0)).toInt();     //获取在线版本号 2.3 左侧的2
    rightAppVersionol = QString(appVersionol.at(2)).toInt();    //获取在线版本号 2.3 右侧的3

    leftAppVersion = QString(appVersion.at(0)).toInt();     //获取本地版本号 2.3 左侧的2
    rightAppVersion = QString(appVersion.at(2)).toInt();    //获取本地版本号 2.3 右侧的3

    if (rightAppVersionol == 0)
    {
        appLastVersion = QString("%1.9").arg(leftAppVersionol - 1);
    }else
    {
        appLastVersion = QString::number(leftAppVersionol) + "." + QString::number(rightAppVersionol - 1);
    }

    // qDebug() << QApplication::applicationVersion() <<"  appVersionol = "<< appVersionol << "  appVersion = "<< appVersion;
    qDebug() << "leftAppVersionol = " << leftAppVersionol << " rightAppVersionol " << rightAppVersionol <<"  leftAppVersion = "<< leftAppVersion << "  rightAppVersion = "<< rightAppVersion;

    if((leftAppVersion > leftAppVersionol) || (leftAppVersion >= leftAppVersionol && rightAppVersion >= rightAppVersionol))
    {
        appupdateol = 0;
        appDownloadUrl.clear();
    }else
    {
        appupdateol = 1;
        updateDialog(appVersionol);
    }

    // 删除上个版本的.exe文件
    QString appLastFileName = "DeviceHelper" + appLastVersion + ".exe";
    QString nowFolder = QCoreApplication::applicationDirPath();
    QString lastFilePath = nowFolder + "/" + appLastFileName;
    QFile::remove(lastFilePath);

}

void DeviceHelper::updateDialog(QString appVersionol)
{
    QDialog* dialog = new QDialog(this);
    // 获取主窗口所在的屏幕
    QScreen *screen = QGuiApplication::screenAt(this->geometry().center());
    if (screen) {
        QRect screenRect = screen->availableGeometry();
        dialog->move(screenRect.center() - dialog->rect().center());
    }
    dialog->setWindowTitle(tr("更新"));
    // 设置对话框尺寸
    dialog->resize(140, 140);

    // 去除问号图标
    dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // 创建一个 QLabel
    appupdatelabel = new QLabel(tr("检测更新"), dialog);
    appupdatelabel->setAlignment(Qt::AlignCenter);
    appupdatelabel->clear();
    QString htmlText = tr("DeviceHelper有最新版本") + "<br><u>V"+ appVersionol + "</u>";
    appupdatelabel->setText(htmlText);

    // 创建“确定”和“取消”按钮
    okButton = new QPushButton(tr("更新"), dialog);
    cancelButton = new QPushButton(tr("取消"), dialog);

    exeprogressBar = new QProgressBar(dialog);
    exeprogressBar->setRange(0, 100);
    exeprogressBar->setValue(0);
    exeprogressBar->setTextVisible(true);
    exeprogressBar->setAlignment(Qt::AlignCenter);
    exeprogressBar->hide();

    // 创建一个水平布局管理器，并添加按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();  // 添加伸缩项，使按钮靠右
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    // 创建一个垂直布局管理器，并添加 QLabel 和按钮布局
    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);
    mainLayout->addWidget(appupdatelabel);
    mainLayout->addWidget(exeprogressBar);
    mainLayout->addLayout(buttonLayout);

    // 设置对话框的布局
    dialog->setLayout(mainLayout);

    // 连接按钮的点击事件
    connect(okButton, &QPushButton::clicked, this, &DeviceHelper::on_okButton_clicked);
    connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::reject);
    // 通过 show 方法就能显示出对话框
    dialog->show();
}

//点击更新按钮
void DeviceHelper::on_okButton_clicked()
{
    exeprogressBar->show();
    okButton->setEnabled(false);
    cancelButton->setEnabled(false);
    appupdatelabel->setText(tr("正在更新"));
    QNetworkRequest request;
    appFileName = "DeviceHelper" + appVersionol + ".exe";
    request.setUrl(QUrl(appDownloadUrl));
    myAPPfile = new QFile(this);
    reply = downmanager->get(request);              //发送请求

    connect(reply,&QNetworkReply::readyRead,this,&DeviceHelper::doAppReadyRead);                //开始
    connect(reply,&QNetworkReply::finished,this,&DeviceHelper::doAppFinished);                  //结束
    connect(reply,&QNetworkReply::downloadProgress,this,&DeviceHelper::doAppDownloadProgress);  //url切割解析

    QString userpath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    appFileNameol = userpath.replace("\\", "/") + "/" + appFileName;
    myAPPfile->setFileName(appFileNameol);
    bool ret = myAPPfile->open(QIODevice::WriteOnly|QIODevice::Truncate);    //创建文件
    if(!ret)
    {
        QMessageBox::warning(this,"warning",tr("打开失败"));
        return;
    }
}

void DeviceHelper::updateAppcutable(QString appupdatepath, QString filename)
{
    // QString originalExePath = QCoreApplication::applicationFilePath();

    // 1. 创建临时.exe文件
    // QFile::copy(originalExePath, appupdatepath);

    // 2. 等待当前.exe文件的进程关闭
    // int currentPid = QCoreApplication::applicationPid();
    // QProcess::execute(QString("taskkill /F /PID %1").arg(currentPid));

    // 3. 删除原有.exe文件
    // QFile::remove(originalExePath);
    QString nowFolder = QCoreApplication::applicationDirPath();
    QString newfilepath = nowFolder + "/" + filename;
    QStringList arguments;

    QFile::copy(appupdatepath, newfilepath);

    // 4. 将临时.exe文件重命名为原有.exe文件名
    // QFile::rename(appupdatepath, originalExePath);

    QProcess::startDetached(newfilepath, arguments);
    QCoreApplication::exit(0);
}


void DeviceHelper::languageInit()
{
    qDebug() <<ql.language();
    QFont tmp_font("Arial");
    switch (ql.language()) {
    case QLocale::Chinese:
        _translator->load(":/lan/language/Translate_CN.qm");
        break;
    case QLocale::English:
        _translator->load(":/lan/language/Translate_EN.qm");
        break;
    case QLocale::Russian:
        _translator->load(":/lan/language/Translate_RU.qm");
        qApp->setFont(tmp_font);
        break;
    default:
        _translator->load(":/lan/language/Translate_EN.qm");
        break;
    }
    qApp->installTranslator(_translator);
    ui->retranslateUi(this);
}

void DeviceHelper::uiInit()
{
    // QIcon program_ico(":/ui/res/ProgramIcon.png");
    QIcon fold_ico(":/ui/res/Fold.png");
    appVersion = QApplication::applicationVersion();
    ui->le->clear();    // 清空提示信息
    ui->le_2->clear();
    on_btnrefresh_clicked();    // 刷新串口
    // ui->label->hide();
    // ui->cmbportname->hide();
    ui->widget_2->hide();
    ui->groupBox->hide();
    ui->ptereceive->setReadOnly(true);
    ui->charReadRadioButton->setChecked(true);
    _scanPortTimer = new QTimer(this);
    connect(_scanPortTimer, SIGNAL(timeout()), this, SLOT(messlot()));
    _scanPortTimer->start(500);
    ui->btnopen->setEnabled(false);
    ui->cmbportname->setEnabled(true);
    // ui->cmbportname->setFixedSize(70, 20);
    ui->btnrefresh_3->setEnabled(false);
    ui->btnrefresh_5->setEnabled(false);
    ui->btnrefresh_6->setEnabled(false);
    ui->btnrefresh_5->setFixedSize(40, 40);
    ui->btnrefresh_5->setIcon(fold_ico);
    ui->btn_appversion->setText(tr("软件版本:V%1").arg(appVersion));
    ui->lineEdit_hexFilePath->setEnabled(false);
    ui->ptereceive_2->clear();
    ui->ptereceive_2->setReadOnly(true);
    downmanager = new QNetworkAccessManager(this);
    // myBINfile = new QFile(this);
    // myAPPfile = new QFile(this);
}

DeviceHelper::~DeviceHelper()
{
    delete ui;
}

void DeviceHelper::on_btnopen_clicked()
{
    if (ui->btnopen->text() == tr("打开串口"))
    {
        ui->cmbportname->setEnabled(true);
        ui->progressBar->setValue(0);
        ui->btnrefresh_3->setEnabled(false);
        ui->btnrefresh_5->setEnabled(false);
        ui->btnrefresh_6->setEnabled(true);
        if(!ui->cmbportname->currentText().isEmpty())
        {
            m_serial.setPortName(ui->cmbportname->currentText());
            m_serial.setBaudRate(ui->cmbportbaud->currentText().toInt());
        }
        switch (ui->bit->currentIndex())  //  设置数据位数
        {
        case 8:m_serial.setDataBits(QSerialPort::Data8); break;
        default: break;
        }
        switch (ui->check->currentIndex())  //  设置奇偶校验
        {
        case 0: m_serial.setParity(QSerialPort::NoParity); break;
        case 1: m_serial.setParity(QSerialPort::OddParity); break;
        case 2: m_serial.setParity(QSerialPort::EvenParity); break;
        default: break;
        }
        switch (ui->stopbit->currentIndex())  //  设置停止位
        {
        case 1: m_serial.setStopBits(QSerialPort::OneStop); break;
        case 2: m_serial.setStopBits(QSerialPort::TwoStop); break;
        default: break;
        }
        m_serial.open(QIODevice::ReadWrite);
        if(m_serial.isOpen())
        {
            ui->le->setText(tr("串口打开成功"));
            connect(&m_serial,SIGNAL(readyRead()),this,SLOT(messlot()));
            ui->btnopen->setText(tr("关闭串口"));
            // connect(&m_serial,&QSerialPort::readyRead,this,&Mserialport::messlot);
        }else{
            ui->le->setText(tr("串口打开失败"));
            // newportName.clear();
            // ui->cmbportname->clear();
            ui->btnopen->setEnabled(false);
            ui->btnrefresh_3->setEnabled(false);
            ui->btnrefresh_5->setEnabled(false);
            ui->btnrefresh_6->setEnabled(false);
            ui->cmbportname->setEnabled(true);
        }
    }else  // 关闭串口
    {
        ui->cmbportname->setEnabled(true);
        ui->btnrefresh_3->setEnabled(false);
        ui->btnrefresh_5->setEnabled(false);
        ui->btnrefresh_6->setEnabled(false);
        ui->lineEdit_hexFilePath->clear();
        ui->le_2->clear();
        m_serial.close();
        if(m_serial.isOpen())
        {
            ui->le->setText(tr("串口关闭失败！！！"));
        }else{
            m_serial.close();
            ui->btnopen->setText(tr("打开串口"));
            ui->le->setText(tr("串口关闭成功"));
        }
    }
}

// 串口接收信息的处理函数
void DeviceHelper::messlot()
{
    QByteArray arrayrecv;
    arrayrecv= m_serial.readAll();
    ui->ptereceive->insertPlainText(arrayrecv);
    lastLine = arrayrecv;
    if (!lastLine.isNull()) {
        if(lastLine.contains("AT+OK", Qt::CaseSensitive))  //判断是否带有AT+OK字符串
        {
            if(lastLine.contains("-b", Qt::CaseSensitive) and lastLine.contains("-h", Qt::CaseSensitive))  //判断是否带有-b和-h字符串
            {
                checkVersion();
            }else if(lastLine.contains("-n", Qt::CaseSensitive))  //如果不带有-b 和 -h 且带有-n字符串，则说明开始升级
            {
                upgrade();
            }
            else  //如果都没有，就再次发送版本检查
            {
                m_serial.write("AT+VER -?\r\n");
            }
        }else if(lastLine.contains("AT+KEY", Qt::CaseSensitive))  //当发送AT+VER -?，会收到串口回复AT+KEY，此时，再次发送AT+VER -k123456，123456为升级秘钥
        {
            m_serial.write("AT+VER -k123456\r\n");
        }else if(lastLine.contains("AT+ERR", Qt::CaseSensitive))  //当回复的是AT+ERR时，则判定为升级失败
        {
            ui->le_2->setText(tr("传输出错，正在重新传输"));
            try {
                QString frameStr = lastLine.mid(9, 3);  //从索引9开始截取3个字符
                frame = frameStr.toInt();  //将截取的字符串转换为整数
            } catch (...) {
                qDebug() << "截取数据包帧数目异常.";
                frame--; // 出错时将 frame 减1
            }
            upgrade();
        }
    }
}

void DeviceHelper::initFirmwareInfo(bool isStart)
{
    QFile file;
    if(isStart)
    {
        try {
            if(updateol)
            {
                file.setFileName(fileNameol);
            }else
            {
                file.setFileName(fileName);
            }

            if (file.open(QIODevice::ReadOnly)) {
                fileLength = file.size();
                total = qCeil(fileLength / static_cast<qreal>(FRAME_SIZE));
                totalArray = file.readAll();
            } else {
                qDebug() << "打开文件失败！";
            }
        } catch (...) {
            qDebug() << "固件初始化发生错误！";
        }
    }else
    {
        total = 0;
        frame = 0;
        fileLength = 0;
        totalArray.clear();
        currentArray = QByteArray(FRAME_CRC_SIZE, 0);
    }
    // qDebug() << "999 initFirmwareInfo total = " << total << " " << fileLength << " " << totalArray.size();
}

void DeviceHelper::upgrade()
{
    currentArray = QByteArray(FRAME_CRC_SIZE, '\0');
    if (frame > total - 1) {  //相关参数重置
        ui->progressBar->setValue(100);
        initFirmwareInfo(false);
        ui->btnrefresh_3->setEnabled(false);
        ui->btnrefresh_5->setEnabled(false);
        ui->btnrefresh_6->setEnabled(false);
        ui->cmbportname->setEnabled(true);
        ui->le_2->setText(tr("升级成功！"));
        ui->btnopen->setText(tr("打开串口"));
        ui->btnopen->setEnabled(false);
        fileName.clear();
        fileNameol.clear();
        ui->le->clear();
        ui->ptereceive_2->clear();
        ui->lineEdit_hexFilePath->clear();
        m_serial.close();
        updateol = 0;
        return;
    }
    int per = (frame * 100) / total;
    ui->progressBar->setValue(per);
    ui->progressBar->setEnabled(true);

    QByteArray temp(FRAME_ALL_SIZE, 0);  // 287  一帧完整数据
    QString cmd = QString("AT+UPDATA -f%1 -n%2 -d").arg(fileLength, 5, 10, QLatin1Char('0')).arg(frame, 3, 10, QLatin1Char('0')).toUtf8();
    temp.replace(0, cmd.toUtf8().size(), cmd.toUtf8());
    int sum = 0;
    if (!totalArray.isNull()) {
        int currentIndex = frame * FRAME_SIZE;
        for (int i = 0; i <= FRAME_MAX_INDEX; ++i) {
            if (i + currentIndex > totalArray.size() - 1) {
                currentArray[i] = 0xFF;
                sum += 0xFF;
            } else {
                currentArray[i] = totalArray[i + currentIndex];
                sum += (currentArray[i] & 0xff);
            }
        }
    }
    currentArray[FRAME_SIZE] = ((sum >> 8) & 0x0FF);
    currentArray[FRAME_SIZE + 1] = (sum & 0xFF);
    currentArray[FRAME_SIZE + 2] = 0x0D; // "\r"
    currentArray[FRAME_SIZE + 3] = 0x0A; // "\n"
    // qDebug() << "999 initFirmwareInfo currentArray = " << currentArray;
    memcpy(temp.data() + cmd.toUtf8().size(), currentArray.data(), FRAME_CRC_SIZE);
    m_serial.write(temp + "\r\n");
    frame++;
}

void DeviceHelper::on_charReadRadioButton_clicked()
{
    if(ui->charReadRadioButton->isChecked())
        at = 1;
    else
        at = 0;
}

void DeviceHelper::on_btnclearre_clicked()
{
    ui->ptereceive->clear();
}

void DeviceHelper::on_btnrefresh_clicked()
{
    m_serial.close();
    ui->btnopen->setText(tr("打开串口"));
    ui->btnrefresh_3->setEnabled(false);
    ui->btnrefresh_5->setEnabled(false);
    ui->btnrefresh_6->setEnabled(false);
    ui->lineEdit_hexFilePath->clear();
    ui->cmbportname->clear();
    ui->le_2->clear();
    int nn = ui->cmbportname->count();
    for (int i =nn-1;i>-1; i--)
    {
        ui->cmbportname->removeItem(i);
    }
    // 清空串口名
    ui->btnopen->setEnabled(true);

    // 创建一个 QStringList 数组来接收所有存在的串口名称
    QStringList currentPortNames;

    // 遍历串口信息
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->cmbportname->addItem(info.portName());
        currentPortNames << info.portName();
        qDebug() << "All port :" << currentPortNames;
    }

    if (currentPortNames.size() == 1) {
        newportName = currentPortNames.at(0);
    }else
    {
        // 对比当前的串口名称列表和上一次的串口名称列表，找出新增的串口
        foreach (const QString &portName, currentPortNames)
        {
            if (!lastPortNames.contains(portName))
            {
                newportName = portName;
                qDebug() << "New port added:" << portName;
            }
        }
    }

    // 设置组合框的当前选中项为新增的串口
    if (!newportName.isEmpty()) {
        int index = ui->cmbportname->findText(newportName);
        if (index != -1) {
            ui->cmbportname->setCurrentIndex(index);
        }
    }

    // 更新上一次的串口名称列表
    lastPortNames = currentPortNames;

    ui->le->setText(tr("已经刷新串口"));
}

void DeviceHelper::on_btnrefresh_3_clicked()
{
    if(m_serial.isOpen())
    {
        if(fileName.isNull())  //没有本地固件，进入在线升级
        {
            ui->le_2->setText(tr("进行在线升级"));
            updateOnline();
        }else
        {
            ui->le_2->setText(tr("进行本地升级"));
            // qDebug() << "YHZ000";
            updateLocal();
        }
    }else if(!m_serial.isOpen())
    {
        ui->le_2->setText(tr("串口未打开，升级失败"));
    }else if(lastLine.isNull())
    {
        ui->le->setText(tr("串口没有返回数据，请检查串口！"));
    }
}

void DeviceHelper::on_btnrefresh_5_clicked()
{
    if(m_serial.isOpen())
    {
        QSettings setting("./Setting.ini", QSettings::IniFormat);          //为了能记住上次打开的路径
        QString lastPath = setting.value("LastFilePath").toString();

        //    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Open File"), lastPath, tr("Model File(*.hex *.bin)"));
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), lastPath, tr("Model File(*.bin)"));

        QFile file(fileName);
        // QString tital = fileName;
        QString mydevice = fileName.section('/', -1).section('-', 0, 0);
        qDebug()<<"fileName = "<<fileName << "mydevice = "<< mydevice;
        if(mydevice != devicename)
        {
            ui->le_2->setText(tr("请选择 %1 固件").arg(devicename));
            ui->btnrefresh_3->setEnabled(false);
        }else if(m_serial.isOpen())
        {
            ui->btnrefresh_3->setEnabled(true);
            ui->le_2->setText(tr("已选择本地固件，可以升级"));
        }
        // DeviceHelper::setWindowTitle(tital);

        bool ok = file.open(QFile::ReadOnly);

        if(!ok)
        {
            QMessageBox msgBox;
            msgBox.setText(tr("打开文件失败！"));
        }
        ui->lineEdit_hexFilePath->setText(fileName);
        file.close();
    }else
    {
        ui->le_2->setText(tr("串口未打开，升级失败"));
    }
}

void DeviceHelper::updateOnline()  //在线升级，必须先检查版本号，只有比板子的版本号高才能升级。本地升级没有这个检测。
{
    m_serial.write("AT+VER -?\r\n");
    ui->btnrefresh_3->setEnabled(false);
    QString url = QString("http://www.skydroid.xin/download/app/app-Remote-control-mcu-update.json");
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *downreply = manager->get(QNetworkRequest(QUrl(url)));
    QByteArray responseData;
    QNetworkRequest request;
    QEventLoop eventLoop;
    QString filename;
    QString brief;
    connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();       //block until finish
    responseData = downreply->readAll();
    // reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (!doc.isArray()) {
        qWarning() << "JSON document is not an array.";
        return;
    }
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue &value : jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            if (obj.contains("name") && obj["name"].toString() == devicename)
            {
                if (obj.contains("downloadUrl"))
                {
                    downloadUrl = obj["downloadUrl"].toString();
                    filename = obj["versionName"].toString();
                    versionol = QString::number(obj["versionCode"].toDouble(), 'f', 1);
                    brief = obj["brief"].toString();
                    qDebug() << "Download URL for name :" << devicename << "downloadUrl: " <<downloadUrl;
                } else {
                    qWarning() << "No downloadUrl found for name :" << devicename;
                }
            }
        }
    }

    leftCharol = QString(versionol.at(0)).toInt();
    rightCharol = QString(versionol.at(2)).toInt();

    request.setUrl(QUrl(downloadUrl));
    myBINfile = new QFile(this);
    reply = downmanager->get(request);              //发送请求

    connect(reply,&QNetworkReply::readyRead,this,&DeviceHelper::doProcessReadyRead);                //开始
    connect(reply,&QNetworkReply::finished,this,&DeviceHelper::doProcessFinished);                  //结束
    connect(reply,&QNetworkReply::downloadProgress,this,&DeviceHelper::doProcessDownloadProgress);  //url切割解析

    QString userpath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    fileNameol = userpath.replace("\\", "/") + "/" + filename;
    myBINfile->setFileName(fileNameol);
    bool ret = myBINfile->open(QIODevice::WriteOnly|QIODevice::Truncate);    //创建文件
    if(!ret)
    {
        QMessageBox::warning(this,"warning",tr("打开失败"));
        return;
    }
    MySleep(2000);  //需要加入延迟，等待下载完毕，否则只下载0kb文件就烧录进去了。
    m_serial.write("AT+VER -b1\r\n");
    checkversion = 1;
    updateol = 1;
}

void DeviceHelper::updateLocal()  //本地升级
{
    if(m_serial.isOpen())
    {
        ui->btnrefresh_3->setEnabled(false);
        m_serial.write("AT+VER -b1\r\n");
        checkversion = 1;
    }else
    {
        ui->le_2->setText(tr("串口未打开，升级失败"));
    }
}

void DeviceHelper::checkVersion()  //BOOT状态检查
{
    if(!checkversion)
    {
        //字符串裁切，从串口返回的字符串中裁取BOOT模式等信息。
        bool isupdate;
        int indexOfH,indexOfV,hardstart,hardlength;
        QString strboot, strhard, strsoft;
        QString bootnum;
        QString trimmedStr = lastLine.trimmed();
        strboot = trimmedStr.section('-', 1, 1);
        strhard = trimmedStr.section('-', 2, 2);
        strsoft = trimmedStr.section('-', 3, 3);
        bootnum = strboot.trimmed().mid(1);
        //获取设备名
        indexOfH = trimmedStr.indexOf("-h");
        indexOfV = trimmedStr.indexOf("-v");
        hardstart = indexOfH + 2; // 跳过“-h”
        hardlength = indexOfV - hardstart - 1; // 计算需要截取的长度
        devicename = trimmedStr.mid(hardstart, hardlength);

        versionnow = strsoft.trimmed().mid(1).insert(1, '.');
        leftChar = QString(versionnow.at(0)).toInt();   //获取版本号 2.3 左侧的2
        rightChar = QString(versionnow.at(2)).toInt();  //获取版本号 2.3 右侧的3

        if(checkversionol)
        {
            QString brief = checkVersionOl();
            if(leftChar == leftCharol and rightChar == rightCharol)
            {
                ui->le_2->setText(tr("与线上版本一致，无需升级"));
                ui->btnrefresh_3->setEnabled(false);
                isupdate = 0;

            }else if((leftChar > leftCharol)||(leftChar >= leftCharol && rightChar >= rightCharol))
            {
                if(versionol.isEmpty())
                {
                    ui->le_2->setText(tr("暂无线上版本，无需升级"));
                }else
                {
                    ui->le_2->setText(tr("本地版本高于线上版本，无需升级"));
                }
                isupdate = 0;
                ui->btnrefresh_3->setEnabled(false);
            }else
            {
                ui->btnrefresh_3->setEnabled(true);
                isupdate = 1;
            }

            QString flag = isupdate ? tr("是") : tr("否");
            QTextCharFormat fmt;
            if(versionol.isEmpty())
            {
                versionol = tr("无");
            }else
            {
                versionol = "V" + versionol;
            }
            if(brief.isEmpty())
            {
                brief = tr("无");
            }

            QString deviceinfo = tr("设备硬件信息：") + "<u>"+ devicename + "</u><br>";
            QString versionnowinfo = tr("当前固件版本：") + "<u>V"+ versionnow + "</u><br>";
            QString flaginfo = tr("是否需要升级：") + "<u>"+ flag + "</u><br>";
            QString versionolinfo = tr("在线固件版本：") + "<u>"+ versionol + "</u><br>";
            QString briefinfo = tr("在线固件信息：") + "<br>" + brief.replace("\n", "<br>") + "<br>";
            fmt.setForeground(QColor(Qt::red));
            ui->ptereceive_2->mergeCurrentCharFormat(fmt);
            checkversionol = 0;
            QString htmlText = deviceinfo +
                               versionnowinfo +
                               flaginfo  +
                               versionolinfo  +
                               briefinfo;
            ui->ptereceive_2->setHtml(htmlText);
            // ui->ptereceive_2->setPlainText(tr("设备硬件信息：") + devicename + " \n" + tr("当前固件版本：") + versionnow + " \n" + tr("是否需要升级：") + flag + " \n" + tr("在线固件版本：") + versionol + " \n" + tr("在线固件信息：") + "\n" + brief);
        }

    }else if(checkversion)
    {
        QString strboot;
        QString bootnum;
        QString checkedStr = lastLine.trimmed();
        strboot = checkedStr.section('-', 1, 1);
        bootnum = strboot.trimmed().mid(1);
        checkversion = 0;
        if(updateol)
        {
            if((leftChar > leftCharol)||(leftChar >= leftCharol && rightChar >= rightCharol))
            {
                if(leftChar == leftCharol and rightChar == rightCharol)
                {
                    updateol = 0;
                    return;
                }
                updateol = 0;
                return;
            }else
            {
                ui->le_2->setText(tr("正在升级"));
            }
        }
        if(bootnum == "1" and (!fileName.isNull() or !fileNameol.isNull()))  //  开始升级
        {
            initFirmwareInfo(true);
            upgrade();
        }else if(fileName.isNull() or fileNameol.isNull())
        {
            if(updateol)
            {
                ui->le_2->setText(tr("未下载成功，请检查网络"));
                ui->btnrefresh_3->setEnabled(true);
                ui->btnrefresh_5->setEnabled(true);
                updateol = 0;
            }else
            {
                ui->le_2->setText(tr("请选择本地固件"));
            }

        }else    //  升级失败
        {
            ui->le_2->setText(tr("未通过版本校验，升级失败"));
            ui->btnrefresh_3->setEnabled(true);
            ui->btnrefresh_5->setEnabled(true);
            updateol = 0;
        }
    }
}

//更新exe文件
void DeviceHelper::doAppReadyRead()             //读取并写入
{
    while(!reply->atEnd())
    {
        QByteArray ba = reply->readAll();
        myAPPfile->write(ba);
    }
}

void DeviceHelper::doAppFinished()
{
    appupdatelabel->setText(tr("更新成功，正在重启"));
    myAPPfile->close();
    MySleep(2000);
    updateAppcutable(appFileNameol, appFileName);
}

void DeviceHelper::doAppDownloadProgress(qint64 recv_total, qint64 all_total)      //显示
{
    QStringList list = downloadUrl.split("/");
    // QString filename = list.at(list.length()-1);

    // 更新进度条
    if (recv_total > 0) {
        int progress = static_cast<int>((recv_total * 100) / all_total);
        exeprogressBar->setValue(progress);
    }

}


//更新bin文件
void DeviceHelper::doProcessReadyRead()             //读取并写入
{
    while(!reply->atEnd())
    {
        QByteArray ba = reply->readAll();
        myBINfile->write(ba);
    }
}

void DeviceHelper::doProcessFinished()
{
    myBINfile->close();
}

void DeviceHelper::doProcessDownloadProgress()      //显示
{
    // QStringList list = downloadUrl.split("/");
    // QString filename = list.at(list.length()-1);

}

void DeviceHelper::MySleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}



void DeviceHelper::on_btnrefresh_6_clicked()
{
    versionol.clear();
    ui->lineEdit_hexFilePath->clear();
    ui->ptereceive_2->clear();
    ui->le_2->clear();
    ui->btnrefresh_5->setEnabled(true);
    ui->btnrefresh_6->setEnabled(true);
    checkversionol = 1;
    m_serial.write("AT+VER -?\r\n");
}

QString DeviceHelper::checkVersionOl()
{
    QString url = QString("http://www.skydroid.xin/download/app/app-Remote-control-mcu-update.json");
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *downreply = manager->get(QNetworkRequest(QUrl(url)));
    QByteArray responseData;
    QEventLoop eventLoop;
    QString brief;
    connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();       //block until finish
    responseData = downreply->readAll();
    // reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (!doc.isArray()) {
        qWarning() << "JSON document is not an array.";
        return 0;
    }
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue &value : jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            if (obj.contains("name") && obj["name"].toString() == devicename)
            {
                versionol = QString::number(obj["versionCode"].toDouble(), 'f', 1);
                if(ql.language() == QLocale::Chinese)
                {
                    brief = obj["brief"].toString();
                }else if(ql.language() == QLocale::English)
                {
                    brief = obj["briefEn"].toString();
                }else if(ql.language() == QLocale::Russian)
                {
                    brief = obj["briefRu"].toString();
                }else
                {
                    brief = obj["briefEn"].toString();
                }
            }
        }
    }

    leftCharol = QString(versionol.at(0)).toInt();
    rightCharol = QString(versionol.at(2)).toInt();

    return brief;
}

void DeviceHelper::changeEvent(QEvent e)
{
    if (e.type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    else
    {
        QWidget::changeEvent(&e);
    }
}


void DeviceHelper::on_btn_appversion_clicked()
{
    checkAppUpdate();
}

