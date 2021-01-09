#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>

QSerialPort *SerialPort;
QTimer *Uart_Timer,*timer,*Speed_Timer;
int setpoint, counter, My_Step=100;
float pre_setpoint=0;
float resolution=0;
float Kp,Ki,Kd,T,POS,Out,Pre_Out2=0, Pre_Out1=0, Pre_In=0;
bool flag_PlotTimer;
char CheckSumRX;
float a=0.002379;
float b=0.002261;
float c=-1.856;
float d=0.860642;
struct Speed_Filter{
    float Raw=0;
    float Pre_Raw=0;
    float Cooked=0;
    float Pre_Cooked=0;
    float a=0.06059;
    float b=0.9394;
} My_Filter;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("STR Controller");
    fillPortAction();
    Setup_Garp();
    SerialPort = new QSerialPort(this);
    Uart_Timer = new QTimer(this);
    Speed_Timer = new QTimer(this);
    connect(Uart_Timer,SIGNAL(timeout()),this,SLOT(ReadData()));
    connect(Speed_Timer,SIGNAL(timeout()),this,SLOT(Send_Speed()));
    //connect(ui->Set,SIGNAL(clicked()),this,SLOT(on_btn_setPoint_clicked()));
    connect(ui->btnExit,SIGNAL(clicked()),this,SLOT(close()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Setup_Garp(){
    ui->customPlot->addGraph(); // blue line
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->customPlot->addGraph(); // red line
    ui->customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->yAxis->setRange(0,15);
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    ui->customPlot->xAxis->setLabel("Time");
    ui->customPlot->yAxis->setLabel("RPM");
}

void MainWindow::fillPortAction() {
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos){
        ui->Com_Uart->addItem(info.portName());
    }
    ui->baudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRate->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    ui->baudRate->addItem(tr("Custom"));

    ui->dataBits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBits->setCurrentIndex(3);

    ui->parity->addItem(tr("None"), QSerialPort::NoParity);
    ui->parity->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parity->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parity->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parity->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBits->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    ui->stopBits->addItem(QStringLiteral("1"), QSerialPort::OneStop);


    ui->flowControl->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControl->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControl->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}
void MainWindow::RealTimeData()
{
 static int yMax=0;
 static int yMin=0;
 static double temp_time=0;
 static QTime time(QTime::currentTime());
 if(flag_PlotTimer){
 flag_PlotTimer = false;
 temp_time=time.elapsed()/1000.0;
 yMax=0;
 }
 double key = time.elapsed()/1000.0;
 float thuw=m_data_RX.myfloat;

 ui->customPlot->graph(0)->addData(key, My_Filter.Cooked);
 ui->customPlot->graph(1)->addData(key,Out);
 ui->customPlot->graph(0)->rescaleValueAxis();
 ui->customPlot->xAxis->setRange(temp_time, key, Qt::AlignLeft);
 if(setpoint >= yMax)
     yMax = setpoint;
 if(m_data_RX.myfloat >= yMax)
     yMax = (int)m_data_RX.myfloat;
 if(setpoint <= yMin)
     yMin = setpoint;
 if(m_data_RX.myfloat <= yMin)
     yMin = (int)m_data_RX.myfloat;
 ui->customPlot->yAxis->setRange(-300, 300);//(yMin-200, yMax+200);
 ui->customPlot->replot();
 // calculate frames per second:
 static double lastFpsKey;
 static int frameCount;
 ++frameCount;
 lastFpsKey = key;
 frameCount = 0;
}

void MainWindow::ReadData()
{
 //ui->m_console->moveCursor(QTextCursor::End);
 QByteArray byte_data = SerialPort->readAll();
 if(!byte_data.isEmpty()){
 if(byte_data.startsWith("$SPEED,")&& byte_data[13]=='\n'&& byte_data[12]=='\r')
 {
 //qDebug() <<byte_data;

 for (int i=0;i<4;i++)
 {
     CheckSumRX+=byte_data[7+i];
 }
// qDebug()<<thuw;
 qDebug()<<CheckSumRX<<byte_data[11];
     if(CheckSumRX==byte_data[11]){
         for(int k= 0;k<4;k++){
         m_data_RX.mybyte[3-k] = byte_data[7+k];
         }
         //ui->SpeedError->display(m_data_RX.myfloat-setpoint);
         }
         My_Filter.Cooked = ((m_data_RX.myfloat <= 1)&&(m_data_RX.myfloat >=0))? 0:m_data_RX.myfloat;
//         My_Filter.Cooked = My_Filter.Pre_Raw*My_Filter.a+My_Filter.Pre_Cooked*My_Filter.b;
//         My_Filter.Pre_Raw = My_Filter.Raw;
//         My_Filter.Pre_Cooked = My_Filter.Cooked;
         ui->RealSpeed->display(My_Filter.Cooked);
         CheckSumRX=0;
         RealTimeData();
         }
 } }




void MainWindow::on_btn_closeUart_clicked()
{
 SerialPort->close();
 setpoint = 0;
}








void MainWindow::on_btn_SetUart_clicked(){
    SerialPort->setPortName(ui->Com_Uart->currentText());
    SerialPort->setBaudRate(ui->baudRate->currentText().toInt());
    SerialPort->setDataBits(static_cast<QSerialPort::DataBits>(ui->dataBits->itemData(ui->dataBits->currentIndex()).toInt()));
    SerialPort->setParity(static_cast<QSerialPort::Parity>(ui->parity->itemData(ui->parity->currentIndex()).toInt()));
    SerialPort->setStopBits(static_cast<QSerialPort::StopBits>(ui->stopBits->itemData(ui->stopBits->currentIndex()).toInt()));
    SerialPort->setFlowControl(static_cast<QSerialPort::FlowControl>(ui->flowControl->itemData(ui->flowControl->currentIndex()).toInt()));
    SerialPort->open(QIODevice::ReadWrite);
    connect(SerialPort, &QSerialPort::readyRead, this,&MainWindow::ReadData);
    Uart_Timer->start(25);
    // Interval 0 means to refresh as fast as possible
}

//----------------------------------------------------------------------
void MainWindow::on_btn_setSpeed_clicked()
{
    setpoint = ui->Set_point->toPlainText().toFloat();
    Speed_Timer->start(5);
}

void MainWindow::on_btn_Clear_clicked()
{
    flag_PlotTimer = true;
    //setpoint=0;
    //SerialPort->close();
    //ui->customPlot->xAxis->setTicker(0);
    ui->customPlot->removeGraph(0);
    ui->customPlot->removeGraph(1);
    Setup_Garp();
    //SerialPort->open(QIODevice::ReadWrite);
}


void MainWindow::on_btnExit_clicked()
{
    setpoint = 0;
    char checksum_Tx = 0;
    QByteArray txbuff;
    txbuff="$SPEED,";
    m_data.myfloat = setpoint;
    for(int k=0;k<4;k++)
    {
    txbuff[7+k]=m_data.mybyte[-k+3];
    checksum_Tx += m_data.mybyte[3-k];
    }
    txbuff[11]=checksum_Tx;
    txbuff[12]= '\r';
    txbuff[13]= '\n';
    SerialPort->write(txbuff,14);
    this->close();
}


void MainWindow::Send_Speed(){
    char checksum_Tx = 0;
    QByteArray txbuff;
    txbuff="$SPEED,";
    Out=a*setpoint+b*pre_setpoint-c*Pre_Out1-d*Pre_Out2;
    Pre_Out2=Pre_Out1;
    Pre_Out1=Out;
    pre_setpoint=setpoint;
    m_data.myfloat = Out;
    for(int k=0;k<4;k++)
    {
    txbuff[7+k]=m_data.mybyte[-k+3];
    checksum_Tx += m_data.mybyte[3-k];
    }
    txbuff[11]=checksum_Tx;
    txbuff[12]= '\r';
    txbuff[13]= '\n';
    SerialPort->write(txbuff,14);
    counter++;
   if((Out>=0.99*setpoint)&&(Out<=1.01*setpoint))
    {
        counter=0;
        Speed_Timer->stop();
        //One_Last_Send();
    }
   if(counter >=140)
   {
       counter=0;
       m_data.myfloat = setpoint;
       for(int k=0;k<4;k++)
       {
       txbuff[7+k]=m_data.mybyte[-k+3];
       checksum_Tx += m_data.mybyte[3-k];
       }
       txbuff[11]=checksum_Tx;
       txbuff[12]= '\r';
       txbuff[13]= '\n';
       SerialPort->write(txbuff,14);
       Speed_Timer->stop();
       //One_Last_Send();
   }
}

void MainWindow::One_Last_Send(){
    char checksum_Tx = 0;
    QByteArray txbuff;
    txbuff="S";
    m_data.myfloat = setpoint;
    for(int k=0;k<4;k++)
    {
    txbuff[1+k]=m_data.mybyte[-k+3];
    checksum_Tx += m_data.mybyte[3-k];
    }
    txbuff[5]=checksum_Tx;
    txbuff[6]= '\r';
    txbuff[7]= '\n';
    SerialPort->write(txbuff,8);
}


