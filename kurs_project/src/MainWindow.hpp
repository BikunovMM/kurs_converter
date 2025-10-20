#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QStringList>
#include <QStringListModel>
#include <QListView>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QString>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QDialog>
#include <QMetaObject>

#include <iostream>
#include <string>
#include <random>
#include <thread>
#include <chrono>
#include <future>
#include <functional>

#include "ServerRequester.hpp"
#include "SessionsManager.hpp"
#include "converter.h"

constexpr int WIN_W = 800;
constexpr int WIN_H = 450;
constexpr int RAND_FN_LEN = 36;
constexpr int ASCII_FST_NUM_POS = 48;
constexpr int ASCII_FST_LET_POS = 97;



/* Class, thats represents main application window
 *
 * Shortcuts:
 *  fst     - first
 *  num     - number
 *  let     - letter
 *  pos     - position
 *  win     - window
 *  w       - width
 *  h       - height
 *  len     - length
 *  fn      - filename
 *  gen     - generate
 *  upl     - upload
 *  downl   - download
 *  auth    - authorization (account)
 *  log     - login         (account)
 *  reg     - register      (account)
 *  prof    - profile       (account)
 *  sett    - settings      (account)
 *  btn     - button
 *  cbox    - check box
 *  def     - default       (filename)
 *  rand    - random        (filename)
 */
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();//virtual
public slots:
    void show_banner(char* data, long long data_len);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void open_page(unsigned int page);
    void upload_file();
    void download_file();
    void convert(const std::string& inpath, const std::string& outpath,
                 const converter::convertation_data& data);
    void gen_def_fn();
    void gen_rand_fn();

    void open_reg_win();
    void open_log_win();
    void open_prof_win();

    void reg_user(QLineEdit *log_ed, QLineEdit *pass_ed,
                  QLineEdit *email_ed, QLabel *log_err_lb,
                  QLabel *pass_err_lb, QLabel *email_err_lb, QDialog *diag);
    void log_user(QDialog *diag);
    void logout_user(QDialog *diag);

    void show_banner_out(char* data, long long data_len);

    void out_fmt_changed();
    void vcodec_cbox_changed();
    void acodec_cbox_changed();
    void icodec_cbox_changed();

    std::string upl_path;
    std::string downl_path;
    struct converter::convertation_data cdata;
    struct converter::format_data fdata;
    std::future<void> convert_file;

    //  Add splitter between page_btns_box and pages_widget
    QGridLayout     *main_grid;
    QVBoxLayout     *auth_box;
    QVBoxLayout     *page_btns_box;
    QStackedWidget  *pages_widget;

    QPushButton *log_btn;
    QPushButton *reg_btn;
    QPushButton *prof_btn;
    QPushButton *sett_btn;

    QPushButton *p0_btn;
    QPushButton *p1_btn;
    QPushButton *p2_btn;

    QWidget *page0;
    QWidget *page1;
    QWidget *page2;

    QGridLayout         *p0_grid;
    QPushButton         *p0_upl_btn;
    QPushButton         *p0_downl_btn;
    QStringList          p0_filenames_list;
    QStringListModel    *p0_filenames_list_model;
    QListView           *p0_filenames_view;
    QGridLayout         *p0_fn_grid;
    QLineEdit           *p0_fn_edit;
    QComboBox           *p0_fn_cbox;
    QHBoxLayout         *p0_fn_gen_box;
    QPushButton         *p0_fn_gen_def_btn;
    QPushButton         *p0_fn_gen_rand_btn;

    //  Add splitter between img and table_view!
    QGridLayout         *p1_grid;
    QPixmap              p1_img0_pixmap;
    QLabel              *p1_img0;
    QStandardItemModel  *p1_model;
    QTableView          *p1_table;


    QGridLayout         *p2_grid;
    QLabel              *p2_vcodec_lb;
    QComboBox           *p2_vcodec_cbox;
    QLabel                *p2_acodec_lb;
    QComboBox             *p2_acodec_cbox;
    QLabel                  *p2_icodec_lb;
    QComboBox               *p2_icodec_cbox;
    QLabel              *p2_pixfmt_lb;
    QComboBox           *p2_pixfmt_cbox;
    QLabel                *p2_smplfmt_lb;
    QComboBox             *p2_smplfmt_cbox;
};

#endif // MAINWINDOW_HPP
