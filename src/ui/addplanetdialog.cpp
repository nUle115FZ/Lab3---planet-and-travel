#include "addplanetdialog.h"
#include "ui_addplanetdialog.h"
#include <QPushButton>

AddPlanetDialog::AddPlanetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPlanetDialog)
{
    ui->setupUi(this);
    setWindowTitle("Добавить планету");
    
    //отключаем кнопку OK если имя пустое
    QPushButton* okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (okButton) {
        okButton->setEnabled(false);
    }
    
    //подключаем валидацию при вводе текста
    connect(ui->planetNameEdit, &QLineEdit::textChanged, 
            this, &AddPlanetDialog::validateInput);
}

AddPlanetDialog::~AddPlanetDialog()
{
    delete ui;
}

QString AddPlanetDialog::getPlanetName() const
{
    return ui->planetNameEdit->text().trimmed();  //обрезаем пробелы
}

void AddPlanetDialog::validateInput()
{
    //получаем текст и убираем пробелы с краев
    QString name = ui->planetNameEdit->text().trimmed();
    
    //кнопка OK доступна только если есть непустое имя
    QPushButton* okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (okButton) {
        okButton->setEnabled(!name.isEmpty());
    }
    
    //подсказка пользователю
    if (name.isEmpty()) {
        ui->planetNameEdit->setStyleSheet("QLineEdit { border: 2px solid red; }");
    } else {
        ui->planetNameEdit->setStyleSheet("");
    }
}

