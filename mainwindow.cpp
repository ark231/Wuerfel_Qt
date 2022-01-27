#include "mainwindow.hpp"

#include <QInputDialog>
#include <QListWidgetItem>

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    std::random_device seed_generator;
    this->rand_engine = randomEngine(seed_generator());
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_button_add_choice_clicked() {
    QString new_choice = QInputDialog::getText(nullptr, tr("enter choice"), tr("new choice:"), QLineEdit::Normal);
    auto new_choice_item = new QListWidgetItem(new_choice);
    new_choice_item->setFlags(Qt::ItemIsEditable /* | Qt::ItemIsSelectable*/ | Qt::ItemIsEnabled);
    this->ui->list_choices->addItem(new_choice_item);
}

void MainWindow::on_button_auswuefeln_clicked() {
    QListWidgetItem *choosen_item = nullptr;
    auto num_choices = this->ui->list_choices->count();
    if (num_choices == 0) {
        return;  // choosing nothing from nothing results nothing
    }
    std::uniform_int_distribution<> dist(0, num_choices - 1);
    auto choosen_idx = dist(this->rand_engine);
    choosen_item = this->ui->list_choices->item(choosen_idx);
    if (choosen_item) {
        this->ui->label_result->setText(choosen_item->text());
    } else {
        this->ui->label_result->setText("error: invalid number. please try again");
    }
}
void MainWindow::on_list_choices_itemClicked(QListWidgetItem *item) { this->ui->list_choices->editItem(item); }

void MainWindow::on_list_choices_itemChanged(QListWidgetItem *item) {
    if (item->text() == "") {
        delete item;
    }
}
