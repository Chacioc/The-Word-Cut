#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <bits/stdc++.h>
#include <QDebug>
#include <QDir>
#include <QRandomGenerator>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init() {
    QDir dir("./data");
    QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    qDebug() << files.size() << endl;
    ui->comboBox->addItems(files);
}


void MainWindow::on_pushButton_3_clicked()
{
    QString filestring = "data/" + ui->comboBox->currentText();
    QFile file(filestring);
    qDebug() << filestring;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        QString all = in.readAll(), tmp;
        QStringList ans;
        bool cnt = false;
        for (int i = 0; i < all.size(); i ++) {
            if (all[i] == '\"') {
                cnt = !cnt;
            } else if (all[i] == ',' || all[i] == '\n') {
                if (cnt) {
                    tmp += all[i];
                } else {
                    ans.push_back(tmp);
                    tmp = "";
                }
            } else {
                tmp += all[i];
            }
        }
        if (tmp.size()) {
            ans.push_back(tmp);
            tmp = "";
        }
        for (int i = 0; i + 1 < ans.size(); i += 2) {
           dicts.push_back({ans.at(i), ans.at(i + 1)});
        }
        file.close();
    }
    QFile file2("data/now.csv");
    QMap<QString, int> mymap;
    if (file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList field = line.split(',');
            mymap[field.at(0)] = field.at(1).toInt();
        }
        file.close();
    }
    now = mymap[filestring];
    qDebug() << (dicts.size());
    ui->stackedWidget->setCurrentIndex(1);
    le = (ui->comboBox_2->currentIndex() + 1) * 10;
    start = std::max(0, now - 3 * le);
    end = std::min(dicts.size(), now + le);
    if (dicts.size()) {
        for (int i = start; i < now; i ++) {
            que1.enqueue({dicts[i], 1});
        }
        for (int i = now; i < end; i ++) {
            que2.enqueue({dicts[i], 1});
        }
        flag = (que1.size() == 0);
        ui->textBrowser->setPlainText(dicts[start].first);
        qDebug() << now << ' ' << que1.size() << ' ' << QString(now - que1.size());
        ui->textBrowser_2->setPlainText(QString::number(now - que1.size()) + '/' + QString::number(now - start));
        ui->textBrowser_3->setPlainText(QString::number(end - que2.size()) + '/' + QString::number(end - now));
    } else {
        ui->textBrowser->setPlainText("ERROR!");
    }
}

void MainWindow::on_pushButton_clicked()
{
    if (dicts.size()) {
        if (!flag) {
            QPair<QPair<QString, QString>, int> tmp = que1.dequeue();
            if (-- tmp.second > 0) {
                que1.enqueue(tmp);
            } else {
                ui->textBrowser_2->setPlainText(QString::number(now - que1.size()) + '/' + QString::number(now - start));
                flag = (que1.size() == 0);
            }
            if (!que1.empty()) {
                tmp = que1.head();
                ui->textBrowser->setPlainText(tmp.first.first);
            } else if (!que2.empty()) {
                tmp = que2.head();
                ui->textBrowser->setPlainText(tmp.first.first);
            } else {
                now += end;
                QMessageBox::information(nullptr, "提示", "学习完成！");
                ui->stackedWidget->setCurrentIndex(0);
            }
        } else {
            QPair<QPair<QString, QString>, int> tmp = que2.dequeue();
            if (-- tmp.second > 0) {
                que2.enqueue(tmp);
            } else {
                ui->textBrowser_3->setPlainText(QString::number(end - que2.size()) + '/' + QString::number(end - now));
            }
            if (!que2.empty()) {
                tmp = que2.head();
                ui->textBrowser->setPlainText(tmp.first.first);
            } else {
                now += end;
                QMessageBox::information(nullptr, "提示", "学习完成！");
                ui->stackedWidget->setCurrentIndex(0);
            }
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (dicts.size()) {
        if (!flag) {
            QPair<QPair<QString, QString>, int> &tmp = que1[0];
            tmp.second += 3;
            ui->textBrowser->setPlainText(tmp.first.first + '\n' + tmp.first.second);
        } else {
            QPair<QPair<QString, QString>, int> &tmp = que2[0];
            tmp.second += 3;
            ui->textBrowser->setPlainText(tmp.first.first + '\n' + tmp.first.second);
        }
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    now += end;
    dicts.clear(); que1.clear(); que2.clear();
}
