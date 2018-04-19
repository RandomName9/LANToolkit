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


    void on_AddTargetBtn_clicked();


    void on_StopAtkBtn_clicked();

    void on_StartAtkBtn_clicked();

    void on_RemoveTargetBtn_clicked();

    void on_ScanLANBtn_clicked();

    void AddIpAddrToList(int HostIndex);

    void on_AddAllTargetBtn_clicked();

    void on_CloseBtn_clicked();

    void on_MinimizeBtn_clicked();

private:

    void AddHostToIpAddrList(int HostIndex);

    void FlatWindowUI();


private:
    Ui::MainWindow *ui;

    int SendPacketNum=0;
    class LANPcap* aLANPcap;
    class SynFloodAttacker* aSynAtker;

    QPoint MousePos; //position when mouse pressed
    QPoint WindowPos;  //local position of the this window
};

#endif // MAINWINDOW_H
