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





   // void on_StopAtkBtn_clicked();

    //void on_StartAtkBtn_clicked();

    //void on_RemoveTargetBtn_clicked();

   // void on_ScanLANBtn_clicked();

    void AddIpAddrToList(int HostIndex);

   // void on_AddAllTargetBtn_clicked();



    void on_CloseBtn_clicked();

    void on_MinimizeBtn_clicked();


    void on_AttackTab_clicked();

    void on_HelperTab_clicked();

    void on_LockAllTargetBtn_clicked();

    void on_LockSynfloodBtn_clicked();

    void on_LockArpsproofBtn_clicked();

    void on_ScanHostsBtn_clicked();

    void on_LockAtkBtn_clicked();

    void on_IpAddrListWidget_itemChanged(class QListWidgetItem *item);

    void on_IpAddrListWidget_itemActivated(QListWidgetItem *item);

    void on_IpAddrListWidget_itemSelectionChanged();

private:

    void AddHostToIpAddrList(int HostIndex);


    void ToggleCurrentItemDisplayInfo(class NetAttacker *Attacker, QString Mark,class QPushButton *ToggleBtn);


    void UpdateAttackStateDescription();

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
