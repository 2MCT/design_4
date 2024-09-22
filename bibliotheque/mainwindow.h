#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //QSqlDatabase db;
private slots:
    void on_btnDash_clicked();

    void on_btnMember_clicked();

    void on_btnBook_clicked();

    void on_btnEmprunt_clicked();

    void on_btnCommunication_clicked();

    void on_btnSetting_clicked();

    void on_btnAbout_clicked();

    void on_btnHelp_clicked();

    void on_btnAccueil_clicked();

    /*===================*/
    void updateChart();
    /*===================*/

    QString getPathImage();

    void on_lineEdit_textChanged(const QString &arg1);



    void on_btnAddMember_clicked();

    //void on_pushButton_12_clicked();

    void on_btnBack_clicked();


    void on_btnClose_clicked();

    void on_btnGetImage_clicked();

    void on_btnAnnuler_clicked();

    void on_btnEnregistrer_clicked();

    void on_pushButton_13_clicked();

    void on_btnCancelAction_clicked();

    void on_btnModify_clicked();

private:
    Ui::MainWindow *ui;

    QSqlDatabase db;
    QSqlQuery query;

    QChart *m_chart;

    void displayData(QTableWidget *tableView);

    void affichageMembre();

    void insertImage(QWidget *parent, QString path);

};
#endif // MAINWINDOW_H
