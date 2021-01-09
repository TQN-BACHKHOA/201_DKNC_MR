#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QtSerialPort>
#include <QTimer>
#include <QString>
#include <QtSerialPort/QSerialPortInfo>
#include <QtDebug>
#include <QByteArray>
#include <QTextStream>
#include "qcustomplot.h"
#include <iostream>
#include <string>

#define RPMmax 250
#define RPMmin -250


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


union ByteToFloat{
    float myfloat;
    char mybyte[4];
} m_data, m_data_RX, PID_Set_Kp, PID_Set_Ki, PID_Set_Kd,POS_Set;

private slots:
    void fillPortAction();
    void on_btn_SetUart_clicked();
    void on_btn_closeUart_clicked();
    void Setup_Garp();
    void RealTimeData();
    void ReadData();
    void on_btn_setSpeed_clicked();
    void on_btn_Clear_clicked();
    void on_btnExit_clicked();
    void Send_Speed();
    void One_Last_Send();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
