#include "cguilogin.h"
#include "ui_cguilogin.h"

//Code pour variable aléatoire
CGuiLogin::CGuiLogin(QWidget *parent, QString *codePIN) :
    QDialog(parent),
    ui(new Ui::CGuiLogin)
{
    ui->setupUi(this);
    QWidget::setWindowTitle("Authentification");

    _codePIN = codePIN;
    //Initialisation du générateur de nombres aléatoires
    QRandomGenerator generator;
    generator.seed(QDateTime::currentMSecsSinceEpoch());

    //Générer une permutation aléatoire des chiffres de 0 à 9
    QVector<int> numbers;
    for (int i = 0; i <= 9; i++) {
        numbers.append(i);
    }
    std::shuffle(numbers.begin(), numbers.end(), generator);

    //Affecter les chiffres aléatoires aux boutons
    ui->pb0->setText(QString::number(numbers.at(0)));
    ui->pb1->setText(QString::number(numbers[1]));
    ui->pb2->setText(QString::number(numbers[2]));
    ui->pb3->setText(QString::number(numbers[3]));
    ui->pb4->setText(QString::number(numbers[4]));
    ui->pb5->setText(QString::number(numbers[5]));
    ui->pb6->setText(QString::number(numbers[6]));
    ui->pb7->setText(QString::number(numbers[7]));
    ui->pb8->setText(QString::number(numbers[8]));
    ui->pb9->setText(QString::number(numbers[9]));

    ui->pbEffacer->setAutoDefault(false);

    //srand(time_t(NULL));// Initialisation du générateur
    ui->leAuthentification->setText("");
 //   _bascule = false;

    // Connexion des signaux et des slots pour les Boutons
    connect(ui->pb0, &QPushButton::released, this, &CGuiLogin::digit_pressed);
    connect(ui->pb1, &QPushButton::released, this, &CGuiLogin::digit_pressed);
    connect(ui->pb2, &QPushButton::released, this, &CGuiLogin::digit_pressed);
    connect(ui->pb3, &QPushButton::released, this, &CGuiLogin::digit_pressed);
    connect(ui->pb4, &QPushButton::released, this, &CGuiLogin::digit_pressed);
    connect(ui->pb5, &QPushButton::released, this, &CGuiLogin::digit_pressed);
    connect(ui->pb6, &QPushButton::released, this, &CGuiLogin::digit_pressed);
    connect(ui->pb7, &QPushButton::released, this, &CGuiLogin::digit_pressed);
    connect(ui->pb8, &QPushButton::released, this, &CGuiLogin::digit_pressed);
    connect(ui->pb9, &QPushButton::released, this, &CGuiLogin::digit_pressed);
}

CGuiLogin::~CGuiLogin()
{
    delete ui;
}// ~CLoginDialog(Destructeur)


void CGuiLogin::digit_pressed()
{
    QPushButton *button = static_cast<QPushButton*>(sender());
    ui->leAuthentification->setText(
                ui->leAuthentification->text() + button->text());
    // si 4 digits entrés
    if (ui->leAuthentification->text().size() == 4) {
        *_codePIN = ui->leAuthentification->text();
        hide();
    } // if
}

void CGuiLogin::closeEvent(QCloseEvent *event)
{
//    if(_bascule == false)
//    {
        event->accept();
//        exit(EXIT_FAILURE);
//    } else {
        event->accept();
//    }
}


void CGuiLogin::on_pbEffacer_clicked()
{
    ui->leAuthentification->clear();
}
