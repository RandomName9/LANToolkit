#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



protected:

    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
private slots:







    void AddIpAddrToList(int HostIndex);





    void on_CloseBtn_clicked();

    void on_MinimizeBtn_clicked();


    void on_AttackTab_clicked();

    void on_HelperTab_clicked();

    void on_LockAllTargetBtn_clicked();

    void on_LockSynfloodBtn_clicked();

    void on_LockArpsproofBtn_clicked();

    void on_ScanHostsBtn_clicked();

    void on_LockAtkBtn_clicked();



    void on_IpAddrListWidget_itemSelectionChanged();

    void on_FileSendPathBtn_clicked();

    void on_DirSendPathBtn_clicked();

    void on_FileReceivePathBtn_clicked();

    void on_SendFileBtn_clicked();


   // void on_AcceptFilesBtn_clicked();


   // void on_InterfaceBox_currentTextChanged(const QString &arg1);

   // void on_InterfaceBox_currentIndexChanged(int index);

    void on_InterfaceBox_currentIndexChanged(int index);



    void on_RecvFileBtn_clicked();





    void on_AttackPowerSlider_valueChanged(int value);

private:




    void ToggleCurrentItemDisplayInfo(class NetAttacker *Attacker, QString Mark,class QPushButton *ToggleBtn);

    void ToggleTab(bool bIsAttackTabClick);

    QString GetCurrentInterfaceIpAddr()const;

    void UpdateAttackStateDescription();


    void SetFileToSend(QString Path);

    void LockAttack();
    void UnlockAttacker();

    void FlatWindowUI();



private:
    Ui::MainWindow *ui;


    class LANPcap* aLANPcap;
    class SynFloodAttacker* aSynAtker;
    class ArpAttacker* aArpAttacker;

    bool bIsLockAttack=false;

    QPoint MousePos; //position when mouse pressed
    QPoint WindowPos;  //local position of the this window

    class LANFileTransfer *FileTransfer;

};

#endif // MAINWINDOW_H
