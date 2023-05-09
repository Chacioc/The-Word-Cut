#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPair>
#include <QTextBrowser>
#include <QQueue>
#include <QListWidgetItem>
#include <QtSql/QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void init();
    QStringList readCSV(QString filestring);
    void saveCSV(QString filestring, QMap<QString, QString> dic);

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButton_8_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_9_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_listWidget_2_itemClicked(QListWidgetItem *item);

    void on_pushButton_11_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
    QVector<QPair<QString, QString> > dicts;
    QQueue<QPair<QPair<QString, QString>, int>> que1, que2;
    QMap<QString, int> mymap;
    QMap<QString, QString> notes;
    QMap<QString, QString> favorites;
    int le, *now, start, end;
    bool flag;
    QSqlDatabase db;
};
#endif // MAINWINDOW_H
