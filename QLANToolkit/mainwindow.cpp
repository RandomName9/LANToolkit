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
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //set up the flat ui for mainwindow,
    ui->setupUi(this);
    this->FlatWindowUI();

    //set up init ui
    ui->ScanprogressBar->hide();
    on_AttackTab_clicked();


    //construct the attacker objects, and FileTransfer class
    aLANPcap=new LANPcap(this);
    aSynAtker=new SynFloodAttacker(aLANPcap);
    aArpAttacker=new ArpAttacker(aLANPcap);
    FileTransfer=new LANFileTransfer(this);

    //add all available driver interface to interface listbox
    ui->InterfaceBox->addItems(aLANPcap->GetInterfaceDescriptionLists());

    //when find the host info,we update it to IpAddrList to show
    QObject::connect(aLANPcap,&LANPcap::OnUpdateHostInfo,this,&MainWindow::AddIpAddrToList);



    //bind the dynamic label with AttackSpeed using signal/slot in qt
    QObject::connect(aLANPcap,&LANPcap::OnAttackSpeedChanged,
                     [this](QString AttackSpeedDescription)
    {

        ui->AttackSpeedDLbl->setText(AttackSpeedDescription);
    });




    //when receive file send to local broadcast, we display it to ui
    QObject::connect(FileTransfer,&LANFileTransfer::OnReceiveFileSendToLocalRequest,
                     [this](QString Sender,FSendFileRequest Request)
    {


        qDebug()<<"sender "<<Sender<<" request"<<Request.toQString();

        //if detect request change,we update the display Slbl

         QList<QListWidgetItem *> items = ui->IpAddrListWidget->findItems(Sender, Qt::MatchExactly);

        if(items.size()==0)return;

        //find the item from the sender
        auto* aItem=items.first();

        if(nullptr!=aItem && aItem->text()==Sender)
        {

           aItem->setIcon(QIcon(":/T_HostMessage.png"));
           auto* CurItem=ui->IpAddrListWidget->currentItem();

           //if the item is CurItem we select, update the broadcast text and enable the recv btn
           if(CurItem!=nullptr && aItem==CurItem)
           {
               bool bHasRequest=false;
               ui->FileTransBroadcastText->setPlainText(FileTransfer->GetFileSendToLocalRequestDetail(Sender,bHasRequest));
               ui->RecvFileBtn->setEnabled(bHasRequest);
           }

        }
    });


    //bind receive online host and add it to ipaddrlist
    QObject::connect(FileTransfer,&LANFileTransfer::OnOnlineHostFound,
                     [this](QString HostIpAddr)
    {

        if(ui->HelperTab->isEnabled())return;

        //item already have

        //code replace by QSet from broadcaster
       // QList<QListWidgetItem *> items = ui->IpAddrListWidget->findItems(HostIpAddr, Qt::MatchExactly);
       // if (items.size() > 0) return;

        QListWidgetItem *Item=new QListWidgetItem(ui->IpAddrListWidget);
        Item->setText(HostIpAddr);
        Item->setIcon(QIcon(":/T_HostUserIco.png"));
        ui->IpAddrListWidget->addItem(Item);

    });


//use macro to deduce code,bind val name with tab switch and progress bar update

#define BIND_PROGRESS_WIDGET(VAL)                                             \
    QObject::connect(FileTransfer,&LANFileTransfer::On##VAL##ProgressUpdate,  \
    [this](float CurProgress,float TotalProgress)                             \
    {                                                                         \
    ui->Cur##VAL##ProgressBar->setValue(CurProgress*100);                     \
    ui->Total##VAL##ProgressBar->setValue(TotalProgress*100);                 \
                                                                              \
    if(TotalProgress>0 && 1==ui->VAL##StackedWidget->currentIndex())          \
    {                                                                         \
    ui->VAL##StackedWidget->setCurrentIndex(0);                               \
    ui->VAL##FileBtn->setEnabled(false);                                      \
    ui->SendFileBtn->setEnabled(false);                                       \
    ui->RecvFileBtn->setEnabled(false);                                       \
    }                                                                         \
                                                                              \
    if(1.f==TotalProgress && 0==ui->VAL##StackedWidget->currentIndex())       \
    {                                                                         \
    ui->VAL##StackedWidget->setCurrentIndex(1);                               \
    ui->Cur##VAL##ProgressBar->setValue(0);                                   \
    ui->Total##VAL##ProgressBar->setValue(0);                                 \
    ui->SendFileBtn->setEnabled(true);                                        \
    ui->RecvFileBtn->setEnabled(true);                                        \
}                                                                             \
});                                                                           \


BIND_PROGRESS_WIDGET(Send);
BIND_PROGRESS_WIDGET(Recv);




#undef BIND_PROGRESS_WIDGET

}






MainWindow::~MainWindow()
{
    delete ui;
}



/** ---------------------------------code provide mainwindow flat ui and drag function-------------------------------------- */
void MainWindow::FlatWindowUI()
{
    //set the window frameless with light shadow
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *ShadowEffect = new QGraphicsDropShadowEffect(this);

    const float ShadowSize=15;

    ShadowEffect->setBlurRadius(ShadowSize);
    ShadowEffect->setOffset(0);
    ShadowEffect->setColor(QColor(125,125,125));
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

//override the mouse move to simulate the drag behave
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

/**---------------------------------end code:provide mainwindow flat ui and drag function-------------------------------------- */









//add the hostindex to ipAddrList and update the progressbar
void MainWindow::AddIpAddrToList(int HostIndex)
{

    //construct a listwidgetitem and add to list
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


    //update the progressbar
    if(255!=HostIndex)
    {
        ui->ScanprogressBar->setValue(HostIndex*100/255);

    }
    else
    {
        //when update event over,we update ui info
        ui->ScanprogressBar->setValue(100);
        ui->ScanprogressBar->hide();
        ui->ScanHostsBtn->setEnabled(true);
        ui->IpAddrListWidget->setEnabled(true);
        ui->AttackCurrentStataDLbl->setPlainText("pending,suggest add target to attack ");
    }
}



//behave to toggle tab with attack and helper
void MainWindow::ToggleTab(bool bIsAttackTabClick)
{
    ui->AttackTab->setEnabled(!bIsAttackTabClick);
    ui->HelperTab->setEnabled(bIsAttackTabClick);



    ui->DisplayStackedWidget->setCurrentIndex(bIsAttackTabClick?0:1);
    ui->ActionStackedWidget->setCurrentIndex(bIsAttackTabClick?0:1);

    ui->IpAddrListWidget->clear();

    //move to new thread to avoid signal block
    QTimer::singleShot(1,
                       [this,bIsAttackTabClick]()
    {

        if(!bIsAttackTabClick)
        {
            UnlockAttacker();
        }



        FileTransfer->SetBroadcastEnable(!bIsAttackTabClick);
        qDebug()<<"FileTransfer is enable"<<FileTransfer->IsBroadcastEnable();
    });




}

void MainWindow::on_AttackTab_clicked()
{
    ToggleTab(true);
}

void MainWindow::on_HelperTab_clicked()
{
    ToggleTab(false);
}

//lock all attack target to be syn_flood
void MainWindow::on_LockAllTargetBtn_clicked()
{
    if(0==ui->IpAddrListWidget->count())return;

    //add all possible syn vulnerable host to list
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




//toggle Btn lbl with mark(|SYN |ARP), and add\remove target
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






//update the attacked host to editor text
void MainWindow::UpdateAttackStateDescription()
{
    int NumSynTargets=aSynAtker->GetTargetCounts();
    int NumArpTargets=aArpAttacker->GetTargetCounts();
    QString AttackerInfo=QString("•SynFlood Num➢ %1 VIP\n•ArpSpoof Num➢ %2 VIP").arg(NumSynTargets).arg(NumArpTargets);
    QString Header=(!bIsLockAttack)?"Targets set ▼\n":"Attacking ▼\n";
    ui->AttackCurrentStataDLbl->setPlainText(Header+AttackerInfo);

}


//lock/unlock arp and syn attack
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

//scan/rescan current host info
void MainWindow::on_ScanHostsBtn_clicked()
{
    aLANPcap->SetCurrentNetInterface(ui->InterfaceBox->currentIndex());
    ui->ScanprogressBar->setValue(0);

    ui->IpAddrListWidget->clear();
    aSynAtker->ClearTargets();
    aArpAttacker->ClearTargets();

    ui->IpAddrListWidget->setEnabled(false);
    ui->ScanprogressBar->show();
    aLANPcap->StartAnalyzeLAN();

    //disable the scan btn until the scan progress is finished
    ui->ScanHostsBtn->setEnabled(false);

    ui->AttackCurrentStataDLbl->setPlainText("Scanning the while LAN...,please wait");
}


//toggle lock/unlock attack
void MainWindow::on_LockAtkBtn_clicked()
{
    bIsLockAttack?UnlockAttacker():LockAttack();
    UpdateAttackStateDescription();
}


//when select a new item in ipAddrList, update the btn info based on it's being attacked
void MainWindow::on_IpAddrListWidget_itemSelectionChanged()
{
    QListWidgetItem *item=ui->IpAddrListWidget->currentItem();
    QString FullText=item->text();
    ui->LockSynfloodBtn->setText(FullText.contains("|SYN")?"unlock synflood target":"lock synflood target");
    ui->LockArpsproofBtn->setText(FullText.contains("|ARP")?"unlock arpspoof target":"lock arpspoof target");


    //update the open port info
    QString IpAddr=FullText.split("|")[0];
    ui->SynFloodPortsDLbl->setText(aSynAtker->GetSynFloodInfo(IpAddr));


    bool bHasRequest=false;
    ui->FileTransBroadcastText->setPlainText(FileTransfer->GetFileSendToLocalRequestDetail(IpAddr,bHasRequest));
    ui->RecvFileBtn->setEnabled(bHasRequest);
}

//set the file to send and display it to ui
void MainWindow::SetFileToSend(QString Path)
{
    if( 0!=Path.length())
    {
        ui->FileSendPathText->setPlainText(Path);
        ui->FileSendPathText->repaint();
        QString FileInfo=FileTransfer->SetFilesToSend(Path);
        ui->FileSendInfoText->setPlainText(FileInfo);
        ui->FileSendInfoText->repaint();
    }
}


void MainWindow::on_FileSendPathBtn_clicked()
{

    QString FileSendPath=QFileDialog::getOpenFileName(this);
    SetFileToSend(FileSendPath);

}

void MainWindow::on_DirSendPathBtn_clicked()
{
    QString FileSendPath=QFileDialog::getExistingDirectory(this);
    SetFileToSend(FileSendPath);
}

//set the custom receive path
void MainWindow::on_FileReceivePathBtn_clicked()
{
    QString FileSavePath=QFileDialog::getExistingDirectory(this);
    if( 0!=FileSavePath)
    {
        ui->FileReceivePathLbl->setText(FileSavePath);
        ui->FileReceivePathLbl->repaint();
        FileTransfer->SetReceiveFileDirPath(FileSavePath);
    }
}




void MainWindow::on_SendFileBtn_clicked()
{
    QString FileSendInfo=ui->FileSendInfoText->toPlainText();

    QListWidgetItem* Item=ui->IpAddrListWidget->currentItem();

    if(""!=FileSendInfo && nullptr!=Item)
    {
        QString FullText=Item->text();
        QString IpAddr=FullText.split("|")[0];
        QString SenderIp=ui->InterfaceBox->currentText().split("|")[1];

        FileTransfer->BroadcastSendFileRequire(IpAddr);


        //   FileTransfer->ConnectToReceiver(IpAddr);
    }

}


void MainWindow::on_RecvFileBtn_clicked()
{
    QListWidgetItem* Item=ui->IpAddrListWidget->currentItem();
    if(nullptr!=Item)
    {
        QString FullText=Item->text();
        QString IpAddr=FullText.split("|")[0];
        IpAddr=IpAddr.trimmed();


        FileTransfer->SetSenderHost(IpAddr);
    }
}




QString MainWindow::GetCurrentInterfaceIpAddr() const
{
    QString IpAddr=ui->InterfaceBox->currentText().split("|")[1];
    IpAddr=IpAddr.trimmed();
    return IpAddr;
}



void MainWindow::on_InterfaceBox_currentIndexChanged(int index)
{
    if(index<0)return;

    ui->IpAddrListWidget->clear();

    //move to new thread to avoid signal block,operation not related to ui will lead progress crash
    QTimer::singleShot(1,
                       [this]()
    {
        FileTransfer->SetLocalHost(this->GetCurrentInterfaceIpAddr());
    });



}






void MainWindow::on_AttackPowerSlider_valueChanged(int value)
{
   aLANPcap->SetAttackPower(value);
}
