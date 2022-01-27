#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

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

private:
    Ui::MainWindow *ui;

    using randomEngine = std::default_random_engine;
    randomEngine rand_engine;
};
#endif  // MAINWINDOW_HPP
