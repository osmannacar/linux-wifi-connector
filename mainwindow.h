#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString runProcess(QString dest, QString command);
    bool getWirelessState();
    QString findCurrentConnectedNetworkInterface();
    void setCurrentConnectedWifiSSID(bool state);
protected:
    void showEvent(QShowEvent *event) override;

public slots:
    void findAvailableConnection();
    void turnOnOrOFWifi(bool state);
    void connectWifi();
};
#endif // MAINWINDOW_H
