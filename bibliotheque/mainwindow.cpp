#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>

#include <QDate>
#include <QTimer>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QPen>

#include <QtCharts>
#include <QLayout>

/*=================================================================*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*============================================================*/
    ui->formWidget->hide();
    ui->aboutMember->hide();
    ui->resultMembre->show();
    /*============================================================*/

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("biblio");
    db.setHostName("localhost");
    db.setUserName("Miarimanana");
    db.setPassword("miarimanana2006");

    if (!db.open()) {
        qDebug() << "DATABASE IS NOT CONNECTED :(";
        qDebug() << db.lastError().text();  // Print detailed error message
        return;
    }

    qDebug() << "DATABASE IS CONNECTED :)";

    // Print available drivers for debugging
    QStringList drivers = QSqlDatabase::drivers();
    qDebug() << "Available drivers:" << drivers;

    QSqlQuery query(db);
    query.prepare("SELECT * FROM adherent");


    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête :" << query.lastError().text();
        return;
    }

    while(query.next()){
        QString nom = query.value("NOM").toString();
        QString prenom =query.value("PRENOM").toString();

        qDebug() << "Nom : "<<nom<<"\t"<<"Prénom : "<<prenom<<"\n";
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}


/*========================================*/

QString new_members(QSqlQuery query){
    query.exec("SELECT COUNT(*) FROM ADHERENT WHERE DATEADHESION > DATE_SUB(CURDATE(), INTERVAL 4 WEEK)") ;
    QString result ;
    while(query.next())
        result = query.value(0).toString() ;
    return result ;
}
QString ex_members(QSqlQuery query){
    query.exec("SELECT COUNT(*) FROM ADHERENT WHERE DATE_SUB(CURDATE(), INTERVAL 1 YEAR)>DATEADHESION")  ;
    QString result ;
    while(query.next())
        result = query.value(0).toString() ;
    return result ;
}
QString particuliers(QSqlQuery query){
    query.exec("SELECT count(*) FROM ADHERENT WHERE NUMADHERENT REGEXP '.*P.*' ;")  ;
    QString result ;
    while(query.next())
        result = query.value(0).toString() ;
    return result ;
}
QString students(QSqlQuery query){
    query.exec("SELECT count(*) FROM ADHERENT WHERE NUMADHERENT REGEXP '.*E.*' ;")  ;
    QString result ;
    while(query.next())
        result = query.value(0).toString() ;
    return result ;
}
QString total_members(QSqlQuery query){
    query.exec("SELECT count(*) FROM ADHERENT WHERE 1;")  ;
    QString result ;
    while(query.next())
        result = query.value(0).toString() ;
    return result ;
}
int emprunteurs(QSqlQuery query, int x) {
    query.prepare("SELECT COUNT(*) FROM EMPRUNTER WHERE DATE_PRET = DATE_SUB(CURDATE(), INTERVAL :X DAY) ;") ;
    query.bindValue(":X",x) ;
    query.exec() ;
    int result ;
    while(query.next())
        result = query.value(0).toInt() ;
    return result ;
}

/*=========================================*/


void MainWindow::on_btnAccueil_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_btnDash_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

    QWidget *barChartContainer = ui->frame_10;

    //ui->new_books_count->setStyleSheet("font-size: 30px;") ;


    /*========================================================*/

    QSqlQuery query;
    ui->newBook->setText(new_members(query)) ;
    ui->oldMember->setText(ex_members(query)) ;
    ui->nombreParticulier->setText(particuliers(query)) ;
    ui->nombreEtudiant->setText(students(query)) ;
    ui->totalMember->setText(total_members(query)) ;
    ui->newMember->setText(new_members(query)) ;
    /*=========================================================*/

    if (barChartContainer->layout() == nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(barChartContainer);
        barChartContainer->setLayout(layout);
    }
    else {
        QLayoutItem *item;
        while ((item = barChartContainer->layout()->takeAt(0))) {
            delete item->widget();
            delete item;
        }
    }

    QBarSeries *series = new QBarSeries();
    QBarSet *set_1 = new QBarSet("Emprunteurs");

    QDate date ;
    QDate y = date.currentDate() ;
    int x = y.dayOfWeek();
    for (int i = 7; i > 0; i--) {
        if((x-i+7) % 7 != 0){
            //qDebug() << emprunteurs(query,(x-i+7)%7) ;
            set_1->append(emprunteurs(query,(x-i+7)%7)) ;
        }
    }
    set_1->setPen(QPen(Qt::blue, 1));
    set_1->setBrush(Qt::blue);

    series->append(set_1);

    m_chart = new QChart();
    m_chart->addSeries(series);
    m_chart->setTitle("EMPRUNTEURS");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList jours;
    for (int i = 6; i >= 0; --i) {
        jours.append(QDate::currentDate().addDays(-i).toString("ddd"));
    }

    QBarCategoryAxis *axeX = new QBarCategoryAxis();
    axeX->append(jours);
    m_chart->addAxis(axeX, Qt::AlignBottom);
    series->attachAxis(axeX);

    QValueAxis *axeY = new QValueAxis();
    axeY->setRange(0, 5);
    m_chart->addAxis(axeY, Qt::AlignLeft);
    series->attachAxis(axeY);

    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *barChartView = new QChartView(m_chart);
    barChartView->setRenderHint(QPainter::Antialiasing);

    barChartContainer->layout()->addWidget(barChartView);

    QWidget *pieChartContainer = ui->frame_11;



    if (pieChartContainer->layout() == nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(pieChartContainer);
        pieChartContainer->setLayout(layout);
    }
    else {
        QLayoutItem *item;
        while ((item = pieChartContainer->layout()->takeAt(0))) {
            delete item->widget();
            delete item;
        }
    }

    QPieSeries *series_2 = new QPieSeries();
    series_2->setHoleSize(0.5);

    QPieSlice *slice_1 = series_2->append("Nouveaux", 30);
    //slice_1->setLabelVisible(true);
    slice_1->setPen(QPen(Qt::green, 1));
    slice_1->setBrush(Qt::green);

    QPieSlice *slice_2 = series_2->append("Perdus", 5);
    //slice_2->setLabelVisible(true);
    slice_2->setPen(QPen(Qt::red, 1));
    slice_2->setBrush(Qt::red);

    QPieSlice *slice_3 = series_2->append("Retournés", 20);
    //slice_3->setLabelVisible(true);
    slice_3->setPen(QPen(Qt::yellow, 1));
    slice_3->setBrush(Qt::yellow);

    QPieSlice *slice_4 = series_2->append("Sortis", 15);
    //slice_4->setLabelVisible(true);
    slice_4->setPen(QPen(Qt::blue, 1));
    slice_4->setBrush(Qt::blue);

    QChart *pieChart = new QChart();
    pieChart->addSeries(series_2);
    pieChart->legend()->setVisible(true);
    pieChart->setTitle("Rapport Au Total de Livres");
    pieChart->legend()->setAlignment(Qt::AlignBottom);
    pieChart->setAnimationOptions(QChart::SeriesAnimations);

    QChartView *pieChartView = new QChartView(pieChart);
    pieChartView->setRenderHint(QPainter::Antialiasing);

    pieChartContainer->layout()->addWidget(pieChartView);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateChart);
    timer->start(86400000);

    updateChart();

    /*==================CHART MEMBERS==================*/
    QWidget *memberChart = ui->memberChart;

    if(memberChart->layout() == nullptr){
        QVBoxLayout *layout = new QVBoxLayout(memberChart);
        memberChart->setLayout(layout);
    }

    else{
        QLayoutItem *item;
        while((item = memberChart->layout()->takeAt(0))){
            delete item->widget();
            delete item;
        }
    }

   QPieSeries *series_3 = new QPieSeries();
   series_3->setHoleSize(0);

   QPieSlice *slc_1 = series_3->append("PARTICULIERS",particuliers(query).toInt());
   slc_1->setPen(QPen(Qt::blue, 0));
   slc_1->setBrush(Qt::blue);

   QPieSlice *slc_2 = series_3->append("ETUDIANTS", students(query).toInt());
   slc_2->setPen(QPen(Qt::black, 0));
   slc_2->setBrush(Qt::black);

   QChart *pieMember = new QChart();
   pieMember->addSeries(series_3);
   pieMember->setAnimationOptions(QChart::SeriesAnimations);

   QChartView *pieMemberChartview = new QChartView(pieMember);
   pieMemberChartview->setRenderHint(QPainter::Antialiasing);

   memberChart->layout()->addWidget(pieMemberChartview);

}



/*=========================UPDATE=========================*/
void MainWindow::updateChart()
{
    QStringList dayLabels;
    int count = 0;
    QDate currentDate = QDate::currentDate();

    for (int i = 0; count < 7; ++i) {
        QDate date = currentDate.addDays(-i);
        if (date.dayOfWeek() != 7) {
            dayLabels.prepend(date.toString("ddd"));
            count++;
        }
    }

    auto axeX = qobject_cast<QBarCategoryAxis*>(m_chart->axisX());
    if (axeX) {
        axeX->clear();
        axeX->append(dayLabels);
    }
}

/*=========================================================*/

void MainWindow::on_btnMember_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    displayData(ui->resultMembre);
}
/*===========================================*/

void MainWindow::affichageMembre(){
    QTableWidget *table = ui->resultMembre;

    if(table){
        ui->resultMembre->activateWindow();
    }

    else{
        ui->resultMembre->raise();
    }
}

/*============================================*/

void MainWindow::on_btnBook_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}


void MainWindow::on_btnEmprunt_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::on_btnCommunication_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}


void MainWindow::on_btnSetting_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}


void MainWindow::on_btnAbout_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}


void MainWindow::on_btnHelp_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
}

/*=====================================================*/

void MainWindow::displayData(QTableWidget *tableView){
    tableView->setColumnCount(6) ;
    tableView->setHorizontalHeaderLabels({"ID","NOM","PRENOM", "TELEPHONE","EMAIL","ADRESSE"}) ;
    tableView->setColumnWidth(0,130) ;
    tableView->setColumnWidth(1,200) ;
    tableView->setColumnWidth(2,180) ;
    tableView->setColumnWidth(3,120) ;
    tableView->setColumnWidth(4,230) ;
    tableView->setColumnWidth(5,150) ;

    tableView->setAlternatingRowColors(true);

    tableView->setStyleSheet(
         "QTableWidget::item {"
         "  background-color : #98caf8;"
         "}"

         "QTableWidget::item:alternate {"
         "  background-color : #e3f4f7;"
         "}"

         "QTableWidget::item {"
         "  color : #101f36;"
         "  padding-top : 15px;"
         "  padding-bottom : 15px;"
         "}"

    );

    QSqlQuery query(db);
    query.prepare("SELECT NUMADHERENT, NOM, PRENOM, TEL, EMAIL, ADRESSE FROM adherent WHERE 1;");

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête :" << query.lastError().text();
        return;
    }
    int row = 0;

    while(query.next()){
        tableView->insertRow(row) ;
        QLabel *Action = new QLabel ;
        Action->setText("Modifier") ;
        QTableWidgetItem *num = new QTableWidgetItem(query.value(0).toString()) ;
        QTableWidgetItem *nom = new QTableWidgetItem(query.value(1).toString()) ;
        QTableWidgetItem *prenom = new QTableWidgetItem(query.value(2).toString()) ;
        QTableWidgetItem *tel = new QTableWidgetItem(query.value(3).toString()) ;
        QTableWidgetItem *mail = new QTableWidgetItem(query.value(4).toString()) ;
        QTableWidgetItem *adress = new QTableWidgetItem(query.value(5).toString()) ;

        num->setFlags(num->flags() & ~Qt::ItemIsEditable) ;
        nom->setFlags(nom->flags() & ~Qt::ItemIsEditable) ;
        prenom->setFlags(prenom->flags() & ~Qt::ItemIsEditable) ;
        tel->setFlags(tel->flags() & ~Qt::ItemIsEditable) ;
        mail->setFlags(mail->flags() & ~Qt::ItemIsEditable) ;
        adress->setFlags(adress->flags() & ~Qt::ItemIsEditable) ;

        tableView->setItem(row, 0, num) ;
        tableView->setItem(row, 1, nom) ;
        tableView->setItem(row, 2, prenom) ;
        tableView->setItem(row, 3, tel) ;
        tableView->setItem(row, 4, mail) ;
        tableView->setItem(row, 5, adress) ;
//        if(tableView->item(row,0)->isS+elected() or tableView->item(row,1)->isSelected() or tableView->item(row,2)->isSelected() or tableView->item(row,3)->isSelected()  or tableView->item(row,4)->isSelected() ){
//            ui->formWidget->show();
//        }
        row++;
     }

}

/*============================================================================*/
/*
QString new_members(QSqlQuery query){
    query.exec("SELECT COUNT(*) FROM ADHERENT WHERE DATEADHESION > DATE_SUB(CURDATE(), INTERVAL 4 WEEK)") ;
    QString result ;
    while(query.next())
        result = query.value(0).toString() ;
    return result ;
}
QString ex_members(QSqlQuery query){
    query.exec("SELECT COUNT(*) FROM ADHERENT WHERE DATE_SUB(CURDATE(), INTERVAL 1 YEAR)>DATEADHESION")  ;
    QString result ;
    while(query.next())
        result = query.value(0).toString() ;
    return result ;
}
QString particuliers(QSqlQuery query){
    query.exec("SELECT count(*) FROM ADHERENT WHERE NUMADHERENT REGEXP '.*P.*' ;")  ;
    QString result ;
    while(query.next())
        result = query.value(0).toString() ;
    return result ;
}
QString students(QSqlQuery query){
    query.exec("SELECT count(*) FROM ADHERENT WHERE NUMADHERENT REGEXP '.*E.*' ;")  ;
    QString result ;
    while(query.next())
        result = query.value(0).toString() ;
    return result ;
}
QString total_members(QSqlQuery query){
    query.exec("SELECT count(*) FROM ADHERENT WHERE 1;")  ;
    QString result ;
    while(query.next())
        result = query.value(0).toString() ;
    return result ;
}
int emprunteurs(QSqlQuery query, int x) {
    query.prepare("SELECT COUNT(*) FROM EMPRUNTER WHERE DATE_PRET = DATE_SUB(CURDATE(), INTERVAL :X DAY) ;") ;
    query.bindValue(":X",x) ;
    query.exec() ;
    int result ;
    while(query.next())
        result = query.value(0).toInt() ;
    return result ;
}
*/

void research_member(QTableWidget *tableView, QSqlQuery query, QString text){
    QString condition, Num, Nom, Prenom, Tel, Email, Adresse;
    Num = "NUMADHERENT LIKE '%"+text+"%' OR " ;
    Nom = "NOM LIKE '%"+text+"%' OR " ;
    Prenom = "PRENOM LIKE '%"+text+"%' OR " ;
    Tel = "TEL LIKE '%"+text+"%' OR " ;
    Email = "EMAIL LIKE '%"+text+"%' OR " ;
    Adresse = "ADRESSE LIKE '%"+text+"%'" ;
    condition = Num+Nom+Prenom+Tel+Email+Adresse;
    query.prepare("SELECT NUMADHERENT, NOM, PRENOM, TEL, EMAIL, ADRESSE adherent WHERE "+condition);

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête :" << query.lastError().text();
        return;
    }
    int row = 0;

    while(query.next()){
        tableView->insertRow(row) ;
        QLabel *Action = new QLabel ;
        Action->setText("Modifier") ;
        QTableWidgetItem *num = new QTableWidgetItem(query.value(0).toString()) ;
        QTableWidgetItem *nom = new QTableWidgetItem(query.value(1).toString()) ;
        QTableWidgetItem *prenom = new QTableWidgetItem(query.value(2).toString()) ;
        QTableWidgetItem *tel = new QTableWidgetItem(query.value(3).toString()) ;
        QTableWidgetItem *mail = new QTableWidgetItem(query.value(4).toString()) ;
        QTableWidgetItem *adress = new QTableWidgetItem(query.value(5).toString()) ;

        num->setFlags(num->flags() & ~Qt::ItemIsEditable) ;
        nom->setFlags(nom->flags() & ~Qt::ItemIsEditable) ;
        prenom->setFlags(prenom->flags() & ~Qt::ItemIsEditable) ;
        tel->setFlags(tel->flags() & ~Qt::ItemIsEditable) ;
        mail->setFlags(mail->flags() & ~Qt::ItemIsEditable) ;
        adress->setFlags(adress->flags() & ~Qt::ItemIsEditable) ;

        tableView->setItem(row, 0, num) ;
        tableView->setItem(row, 1, nom) ;
        tableView->setItem(row, 2, prenom) ;
        tableView->setItem(row, 3, tel) ;
        tableView->setItem(row, 4, mail) ;
        tableView->setItem(row, 5, adress) ;
//        if(tableView->item(row,0)->isS+elected() or tableView->item(row,1)->isSelected() or tableView->item(row,2)->isSelected() or tableView->item(row,3)->isSelected()  or tableView->item(row,4)->isSelected() ){
//            ui->formWidget->show();
//        }
        row++;
     }
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    QSqlQuery query;
    research_member(ui->resultMembre,query, arg1);
}


void MainWindow::on_btnAddMember_clicked()
{
    /*
    int width = ui->resultMembre->width();
    ui->fromAdd->setMinimumWidth(500);
    ui->resultMembre->setMaximumWidth(width-500);
    */

//    QString picture = ":/icons/user-plus.svg";
//    insertImage(ui->portProfil, picture);
    ui->formWidget->show();
    ui->resultMembre->hide();
}

/*
void MainWindow::on_btnBack0_clicked()
{
     ui->resultMembre->show();
     ui->formWidget->hide();
}
*/

void MainWindow::on_btnBack_clicked()
{
    ui->resultMembre->show();
    ui->formWidget->hide();
}


void MainWindow::on_btnClose_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Confirmation","Vouler-vous enregistrer?",QMessageBox::Save | QMessageBox::Close);
    if(reply == QMessageBox::Close){
        ui->formWidget->hide();
        ui->resultMembre->show();
    }

    else if(reply == QMessageBox::Save){
        ui->formWidget->hide();
        /*====enrgistrer les inputs==============*/
    }
}

void MainWindow::insertImage(QWidget *parent, QString path){
    QPixmap pixmap(path);
    QWidget *m_widget = parent;
    QHBoxLayout *h_layout = new QHBoxLayout(this);
    QLabel *label = new QLabel(this);
    label->setMaximumHeight(200);
    label->setMaximumWidth(200);
    label->setScaledContents(true);
    label->setPixmap(pixmap);
    m_widget->setLayout(h_layout);
    h_layout->addWidget(label);
}


QString MainWindow::getPathImage(){
    QString fileName = QFileDialog::getOpenFileName(nullptr,"Choisir une image","","Images(*.png *.jpg *.jpeg *.bmp *.gif)");
    if(fileName.isEmpty()){
        QMessageBox::warning(this, "Warning", "Impossible d'ajouter l'image ou peut-etre que vous n'en choisissez rien!");
        return "";
    }
    QMessageBox::information(this,"Success", "Image est ajoutee");
    return fileName;
}



void MainWindow::on_btnGetImage_clicked()
{
    QString source = getPathImage();
    insertImage(ui->portProfil, source);
}


void MainWindow::on_btnAnnuler_clicked()
{
   QMessageBox::StandardButton reply;
   reply = QMessageBox::question(this,"Confirmation","Vouler vous annuler l'enregistrement?",QMessageBox::Yes | QMessageBox::No);
   if(reply == QMessageBox::Yes){
       ui->formWidget->hide();
   }
}


void MainWindow::on_btnEnregistrer_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Confirmation","Vouler-vous enregistrer?",QMessageBox::Save | QMessageBox::No);
    if(reply == QMessageBox::Save){
        ui->formWidget->hide();
        //faire ici l'enrgistrement( requete SQL)
    }
}

/*===============MODIFIER MEMBRE ===================*/

void MainWindow::on_pushButton_13_clicked()
{
     ui->aboutMember->show();
     ui->formWidget->hide();
     ui->resultMembre->hide();

     ui->aboutMember->setMaximumWidth(500);
}



void MainWindow::on_btnCancelAction_clicked()
{
    ui->aboutMember->hide();
    ui->formWidget->hide();
    ui->resultMembre->show();
}


void MainWindow::on_btnModify_clicked()
{
    ui->aboutMember->hide();
    ui->formWidget->show();
    ui->resultMembre->hide();

    ui->formWidget->setMaximumWidth(1000);
}

