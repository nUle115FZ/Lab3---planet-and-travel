#include "addedgedialog.h"
#include "ui_addedgedialog.h"

AddEdgeDialog::AddEdgeDialog(StarGraph* graph, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEdgeDialog)
{
    ui->setupUi(this);
    setWindowTitle("Добавить гиперпространственный переход");
    
    //заполняем списки планет
    const auto& planets = graph->GetAllPlanets();
    for (const auto& pair : planets) {
        QString planetName = QString::fromStdString(pair.second);
        ui->fromComboBox->addItem(planetName);
        ui->toComboBox->addItem(planetName);
    }
    
    //устанавливаем значения по умолчанию
    ui->distanceSpinBox->setValue(100.0);
    ui->riskSpinBox->setValue(0.2);
}

AddEdgeDialog::~AddEdgeDialog()
{
    delete ui;
}

QString AddEdgeDialog::getFromPlanet() const
{
    return ui->fromComboBox->currentText();
}

QString AddEdgeDialog::getToPlanet() const
{
    return ui->toComboBox->currentText();
}

double AddEdgeDialog::getDistance() const
{
    return ui->distanceSpinBox->value();
}

double AddEdgeDialog::getRisk() const
{
    return ui->riskSpinBox->value();
}

