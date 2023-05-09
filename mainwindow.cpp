#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <bits/stdc++.h>
#include <QDebug>
#include <QDir>
//#include <QRandomGenerator>
#include <QMessageBox>
#include <QtWidgets>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    QFile file("data/now.csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        for (auto it = mymap.begin(); it != mymap.end(); ++ it) {
            if (it.key().contains(',') || it.key().contains('\n')) {
                out << '\"' + it.key() + '\"';
            } else {
                out << it.key();
            }
            out << ',';
            out << QString::number(it.value());
            out << '\n';
        }
        file.close();
    }
    saveCSV("data/notes.csv", notes);
    saveCSV("data/favorites.csv", favorites);
    delete ui;
}

void MainWindow::init() {
    QDir dir("./library");
    QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    qDebug() << files.size() << endl;
    ui->comboBox->addItems(files);
    QFile file2("data/now.csv");
    if (file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file2);
        in.setCodec("UTF-8");
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList field = line.split(',');
            mymap[field.at(0)] = field.at(1).toInt();
        }
        file2.close();
    }
    QStringList ans = readCSV("data/notes.csv");
    for (int i = 0; i + 1 < ans.size(); i += 2) {
       notes[ans.at(i)] = ans.at(i + 1);
    }
    ans = readCSV("data/favorites.csv");
    for (int i = 0; i + 1 < ans.size(); i += 2) {
       favorites[ans.at(i)] = ans.at(i + 1);
    }
    db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("word.db");
}


void MainWindow::on_pushButton_3_clicked()
{
    QString filestring = "library/" + ui->comboBox->currentText();
    QFile file(filestring);
    qDebug() << filestring;
    dicts.clear(); que1.clear(); que2.clear();
    auto ans = readCSV(filestring);
    qDebug() << ans.size();
    for (int i = 0; i + 1 < ans.size(); i += 2) {
       dicts.push_back({ans.at(i), ans.at(i + 1)});
    }
    now = &mymap[filestring];
    qDebug() << (dicts.size());
    ui->stackedWidget->setCurrentIndex(1);
    le = (ui->comboBox_2->currentIndex() + 1) * 10;
    start = std::max(0, (*now) - 3 * le);
    end = std::min(dicts.size(), (*now) + le);
    if (dicts.size()) {
        for (int i = start; i < (*now); i ++) {
            que1.enqueue({dicts[i], 1});
        }
        for (int i = (*now); i < end; i ++) {
            que2.enqueue({dicts[i], 1});
        }
        flag = (que1.size() == 0);
        ui->textBrowser->setPlainText(dicts[start].first);
        int le1 = (*now) - start, le2 = end - (*now);
        qDebug() << now << ' ' << que1.size() << ' ' << QString((*now) - que1.size());
        ui->textBrowser_2->setPlainText(QString::number(le1 - que1.size()) + '/' + QString::number(le1));
        ui->textBrowser_3->setPlainText(QString::number(le2 - que2.size()) + '/' + QString::number(le2));
    } else {
        ui->textBrowser->setPlainText("ERROR!");
    }
    ui->pushButton_9->setEnabled(false);
    ui->pushButton_10->setEnabled(true);
    ui->textEdit_2->clear();
    ui->textEdit_2->setReadOnly(true);
    ui->textEdit_2->setVisible(false);
    ui->label_10->setVisible(false);
    ui->pushButton_9->setVisible(false);
    ui->pushButton_13->setVisible(false);
}

void MainWindow::on_pushButton_clicked()
{
    if (dicts.size()) {
        int le1 = (*now) - start, le2 = end - (*now);
        if (!flag) {
            QPair<QPair<QString, QString>, int> tmp = que1.dequeue();
            if (-- tmp.second > 0) {
                que1.enqueue(tmp);
            } else {
                ui->textBrowser_2->setPlainText(QString::number(le1 - que1.size()) + '/' + QString::number(le1));
                flag = (que1.size() == 0);
            }
            if (!que1.empty()) {
                tmp = que1.head();
                ui->textBrowser->setPlainText(tmp.first.first);
            } else if (!que2.empty()) {
                tmp = que2.head();
                ui->textBrowser->setPlainText(tmp.first.first);
            } else {
                (*now) += end;
                QMessageBox::information(nullptr, "提示", "学习完成！");
                ui->stackedWidget->setCurrentIndex(0);
            }
        } else {
            QPair<QPair<QString, QString>, int> tmp = que2.dequeue();
            if (-- tmp.second > 0) {
                que2.enqueue(tmp);
            } else {
                ui->textBrowser_3->setPlainText(QString::number(le2 - que2.size()) + '/' + QString::number(le2));
            }
            if (!que2.empty()) {
                tmp = que2.head();
                ui->textBrowser->setPlainText(tmp.first.first);
            } else {
                (*now) += end;
                QMessageBox::information(nullptr, "提示", "学习完成！");
                ui->stackedWidget->setCurrentIndex(0);
            }
        }
        ui->pushButton_8->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->textEdit_2->clear();
        ui->pushButton_9->setEnabled(false);
        ui->pushButton_10->setEnabled(true);
        ui->textEdit_2->setReadOnly(true);
        ui->textEdit_2->setVisible(false);
        ui->label_10->setVisible(false);
        ui->pushButton_9->setVisible(false);
        ui->pushButton_13->setVisible(false);
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
        ui->pushButton_2->setEnabled(false);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    (*now) += end;
    dicts.clear(); que1.clear(); que2.clear();
}

void MainWindow::saveCSV(QString filestring, QMap<QString, QString> dic) {
    QStringList ans;
    QFile file(filestring);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        for (auto it = dic.begin(); it != dic.end(); ++ it) {
            if (it.key().contains(',') || it.key().contains('\n')) {
                out << '\"' + it.key() + '\"';
            } else {
                out << it.key();
            }
            out << ',';
            if (it.value().contains(',') || it.value().contains('\n')) {
                out << '\"' + it.value() + '\"';
            } else {
                out << it.value();
            }
            out << '\n';
        }
        file.close();
    }
}

QStringList MainWindow::readCSV(QString filestring) {
    QFile file(filestring);
    qDebug() << filestring;
    QStringList ans;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        QString all = in.readAll(), tmp;
        qDebug() << all.size();
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
        file.close();
    }
    return ans;
}

void MainWindow::on_pushButton_6_clicked()
{
    QString filestring = "library/" + ui->comboBox->currentText();
    QStringList ans = readCSV(filestring);
    for (int i = 0; i + 1 < ans.size(); i += 2) {
       dicts.push_back({ans.at(i), ans.at(i + 1)});
    }
    ui->listWidget->clear();
    ui->textBrowser_6->clear();
    for (auto &it : dicts) {
        ui->listWidget->addItem(it.first);
    }
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

QString word_name;

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    int cnt = ui->listWidget->row(item);
    word_name = item->text();
    ui->textBrowser_6->setPlainText(dicts[cnt].second);
}

void MainWindow::on_pushButton_8_clicked()
{
    if (dicts.size()) {
        if (!flag) {
            auto tmp = que1[0].first;
            if (favorites.contains(tmp.first)) {
                QMessageBox::information(nullptr, "提示", "该单词已被收藏");
            } else {
                favorites[tmp.first] = tmp.second;
            }
        } else {
            auto tmp = que2[0].first;
            if (favorites.contains(tmp.first)) {
                QMessageBox::information(nullptr, "提示", "该单词已被收藏");
            } else {
                favorites[tmp.first] = tmp.second;
            }
        }
        ui->pushButton_8->setEnabled(false);
    }
}

void MainWindow::on_pushButton_10_clicked()
{
    if (dicts.size()) {
        if (!flag) {
            auto tmp = que1[0].first;
            if (notes.contains(tmp.first)) {
                ui->textEdit_2->setPlainText(notes[tmp.first]);
            } else {
                ui->textEdit_2->setPlainText("该单词暂无笔记");
            }
        } else {
            auto tmp = que2[0].first;
            if (notes.contains(tmp.first)) {
                ui->textEdit_2->setPlainText(notes[tmp.first]);
            } else {
                ui->textEdit_2->setPlainText("该单词暂无笔记");
            }
        }
        ui->pushButton_9->setEnabled(true);
        ui->pushButton_10->setEnabled(false);
        ui->textEdit_2->setReadOnly(false);
        ui->textEdit_2->setVisible(true);
        ui->label_10->setVisible(true);
        ui->pushButton_9->setVisible(true);
        ui->pushButton_13->setVisible(true);
    }
}

void MainWindow::on_pushButton_9_clicked()
{
    if (dicts.size()) {
        QString txt = ui->textEdit_2->toPlainText();
        if (!flag) {
            auto tmp = que1[0].first;
            notes[tmp.first] = txt;
            ui->textEdit_2->setPlainText(notes[tmp.first]);
        } else {
            auto tmp = que2[0].first;
            notes[tmp.first] = txt;
            ui->textEdit_2->setPlainText(notes[tmp.first]);
        }
    }
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if (ui->tabWidget->currentIndex() == index) {
        ui->listWidget_2->clear();
        for (auto it = favorites.begin(); it != favorites.end(); ++ it) {
            ui->listWidget_2->addItem(it.key());
        }
        word_name = "";
    }
}

void MainWindow::on_listWidget_2_itemClicked(QListWidgetItem *item)
{
    QString tmp = item->text();
    word_name = tmp;
    ui->textEdit_3->setPlainText(favorites[tmp]);
    if (notes.contains(tmp)) {
        ui->textEdit_4->setPlainText(notes[tmp]);
    } else {
        ui->textEdit_4->setPlainText("该单词无笔记");
    }
}

void MainWindow::on_pushButton_11_clicked()
{
    QString mtxt = ui->textEdit_3->toPlainText();
    QString ntxt = ui->textEdit_4->toPlainText();
    QString key = word_name;
    qDebug() << key;
    notes[key] = ntxt;
    favorites[key] = mtxt;
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 2) {
        word_name = "";
        ui->listWidget_2->clear();
        for (auto it = favorites.begin(); it != favorites.end(); ++ it) {
            ui->listWidget_2->addItem(it.key());
        }
    }
}

void MainWindow::on_pushButton_12_clicked()
{
    if (word_name != "") {
        favorites.remove(word_name);
        ui->listWidget_2->clear();
        for (auto it = favorites.begin(); it != favorites.end(); ++ it) {
            ui->listWidget_2->addItem(it.key());
        }
    }
}

void MainWindow::on_pushButton_13_clicked()
{
    if (dicts.size()) {
        if (!flag) {
            auto tmp = que1[0].first;
            if (notes.contains(tmp.first)) {
                notes.remove(tmp.first);
                ui->textEdit_2->setPlainText("该单词暂无笔记");
            }
        } else {
            auto tmp = que2[0].first;
            if (notes.contains(tmp.first)) {
                notes.remove(tmp.first);
                ui->textEdit_2->setPlainText("该单词暂无笔记");
            }
        }
        ui->pushButton_9->setEnabled(true);
        ui->pushButton_10->setEnabled(false);
    }
}

void MainWindow::on_pushButton_14_clicked()
{
    if (word_name != "") {
        favorites[word_name] = ui->textBrowser_6->toPlainText();
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(db.open()==false) {
        QMessageBox::information(this,"数据库打开失败", db.lastError().text());
    }
    QSqlQuery query;
    QString txt = ui->textEdit->toPlainText();
    QString tmp = "SELECT mean FROM word where id == \"" + txt + "\"";
    if (!query.exec(tmp)) {
        qDebug() << "Error: Fail to select data. " << query.lastError();
    } else {
        ui->textBrowser_4->clear();
        while (query.next()) {
            ui->textBrowser_4->insertPlainText(query.value(0).toString());
        }
    }
    db.close();
}
