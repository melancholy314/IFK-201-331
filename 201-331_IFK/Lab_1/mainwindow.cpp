#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "crypt_cont.h"



MainWindow::MainWindow(QWidget *parent):QMainWindow(parent) ,  ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
//

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::check_pin_code() {
    //qDebug() << pin_code_input->text();

    QByteArray pin_hash_code_current = QCryptographicHash::hash(QByteArray(pin_code_input->text().toUtf8()), QCryptographicHash::Md5).toHex(); // Хэшируем по Sha256

    if (pin_hash_code_current == pin_hash_code_orig) {
        this->key = pin_hash_code_current; //назначаем ключом хэш значение от пароля
        delete[] window_pin;

        this->show_main_window(); //запускаем основное окно
        this->show();
        return 0;

    }
    else {
        QMessageBox::critical(NULL,QObject::tr("Ошибка"),tr("Ошибка ввода пин-кода"));
        return 0;
    }
}

void MainWindow::show_pin_check_window() {

    window_pin = new QWidget(); //Создание виджета как окна для ввода пин кода
    window_pin->setGeometry(683,350,270,120);
    //window_pin->setStyleSheet("display: table-cell;");

    //window_pin->setBaseSize(250,100);
    //window_pin->setFixedSize(250,100);
    QVBoxLayout *layout_pin = new QVBoxLayout;


    QLabel *pin_code_label = new QLabel("Пин код"); //Лейбл для пинкода
    pin_code_input = new QLineEdit(); // Инпут для пинкода
    pin_code_input->setEchoMode(QLineEdit::Password);

    QPushButton *check_pin_btn = new QPushButton("Вход"); //Кнопка для проверки
    connect(check_pin_btn, SIGNAL(clicked()), this, SLOT(check_pin_code())); //При нажатии на кнопку отправляемся в функцию проверки


    //Добавляем в окно лейбл кнопку и инпут
    layout_pin->addWidget(pin_code_label);
    layout_pin->addWidget(pin_code_input);
    layout_pin->addWidget(check_pin_btn);
    window_pin->setLayout(layout_pin);

    window_pin->show();
}

void MainWindow::show_main_window() {

    encrypt_file.setFileName(encrypt_file_name);

    crypt_cont crypt;


    //Проверка на существование файла с полностью зашифрованными файлами
    if (!encrypt_file.exists()) {
        crypt.encrypt_file(encrypt_file_name, (unsigned char *)key.data()); //Шифрование файла с открытыми данными (Результат - созданный файл с зашифрованными данными)
    }

    //вызываем функцию расшифровки, отправляет имя файла и ключ(.data преобразует в char)
    QString encrypt_data = crypt.decrypt_file(encrypt_file_name, (unsigned char *)key.data());
    //приходит файл с расшифрованными названиями сайтов но с защифрованными связками логин+пароль

    doc = QJsonDocument::fromJson(QByteArray(encrypt_data.toUtf8()), &docError);

    json = doc.object();

    credentials = json.value("credentials");

    for (int i = 0; credentials[i].isObject(); i++) {
        QJsonObject obj_temp = credentials[i].toObject();
        QVector<QString> vec_temp = {obj_temp["site"].toString(), obj_temp["data"].toString()};
        data_vector.push_back(vec_temp);
    }

    //qDebug() << data_vector;

    //Отслеживание нажатие определенной кнопки
    mapper_log = new QSignalMapper(this);
    mapper_pas = new QSignalMapper(this);


    QObject::connect(mapper_log,SIGNAL(mappedInt(int)),this,SLOT(show_data_log(int))); //сигнал на кнопку логина
    QObject::connect(mapper_pas,SIGNAL(mappedInt(int)),this,SLOT(show_data_pass(int))); //сигнал на кнопку пароля

    //Добавление виджетов в общий лист виджетов
    for (int i = 0; i < data_vector.length(); i++) {

        QWidget *wig = create_widget(data_vector[i][0], i); // создание виджета
        list_of_widgets.push_back(wig);
    }


    for (int var = 0; var < list_of_widgets.length(); ++var) {
        ui->verticalLayout->addWidget(list_of_widgets[var]);
    }

}

//Функция создания виджетов
QWidget* MainWindow::create_widget(QString site, int id) {
    QHBoxLayout *layout = new QHBoxLayout;

    //Создание форм
    QLineEdit *site_name = new QLineEdit();
    QLineEdit *login = new QLineEdit();
    QLineEdit *password = new QLineEdit();

    site_name->setReadOnly(true);
    site_name->setText(site);
    //Логин
    login->setEchoMode(QLineEdit::Password);
    login->setReadOnly(true);
    login->setText("1234567890");

    //Пароль
    password->setEchoMode(QLineEdit::Password);
    password->setReadOnly(true);
    password->setText("1234567890");

    //Удаление рамок
    site_name->setStyleSheet("border: none;");
    login->setStyleSheet("border: none;");
    password->setStyleSheet("border: none;");

    //Кнопки копирования данных
    QPushButton *copy_login_btn = new QPushButton("copy");
    QPushButton *copy_pass_btn = new QPushButton("copy");


    //Размеры кнопок
    copy_login_btn->setFixedSize(50,20);
    copy_pass_btn->setFixedSize(50,20);

    //Сигналы на передачу данных от кнопок
    mapper_log->setMapping(copy_login_btn,id); //Вместе с кнопкой передаем номер цикла - это будет id
    connect(copy_login_btn,SIGNAL(clicked()),mapper_log,SLOT(map())); //слот на передачу при нажатии

    mapper_pas->setMapping(copy_pass_btn,id); //Вместе с кнопкой передаем номер цикла - это будет id
    connect(copy_pass_btn,SIGNAL(clicked()),mapper_pas,SLOT(map())); //слот на передачу при нажатии

    //добавление в layout
    layout->addWidget(site_name);
    layout->addWidget(login);
    layout->addWidget(copy_login_btn);
    layout->addWidget(password);
    layout->addWidget(copy_pass_btn);

    QWidget *wig = new QWidget(); // создание виджета

    //wig->installEventFilter(this);

    wig->setObjectName(QString(site)); // устанавливаем имя виджета для дальнейшего поиска
    wig->setLayout(layout); // добавление в виджет
    wig->setStyleSheet("background-color: #9aeda6;");
    //wig->setBaseSize(455,60);
    //wig->setFixedSize(455,60);


    return wig;

}

int MainWindow::show_data_log(int id) {

    QClipboard* pcb = QApplication::clipboard();
    //Копируем в буфер обмена
    pcb->setText(decrypt_pass_or_log("login",id), QClipboard::Clipboard);

    return 0;
}


int MainWindow::show_data_pass(int id) {

    QClipboard* pcb = QApplication::clipboard();
    //Копируем в буфер обмена
    pcb->setText(decrypt_pass_or_log("password",id), QClipboard::Clipboard);

    return 0;
}


QString MainWindow::decrypt_pass_or_log (QString log_or_pass, int id) {

    QString log_and_pass_chipr = data_vector[id][1]; //Забираем из вектора строчку с зашифрованным логином и паролем

    QByteArray log_and_pass_chipr_bit = QByteArray::fromBase64(log_and_pass_chipr.toUtf8()); //Переводим в байты

    crypt_cont decrypt_log_and_pass;

    unsigned char decrypt[256] = {0};
    int decrypt_len;


    //Расшифровываем строчку
    decrypt_len = decrypt_log_and_pass.decrypt((unsigned char*)log_and_pass_chipr_bit.data(),log_and_pass_chipr_bit.length(),(unsigned char *)key.data(),decrypt_log_and_pass.iv,decrypt);

    //Расшифрованные данные переводим в байты
    QByteArray decrypt_bit =  QByteArray::fromRawData((const char*)decrypt, decrypt_len);
    //qDebug() << decrypt_bit.data();

    //Создаем json документ
    QJsonDocument pass_log_doc = QJsonDocument::fromJson(decrypt_bit, &docError);
    QJsonObject pass_log_json = pass_log_doc.object();


    return (pass_log_json[log_or_pass].toString());
}


void MainWindow::on_search_line_textChanged(const QString &arg1)
{
    //Делаем все эллементы видимыми
    for (int var = 0; var < list_of_widgets.length(); var++) {
        list_of_widgets[var]->show();
    }

    //Скрываем эллементы, в которых не содержится введенной подстроки
    for (int var = 0; var < list_of_widgets.length(); var++) {
        if(!list_of_widgets[var]->objectName().contains(arg1)) {
            list_of_widgets[var]->hide();
        }
    }

    //qDebug() << arg1;
}
