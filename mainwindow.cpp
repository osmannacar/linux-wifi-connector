#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QStringList>
#include <QProcess>
#include <iostream>
#include <QPushButton>
#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //    iwlist wlp6s0 s | grep ESSID
    //    nmcli -f SSID,BSSID,DEVICE,RATE,BARS,SECURITY,ACTIVE dev wifi
    //    nmcli c down id NAME` - Will disconnect the connection NAME
    //    nmcli c up id NAME` - Will connect the connection NAME
    //    nmcli c - delete connection
    //    iwgetid -r - current connected ssid
    //    cd /etc/NetworkManager/system-connections
    //    ls
    //    sudo cat NAME | grep psk=
    //    nmcli radio wifi off - on = wireless start stop
    // nmcli device status | grep wifi - wireless status
    this->setMinimumSize(1000, 600);
    this->ui->btn_refresh->setToolTip("Refresh");

    this->ui->btn_refresh->setIcon(QIcon(QPixmap::fromImage(QImage(":/img/refresh.svg").scaled(this->ui->btn_refresh->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation))));
   QObject::connect(this->ui->btn_refresh, SIGNAL(clicked()), this, SLOT(findAvailableConnection()));
    QObject::connect(this->ui->btn_wifi, SIGNAL(clickSwitch(bool)), this, SLOT(turnOnOrOFWifi(bool)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::runProcess(QString dest, QString command)
{
    QString str;
    this->setCursor(Qt::WaitCursor);
    QProcess process;
    if(!dest.isEmpty())
        process.setWorkingDirectory(dest);
    process.start("sh", QStringList() << "-c"  << command);
    process.waitForReadyRead();
    process.waitForFinished();
    str = process.readAllStandardOutput();
    process.kill();
    this->setCursor(Qt::ArrowCursor);
    return  str;
}

void MainWindow::showEvent(QShowEvent *event)
{
    if(event->type() == QShowEvent::Show){
        this->ui->btn_wifi->setSwitch(getWirelessState());
        this->findAvailableConnection();
    }

    this->QWidget::showEvent(event);
}


void MainWindow::turnOnOrOFWifi(bool state)
{
    QString msgtext = QString("Wi-fi is turned ");
    QString str;
    QString command = "nmcli radio wifi ";;
    if(state){
        msgtext.append("on");
        command.append("on");
        this->ui->btn_wifi->setToolTip("Turn off");
    }else {
        msgtext.append("off");
        command.append("off");
        this->ui->btn_wifi->setToolTip("Turn on");
    }
    str = this->runProcess("", command);
    if(str.isEmpty()){
        QMessageBox::information(this,"Info", msgtext, QMessageBox::Ok);
        this->setCursor(Qt::WaitCursor);
        if(state)
            QThread::sleep(5);
        this->findAvailableConnection();
        this->setCurrentConnectedWifiSSID(state);
        this->setCursor(Qt::ArrowCursor);
    }else {
        QMessageBox::critical(this,"Error", msgtext, QMessageBox::Ok);
    }
}

// find current connected ssid
QString MainWindow::findCurrentConnectedNetworkInterface()
{
    QString command, ssid;
    command = "iwgetid -r";
    ssid = this->runProcess("", command);
    ssid.remove("\n");
    return ssid;
}

void MainWindow::setCurrentConnectedWifiSSID(bool state)
{
    if(state){
        this->ui->lbl_connected_wifi->setText("Current Wifi: " + this->findCurrentConnectedNetworkInterface());
    }else {
        this->ui->lbl_connected_wifi->setText("");
    }
}

void MainWindow::findAvailableConnection()
{
    this->ui->tableWidget->clear();
    this->ui->tableWidget->setColumnCount(8);
    this->ui->tableWidget->setHorizontalHeaderLabels( QStringList() << "SSID" << "BSSID" << "DEVICE" << "RATE" << "BARS" << "SECURITY" << "ACTIVE" <<" ");
    if(getWirelessState()){
        //find all available wifi and just list of SSID,BSSID,DEVICE,RATE,BARS,SECURITY,ACTIVE information
        QString command = "nmcli -f SSID,BSSID,DEVICE,RATE,BARS,SECURITY,ACTIVE dev wifi";

        QString str = this->runProcess("", command);
        QStringList strList = str.split("\n");
        QString header = strList.at(0);
        strList.removeFirst(); // remove title (SSID,BSSID,DEVICE,RATE,BARS,SECURITY,ACTIVE)
        strList.removeAll(""); // remove all "" values
        QStringList SSID, BSSID, DEVICE, RATE, BARS, SECURITY, ACTIVE;


        this->ui->tableWidget->setRowCount(strList.size());
        for (int i = 0; i < strList.size(); ++i) {
            QTableWidgetItem * proItem;
            SSID.append(strList.at(i).mid(0, header.indexOf("BSSID")).trimmed());
            proItem = new QTableWidgetItem();
            proItem->setText(SSID.at(i));
            proItem->setFlags(proItem->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEditable));
            this->ui->tableWidget->setItem(i, 0, proItem);

            BSSID.append(strList.at(i).mid(header.indexOf("BSSID"), header.indexOf("DEVICE") - header.indexOf("BSSID")).trimmed());
            proItem = new QTableWidgetItem();
            proItem->setText(BSSID.at(i));
            proItem->setFlags(proItem->flags()  & ~(Qt::ItemIsSelectable | Qt::ItemIsEditable));
            this->ui->tableWidget->setItem(i, 1, proItem);

            DEVICE.append(strList.at(i).mid(header.indexOf("DEVICE"), header.indexOf("RATE") - header.indexOf("DEVICE")).trimmed());
            proItem = new QTableWidgetItem();
            proItem->setText(DEVICE.at(i));
            proItem->setFlags(proItem->flags()  & ~(Qt::ItemIsSelectable | Qt::ItemIsEditable));
            this->ui->tableWidget->setItem(i, 2, proItem);

            RATE.append(strList.at(i).mid(header.indexOf("RATE"), header.indexOf("BARS") - header.indexOf("RATE")).trimmed());
            proItem = new QTableWidgetItem();
            proItem->setText(RATE.at(i));
            proItem->setFlags(proItem->flags()  & ~(Qt::ItemIsSelectable | Qt::ItemIsEditable));
            this->ui->tableWidget->setItem(i, 3, proItem);

            BARS.append(strList.at(i).mid(header.indexOf("BARS"), header.indexOf("SECURITY") - header.indexOf("BARS")).trimmed());
            proItem = new QTableWidgetItem();
            proItem->setText(BARS.at(i));
            proItem->setFlags(proItem->flags()  & ~(Qt::ItemIsSelectable | Qt::ItemIsEditable));
            this->ui->tableWidget->setItem(i, 4, proItem);

            SECURITY.append(strList.at(i).mid(header.indexOf("SECURITY"), header.indexOf("ACTIVE") - header.indexOf("SECURITY")).trimmed());
            proItem = new QTableWidgetItem();
            proItem->setText(SECURITY.at(i));
            proItem->setFlags(proItem->flags()  & ~(Qt::ItemIsSelectable | Qt::ItemIsEditable));
            this->ui->tableWidget->setItem(i, 5, proItem);

            ACTIVE.append(strList.at(i).mid(header.indexOf("ACTIVE"), header.length() - header.indexOf("ACTIVE")).trimmed());
            proItem = new QTableWidgetItem();
            proItem->setText(ACTIVE.at(i));
            proItem->setFlags(proItem->flags()  & ~(Qt::ItemIsSelectable | Qt::ItemIsEditable));
            this->ui->tableWidget->setItem(i, 6, proItem);
            QWidget *widget = new QWidget();
            QHBoxLayout *vBox = new QHBoxLayout();
            QPushButton *settings = new QPushButton();
            settings->setFixedSize(30, 30);
            widget->setFixedSize(45, 45);
            settings->setIcon(QIcon(QPixmap::fromImage(QImage(":/img/wifi.png").scaled(settings->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation))));
            settings->setToolTip("Connect Wifi");
            settings->setObjectName(QString::number(i));
            vBox->addWidget(settings, Qt::AlignHCenter);
            widget->setLayout(vBox);
            this->ui->tableWidget->setCellWidget(i, 7, widget);
            this->ui->tableWidget->setRowHeight(i, 45);
            QObject::connect(settings, SIGNAL(clicked()), this, SLOT(connectWifi()));
        }
    }
    this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->ui->tableWidget->setColumnWidth(2, 20);
    this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Custom);
    this->ui->tableWidget->resizeColumnToContents(2);
    this->ui->tableWidget->setColumnWidth(4, 20);
    this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Custom);
    this->ui->tableWidget->resizeColumnToContents(4);
    this->ui->tableWidget->setColumnWidth(6, 20);
    this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Custom);
    this->ui->tableWidget->resizeColumnToContents(6);
    this->ui->tableWidget->setColumnWidth(7, 20);
    this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Custom);
    this->ui->tableWidget->resizeColumnToContents(7);
}

bool MainWindow::getWirelessState()
{
    bool flag = false;
    QString command, str;
    //if wireless connected a wifi, this a message and it contains 'connected'
    command = "nmcli device status | grep wifi";
    str = this->runProcess("", command);
    if(str.contains("connected")){
        flag = true;
        this->ui->btn_wifi->setToolTip("Turn off");
    }else {
        this->ui->btn_wifi->setToolTip("Turn on");
    }
    this->setCurrentConnectedWifiSSID(flag);
    return flag;
}

void MainWindow::connectWifi()
{
    QObject* obj = sender();
    int selected = obj->objectName().toInt();

    QString ssid  =  this->ui->tableWidget->item(selected, 0)->text();

    // must be written your computers user password instead of 2052
    QString command = QString("echo 2052 | sudo -S cat ") + ssid + QString(" | grep psk=");
    QString password = this->runProcess("/etc/NetworkManager/system-connections", command);
    if(!password.isEmpty()){
        password.remove("\n");
        password.remove("psk=");
    }
    bool isOk;
    QString text = QInputDialog::getText(this,"Password","Please enter " + ssid +" password", QLineEdit::Normal, QString(password), &isOk);
    if(isOk){
        command = "nmcli c down id " + this->findCurrentConnectedNetworkInterface();
        QString str = this->runProcess("", command);

        command = "nmcli dev wifi connect " + ssid + " password " + text;
        str = this->runProcess("", command);
        if(str.contains("successfully")){
            QMessageBox::information(this,"Info", "Succesfully connected", QMessageBox::Ok);
        }else {
            QMessageBox::critical(this,"Error", "Invalid Password", QMessageBox::Ok);
        }
        this->findAvailableConnection();
    }

}
