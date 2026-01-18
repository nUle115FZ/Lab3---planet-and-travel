#ifndef ADDPLANETDIALOG_H
#define ADDPLANETDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>

namespace Ui {
class AddPlanetDialog;
}

class AddPlanetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPlanetDialog(QWidget *parent = nullptr);
    ~AddPlanetDialog();
    
    QString getPlanetName() const;

private slots:
    void validateInput();  //проверка валидности ввода

private:
    Ui::AddPlanetDialog *ui;
};

#endif //aDDPLANETDIALOG_H

