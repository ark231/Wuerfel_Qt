#include "mainwindow.hpp"

#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QStandardPaths>
#include <array>

#include "./ui_mainwindow.h"
namespace Wuerfel {
enum class Mode {
    CHOICES = 0,
    RANDGEN = 1,
};
enum class RandgenType {
    INT = 0,
    REAL = 1,
};
}  // namespace Wuerfel

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    std::random_device seed_generator;
    this->rand_engine = randomEngine(seed_generator());

    ui->stacked_modes->setCurrentIndex(static_cast<int>(Wuerfel::Mode::CHOICES));
    ui->stacked_types->setCurrentIndex(static_cast<int>(Wuerfel::RandgenType::INT));

    auto data_dirname = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir data_dir(data_dirname);
    if (not data_dir.exists()) {
        if (not data_dir.mkpath(data_dirname)) {
            QMessageBox::warning(nullptr, tr("mkdir failed"),
                                 tr("error: couldn't create path \"%1\".").arg(data_dirname));
            this->ui->menutemplate->setEnabled(false);
        }
    }
    this->template_dir = data_dir;
    this->template_path = data_dir.filePath("templates.json");
    this->template_file.setFileName(template_path);
    if (not template_file.exists()) {
        QFile::copy(":/res/text/templates.json", template_path);
    }
    if (template_file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadUser |
                                     QFileDevice::WriteUser)) {
        if (template_file.permissions() !=
            (QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadUser | QFileDevice::WriteUser)) {
            QMessageBox::warning(nullptr, tr("chmod failed"),
                                 tr("error: couldn't change permissions of \"%1\".").arg(template_path));
            this->ui->menutemplate->setEnabled(false);
        }
    }
    if (not template_file.open(QIODevice::ReadWrite)) {
        QMessageBox::warning(nullptr, tr("template file open error"),
                             tr("error: couldn't open template file %1").arg(template_path));
        this->ui->menutemplate->setEnabled(false);
    }
    template_file.seek(0);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_button_add_choice_clicked() {
    QString new_choice = QInputDialog::getText(nullptr, tr("enter choice"), tr("new choice:"), QLineEdit::Normal);
    auto new_choice_item = new QListWidgetItem(new_choice);
    new_choice_item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
    this->ui->list_choices->addItem(new_choice_item);
}

void MainWindow::on_button_auswuefeln_clicked() {
    switch (ui->stacked_modes->currentIndex()) {
        case static_cast<int>(Wuerfel::Mode::CHOICES): {
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
                QMessageBox::warning(nullptr, tr("invalid number error"),
                                     tr("error: invalid number. please try again"));
            }
        } break;
        case static_cast<int>(Wuerfel::Mode::RANDGEN): {
            switch (ui->stacked_types->currentIndex()) {
                case static_cast<int>(Wuerfel::RandgenType::INT): {
                    auto max = ui->spinbox_int_max->value();
                    auto min = ui->spinbox_int_min->value();
                    std::uniform_int_distribution<int> dist(min, max);
                    ui->label_result->setText(QString::number(dist(rand_engine)));
                } break;
                case static_cast<int>(Wuerfel::RandgenType::REAL): {
                    auto max = ui->spinbox_double_max->value();
                    auto min = ui->spinbox_double_min->value();
                    std::uniform_real_distribution<double> dist(min, max);
                    ui->label_result->setText(QString::number(dist(rand_engine)));
                } break;
            }
        } break;
    }
}
void MainWindow::on_list_choices_itemClicked(QListWidgetItem *item) { this->ui->list_choices->editItem(item); }

void MainWindow::on_list_choices_itemChanged(QListWidgetItem *item) {
    if (ui->actionrandgen_mode->isChecked()) {
        return;
    }
    if (item->text() == "") {
        delete item;
    }
}

void MainWindow::on_actionopen_triggered() {
    template_file.seek(0);  // 念の為
    auto template_json = QJsonDocument::fromJson(template_file.readAll());
    template_file.seek(0);
    auto template_obj = template_json.object();
    auto keys = template_obj.keys();
    bool confirmed;
    auto selected_key =
        QInputDialog::getItem(nullptr, tr("open template"), tr("choose template name"), keys, 0, false, &confirmed);
    if (not confirmed) {
        return;
    }
    auto values = template_obj[selected_key].toObject()["choices"];
    if (values.type() != QJsonValue::Type::Array) {
        QMessageBox::warning(nullptr, tr("template open error"),
                             tr("error: couldn't open template \"%1\". returned value wasn't array").arg(selected_key),
                             QMessageBox::Ok);
        return;
    }
    this->ui->list_choices->clear();
    for (const auto &value : values.toArray()) {
        auto new_choice_item = new QListWidgetItem(value.toString());
        new_choice_item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
        this->ui->list_choices->addItem(new_choice_item);
    }
}

void MainWindow::on_actionsave_triggered() {
    bool confirmed;
    QString template_title = QInputDialog::getText(nullptr, tr("save template"), tr("enter template title"),
                                                   QLineEdit::Normal, "", &confirmed);
    if (not confirmed) {
        return;
    }
    QJsonArray template_data;
    for (size_t i = 0; i < this->ui->list_choices->count(); i++) {
        template_data.append(this->ui->list_choices->item(i)->text());
    }
    auto template_json = QJsonDocument::fromJson(template_file.readAll());
    auto template_obj = template_json.object();
    if (template_obj.keys().contains(template_title)) {
        switch (QMessageBox::warning(nullptr, tr("template already exists"),
                                     tr("template with given name already exists. do you want to overwrite it?"),
                                     QMessageBox::Yes | QMessageBox::Abort)) {
            case QMessageBox::Yes:
                break;  // proceed to actually saving to file
            case QMessageBox::Abort:
            default:
                return;
        }
    }
    QJsonObject template_indiv_obj;
    template_indiv_obj.insert("choices", template_data);
    template_obj[template_title] = template_indiv_obj;
    QJsonDocument template_doc(template_obj);
    template_file.resize(0);
    template_file.seek(0);
    if (template_file.write(template_doc.toJson(QJsonDocument::Compact)) <= 0) {
        QMessageBox::warning(nullptr, tr("template saveerror"), tr("error: couldn't write template to file"));
        return;
    }
    template_file.flush();
}

void MainWindow::on_actionclear_all_triggered() { this->ui->list_choices->clear(); }

void MainWindow::on_actionremove_triggered() {
    auto template_obj = QJsonDocument::fromJson(template_file.readAll()).object();
    bool confirmed = false;
    auto selected_key = QInputDialog::getItem(nullptr, tr("delete template"), tr("choose template name to delete"),
                                              template_obj.keys(), 0, false, &confirmed);
    if (not confirmed) {
        return;
    }
    template_obj.remove(selected_key);
    QJsonDocument template_doc(template_obj);
    template_file.resize(0);
    template_file.seek(0);
    if (template_file.write(template_doc.toJson(QJsonDocument::Compact)) <= 0) {
        QMessageBox::warning(nullptr, tr("template saveerror"), tr("error: couldn't write template to file"));
        return;
    }
    template_file.flush();
    template_file.seek(0);
}
void MainWindow::on_actionrandgen_mode_triggered(bool checked) {
    ui->menutemplate->setEnabled(not checked);
    ui->actionclear_all->setEnabled(not checked);
    if (checked) {
        ui->stacked_modes->setCurrentIndex(static_cast<int>(Wuerfel::Mode::RANDGEN));
    } else {
        ui->stacked_modes->setCurrentIndex(static_cast<int>(Wuerfel::Mode::CHOICES));
    }
}
void MainWindow::on_combo_randgen_type_currentTextChanged(const QString &arg1) {
    if (arg1 == tr("int")) {
        ui->stacked_types->setCurrentIndex(static_cast<int>(Wuerfel::RandgenType::INT));
    } else if (arg1 == tr("real")) {
        ui->stacked_types->setCurrentIndex(static_cast<int>(Wuerfel::RandgenType::REAL));
    }
}
