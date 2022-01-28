#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QDir>
#include <QListWidgetItem>
#include <QMainWindow>
#include <random>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private slots:
    void on_button_add_choice_clicked();

    void on_button_auswuefeln_clicked();

    void on_list_choices_itemClicked(QListWidgetItem *item);

    void on_list_choices_itemChanged(QListWidgetItem *item);

    void on_actionopen_triggered();

    void on_actionsave_triggered();

    void on_actionclear_all_triggered();

    void on_actionremove_triggered();

    void on_actionrandgen_mode_triggered(bool checked);

    void on_combo_randgen_type_currentTextChanged(const QString &arg1);

   private:
    Ui::MainWindow *ui;

    using randomEngine = std::mt19937;
    randomEngine rand_engine;
    QDir template_dir;
    QString template_path;
    QFile template_file;
};
#endif  // MAINWINDOW_HPP
