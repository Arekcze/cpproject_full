#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "random.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString file_holding_report = "C:\\Users\\arek\\Documents\\nn\\xd14.txt";
    std::string file_holding_report_converted_to_utf= file_holding_report.toUtf8().constData();
    QString file_holding_net_weights = "C:\\Users\\arek\\Documents\\nn\\xd15.txt";
    std::string file_holding_net_weights_converted_to_utf= file_holding_net_weights.toUtf8().constData();
    string adress_of_the_file_from_which_data_is_given = "C:\\Users\\arek\\Documents\\nn\\data3.txt";
    //string input_file = "C:\\Users\\arek\\source\\repos\\ConsoleApplication8\\ConsoleApplication8\\data3.txt";
    run_neural_net(file_holding_report_converted_to_utf, file_holding_net_weights_converted_to_utf, adress_of_the_file_from_which_data_is_given);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_pushButton_clicked()
{
    QString file_holding_report = "C:\\Users\\arek\\Documents\\nn\\xd14.txt";

        QFile file(file_holding_report);
        if(!file.open(QIODevice::ReadOnly)){QMessageBox::information(0,"info",file.errorString());}
        QTextStream in(&file);
        ui->textBrowser->setText(in.readAll());
}

void MainWindow::on_pushButton_2_clicked()
{
    QString file_holding_net_weights = "C:\\Users\\arek\\Documents\\nn\\xd15.txt";
    QFile file2(file_holding_net_weights);
    if(!file2.open(QIODevice::ReadOnly)){QMessageBox::information(0,"info",file2.errorString());}
    QTextStream in2(&file2);
    ui->textBrowser_2->setText(in2.readAll());


}
