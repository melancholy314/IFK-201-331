#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QJsonParseError>
#include <QFile>
#include <QMainWindow>
#include <QJsonArray>
#include "QJsonDocument"
#include "QJsonObject"
#include "QSignalMapper"
#include "QClipboard"
#include "QListWidgetItem"
#include "QVBoxLayout"
#include "QLineEdit"
#include "QLabel"
#include "QPushButton"
#include "QMessageBox"
#include "QList"
#include "QCryptographicHash"
#include <QSpacerItem>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    const QByteArray pin_hash_code_orig = "202cb962ac59075b964b07152d234b70";

    //окно для пин кода
    QWidget *window_pin;
    QByteArray key;
    QLineEdit *pin_code_input;

    //Шифрование:Расшифровка
    const QString encrypt_file_name = "C:/201-331_Ivanov/data.json";
    const QString open_file_name = "C:/201-331_Ivanov/open_data.json";
    QFile encrypt_file;
    QString decrypt_pass_or_log (QString log_or_pas, int id);


    //основное окно
    QSignalMapper *mapper_log;//отслеживание кнопки копирования логина
    QSignalMapper *mapper_pas;//отслеживание кнопки копирования пароля
    QJsonDocument doc;
    QJsonObject json;
    QJsonParseError docError;
    QJsonValue credentials;
    QList<QWidget *> list_of_widgets;
    QVector<QVector<QString>> data_vector;

    void show_pin_check_window();



private slots:
    void on_search_line_textChanged(const QString &arg1);
    void show_main_window();
    QWidget* create_widget(QString site, int id); //Создание виджета

    int check_pin_code();

    int show_data_log(int id);
    int show_data_pass(int id);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
