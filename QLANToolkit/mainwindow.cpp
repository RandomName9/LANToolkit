#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <LANAttacker/lanpcap.h>
#include <LANAttacker/synfloodattacker.h>
#include <LANAttacker/arpattacker.h>
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <LANHelper/lanfiletransfer.h>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //set up the flat ui for mainwindow
    ui->setupUi(this);
    this->FlatWindowUI();
    ui->ScanprogressBar->hide();



    aLANPcap=new LANPcap(this);

    ui->InterfaceBox->addItems(aLANPcap->GetInterfaceDescriptionLists());

    //when find the host info,we update it to IpAddrList to show
    QObject::connect(aLANPcap,&LANPcap::OnUpdateHostInfo,this,&MainWindow::AddIpAddrToList);




    QObject::connect(aLANPcap,&LANPcap::OnAttackSpeedChanged,
                     [this](QString AttackSpeedDescription)
    {

        ui->AttackSpeedDLbl->setText(AttackSpeedDescription);
    });


    aSynAtker=new SynFloodAttacker(aLANPcap,aLANPcap);
    aArpAttacker=new ArpAttacker(aLANPcap,aLANPcap);
    ui->IpAddrListWidget->setAutoScroll(false);


    FileTransfer=new LANFileTransfer(this);
  //  FileTransfer->BroadcastFileTransRequire("127.0.0.1","127.0.0.1","broadcast test",123);


    // qDebug()<<FileTransfer->SetFilesToSend("D:/TestSend");



    //FileTransfer->SetSenderHost("127.0.0.1");
    // FileTransfer->ConnectToReceiver("127.0.0.1");



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
        ui->ScanHostsBtn->setEnabled(true);
        ui->IpAddrListWidget->setEnabled(true);
        ui->AttackCurrentStataDLbl->setPlainText("pending,suggest add target to attack ");
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
            Item->setIcon(QIcon(":/T_HostUserIco.png"));
            ui->IpAddrListWidget->addItem(Item);

        }

    }
}



void MainWindow::FlatWindowUI()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *ShadowEffect = new QGraphicsDropShadowEffect(this);

    const float ShadowSize=15;

    ShadowEffect->setBlurRadius(ShadowSize);
    ShadowEffect->setOffset(0);
    ShadowEffect->setColor(QColor(0,0,0));
    this->setContentsMargins(ShadowSize,ShadowSize,ShadowSize,ShadowSize);
    this->setGraphicsEffect(ShadowEffect);


    this->resize( this->size()+QSize(ShadowSize*2,ShadowSize*2));
}


void MainWindow::on_CloseBtn_clicked()
{
    this->close();
}

void MainWindow::on_MinimizeBtn_clicked()
{
    this->showMinimized();
}



void MainWindow::on_AttackTab_clicked()
{
    ui->AttackTab->setEnabled(false);
    ui->HelperTab->setEnabled(true);
}

void MainWindow::on_HelperTab_clicked()
{
    ui->AttackTab->setEnabled(true);
    ui->HelperTab->setEnabled(false);
}

void MainWindow::on_LockAllTargetBtn_clicked()
{
    if(0==ui->IpAddrListWidget->count())return;

    for(int Index=0;Index<ui->IpAddrListWidget->count();++Index)
    {

        QString FullText=ui->IpAddrListWidget->item(Index)->text();
         QString IpAddr=FullText.split("|")[0];
        if(!FullText.contains("|SYN"))
        {
            ui->IpAddrListWidget->item(Index)->setText(FullText+"|SYN");
            aSynAtker->AddTarget(IpAddr);
        }
    }
    UpdateAttackStateDescription();
}





void MainWindow::ToggleCurrentItemDisplayInfo(NetAttacker *Attacker, QString Mark,QPushButton *ToggleBtn)
{
    QListWidgetItem* Item=ui->IpAddrListWidget->currentItem();
    QString FullText=Item->text();
    QString IpAddr=FullText.split("|")[0];
    QString DMark="|"+Mark;
    if(!FullText.contains(DMark))
    {
        Item->setText(FullText+DMark);
        Attacker->AddTarget(IpAddr);
        ToggleBtn->setText(ToggleBtn->text().replace("lock","unlock"));
    }
    else
    {
        Item->setText(FullText.remove(DMark));

        Attacker->RemoveTarget(IpAddr);

        ToggleBtn->setText(ToggleBtn->text().replace("unlock","lock"));

    }

    this->UpdateAttackStateDescription();


}

void MainWindow::UpdateAttackStateDescription()
{
    int NumSynTargets=aSynAtker->GetTargetCounts();
    int NumArpTargets=aArpAttacker->GetTargetCounts();
    QString AttackerInfo=QString("•SynFlood Num➢ %1 VIP\n•ArpSpoof Num➢ %2 VIP").arg(NumSynTargets).arg(NumArpTargets);
    QString Header=(!bIsLockAttack)?"Targets set ▼\n":"Attacking ▼\n";
    ui->AttackCurrentStataDLbl->setPlainText(Header+AttackerInfo);

}

void MainWindow::LockAttack()
{
    bIsLockAttack=true;
    aSynAtker->StartAttackTargets();
    aArpAttacker->StartAttackTargets();
    ui->LockAtkBtn->setText("unlock attack");
    UpdateAttackStateDescription();
}

void MainWindow::UnlockAttacker()
{
    bIsLockAttack=false;
    aSynAtker->StopAttackTargets();
    aArpAttacker->StopAttackTargets();
    ui->LockAtkBtn->setText("lock attack");
    UpdateAttackStateDescription();
}



void MainWindow::on_LockSynfloodBtn_clicked()
{

    ToggleCurrentItemDisplayInfo(aSynAtker,"SYN",ui->LockSynfloodBtn);

}

void MainWindow::on_LockArpsproofBtn_clicked()
{

    ToggleCurrentItemDisplayInfo(aArpAttacker,"ARP",ui->LockArpsproofBtn);
}

void MainWindow::on_ScanHostsBtn_clicked()
{
    aLANPcap->SetCurrentNetInterface(ui->InterfaceBox->currentIndex());
    ui->ScanprogressBar->setValue(0);

    ui->IpAddrListWidget->clear();
    aSynAtker->ClearTargets();

    ui->IpAddrListWidget->setEnabled(false);
    ui->ScanprogressBar->show();
    aLANPcap->StartAnalyzeLAN();

    //disable the scan btn until the scan progress is finished
    ui->ScanHostsBtn->setEnabled(false);

    ui->AttackCurrentStataDLbl->setPlainText("Scanning the while LAN...,please wait");
}

void MainWindow::on_LockAtkBtn_clicked()
{

    if(bIsLockAttack)
    {
      this->UnlockAttacker();
    }
    else
    {
      this->LockAttack();
    }
    UpdateAttackStateDescription();



}

void MainWindow::on_IpAddrListWidget_itemChanged(QListWidgetItem *item)
{




}

void MainWindow::on_IpAddrListWidget_itemActivated(QListWidgetItem *item)
{

}

void MainWindow::on_IpAddrListWidget_itemSelectionChanged()
{
    QListWidgetItem *item=ui->IpAddrListWidget->currentItem();
    QString FullText=item->text();
    ui->LockSynfloodBtn->setText(FullText.contains("|SYN")?"unlock synflood target":"lock synflood target");
    ui->LockArpsproofBtn->setText(FullText.contains("|ARP")?"unlock arpspoof target":"lock arpspoof target");

    QString IpAddr=FullText.split("|")[0];
    ui->SynFloodPortsDLbl->setText(aSynAtker->GetSynFloodInfo(IpAddr));
}
