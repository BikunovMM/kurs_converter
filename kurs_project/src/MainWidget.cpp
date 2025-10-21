#include "MainWidget.hpp"
//#include "converter.h"

MainWidget::MainWidget()
    : QWidget()
{
    //  MAIN_LAYOUTS
    //
    main_grid = new QGridLayout(this);

    auth_box = new QVBoxLayout();
    main_grid->addLayout(auth_box, 0, 0, 1, 2,
                         Qt::AlignBottom | Qt::AlignRight);

    page_btns_box = new QVBoxLayout();
    main_grid->addLayout(page_btns_box, 1, 0);//If you want to stretch widget in grid. you have to addWidget with only the widget of course, and with its's rowPos and columnPos. Do not type colSpan and rowSpan!!!

    pages_widget = new QStackedWidget();
    pages_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pages_widget->setStyleSheet("QStackedWidget {background-color: lightgray;}");
    main_grid->addWidget(pages_widget, 1, 1);

    //  AUTH_BOX
    //

    log_btn = new QPushButton("log_in");
    connect(log_btn, &QPushButton::clicked, this, &MainWidget::open_log_win);
    auth_box->addWidget(log_btn);

    reg_btn = new QPushButton("register");
    connect(reg_btn, &QPushButton::clicked, this, &MainWidget::open_reg_win);
    auth_box->addWidget(reg_btn);

    prof_btn = new QPushButton("profile");
    connect(prof_btn, &QPushButton::clicked, this, &MainWidget::open_prof_win);
    auth_box->addWidget(prof_btn);

    sett_btn = new QPushButton("settings");
    auth_box->addWidget(sett_btn);

    //  PAGE_BTNS_BOX
    //

    p0_btn = new QPushButton("main");
    connect(p0_btn, &QPushButton::clicked, this, [=](){open_page(0);});
    page_btns_box->addWidget(p0_btn, 0,Qt::AlignTop | Qt::AlignLeft);

    p1_btn = new QPushButton("metadata");
    connect(p1_btn, &QPushButton::clicked, this, [=](){open_page(1);});
    page_btns_box->addWidget(p1_btn, 0,Qt::AlignTop | Qt::AlignLeft);

    p2_btn = new QPushButton("parameters");
    connect(p2_btn, &QPushButton::clicked, this, [=](){open_page(2);});
    page_btns_box->addWidget(p2_btn, 1,Qt::AlignTop | Qt::AlignLeft);

    //  PAGES
    //

    page0 = new QWidget();
    page0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pages_widget->addWidget(page0);

    page1 = new QWidget();
    page1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pages_widget->addWidget(page1);

    page2 = new QWidget();
    page2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pages_widget->addWidget(page2);

    //  PAGE0
    //

    p0_grid = new QGridLayout(page0);
    p0_grid->setRowStretch(1, 1);
    p0_grid->setColumnStretch(0, 1);
    p0_grid->setColumnStretch(1, 1);
    p0_grid->setRowMinimumHeight(0, 35);

    p0_upl_btn = new QPushButton("upload");
    p0_upl_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(p0_upl_btn, &QPushButton::clicked, this, &MainWidget::upload_file);
    p0_grid->addWidget(p0_upl_btn, 0, 0);

    p0_downl_btn = new QPushButton("download");
    p0_downl_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(p0_downl_btn, &QPushButton::clicked, this, &MainWidget::download_file);
    p0_grid->addWidget(p0_downl_btn, 0, 1);


    p0_filenames_list = {"..."};

    p0_filenames_list_model = new QStringListModel(p0_filenames_list);

    p0_filenames_view = new QListView();
    p0_filenames_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    p0_filenames_view->setModel(p0_filenames_list_model);
    p0_filenames_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    p0_grid->addWidget(p0_filenames_view, 1, 0);


    p0_fn_grid = new QGridLayout();
    p0_fn_grid->setColumnStretch(0, 1);
    p0_fn_grid->setRowStretch(2, 1);
    p0_grid->addLayout(p0_fn_grid, 1, 1);

    p0_fn_edit = new QLineEdit();
    p0_fn_grid->addWidget(p0_fn_edit, 0, 0);

    p0_fn_cbox = new QComboBox();
    p0_fn_cbox->setMinimumWidth(75);
    p0_fn_cbox->addItem("...");
    connect(p0_fn_cbox, &QComboBox::currentIndexChanged, this, &MainWidget::out_fmt_changed);
    p0_fn_grid->addWidget(p0_fn_cbox, 0, 1);

    p0_fn_gen_box = new QHBoxLayout();
    p0_fn_grid->addLayout(p0_fn_gen_box, 1, 0, 1, 2);

    p0_fn_gen_def_btn = new QPushButton("default");
    connect(p0_fn_gen_def_btn, &QPushButton::clicked, this, &MainWidget::gen_def_fn);
    p0_fn_gen_box->addWidget(p0_fn_gen_def_btn, 1);

    p0_fn_gen_rand_btn = new QPushButton("random");
    connect(p0_fn_gen_rand_btn, &QPushButton::clicked, this, &MainWidget::gen_rand_fn);
    p0_fn_gen_box->addWidget(p0_fn_gen_rand_btn, 1);

    //  PAGE1
    //

    p1_grid = new QGridLayout(page1);

    p1_img0 = new QLabel();
    p1_img0->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    p1_img0->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    p1_img0->setMinimumSize(300, 300);
    p1_img0->resize(300, 300);
    p1_grid->addWidget(p1_img0, 0, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    p1_model = new QStandardItemModel();

    p1_table = new QTableView();
    p1_table->setModel(p1_model);
    p1_table->setStatusTip("input file\'s metadata");
    p1_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    p1_table->setAlternatingRowColors(true);
    p1_table->setEditTriggers(QAbstractItemView::DoubleClicked);// QAbstractItemView::NoEditTriggers);
    p1_table->hide();
    p1_grid->addWidget(p1_table, 0, 1);

    //  PAGE2
    //

    p2_grid = new QGridLayout(page2);
    p2_grid->setContentsMargins(50, 12, 300, 12);
    p2_grid->setRowStretch(5, 1);
    p2_grid->setSpacing(25);

    //  vid codec
    p2_vcodec_lb = new QLabel("video codec:");
    p2_vcodec_lb->hide();
    p2_grid->addWidget(p2_vcodec_lb, 0, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    p2_vcodec_cbox = new QComboBox();
    p2_vcodec_cbox->hide();
    p2_vcodec_cbox->setMinimumWidth(175);
    p2_vcodec_cbox->insertItem(0, "...");
    connect(p2_vcodec_cbox, &QComboBox::currentIndexChanged, this, &MainWidget::vcodec_cbox_changed);
    p2_grid->addWidget(p2_vcodec_cbox, 0, 1, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    //  audio codec
    p2_acodec_lb = new QLabel("audio codec:");
    p2_acodec_lb->hide();
    p2_grid->addWidget(p2_acodec_lb, 1, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    p2_acodec_cbox = new QComboBox();
    p2_acodec_cbox->hide();
    p2_acodec_cbox->setMinimumWidth(175);
    p2_acodec_cbox->insertItem(0, "...");
    connect(p2_acodec_cbox, &QComboBox::currentIndexChanged, this, &MainWidget::acodec_cbox_changed);
    p2_grid->addWidget(p2_acodec_cbox, 1, 1, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    //  img codec
    p2_icodec_lb = new QLabel("image codec:");
    p2_icodec_lb->hide();
    p2_grid->addWidget(p2_icodec_lb, 2, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    p2_icodec_cbox = new QComboBox();
    p2_icodec_cbox->hide();
    p2_icodec_cbox->setMinimumWidth(175);
    p2_icodec_cbox->insertItem(0, "...");
    connect(p2_icodec_cbox, &QComboBox::currentIndexChanged, this, &MainWidget::icodec_cbox_changed);
    p2_grid->addWidget(p2_icodec_cbox, 2, 1, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    //  pix_fmt codec
    p2_pixfmt_lb = new QLabel("pixel format:");
    p2_pixfmt_lb->hide();
    p2_grid->addWidget(p2_pixfmt_lb, 3, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    p2_pixfmt_cbox = new QComboBox();
    p2_pixfmt_cbox->hide();
    p2_pixfmt_cbox->setMinimumWidth(175);
    p2_pixfmt_cbox->insertItem(0, "...");
    p2_grid->addWidget(p2_pixfmt_cbox, 3, 1, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    //  smpl_fmt codec
    p2_smplfmt_lb = new QLabel("sample format:");
    p2_smplfmt_lb->hide();
    p2_grid->addWidget(p2_smplfmt_lb, 4, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    p2_smplfmt_cbox = new QComboBox();
    p2_smplfmt_cbox->hide();
    p2_smplfmt_cbox->setMinimumWidth(175);
    p2_smplfmt_cbox->insertItem(0, "...");
    p2_grid->addWidget(p2_smplfmt_cbox, 4, 1, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    //  IS_USER_AUTH'ED
    //
    if (Sessions::Manager::contains_session()) {
        reg_btn->setHidden(true);
        log_btn->setHidden(true);
    }
    else {
        prof_btn->setHidden(true);
        sett_btn->setHidden(true);
    }

    //  WINDOW
    //

    setWindowTitle("converter");
    setMinimumSize(WIN_W, WIN_H);
    resize(WIN_W, WIN_H);
}

MainWidget::~MainWidget()
{
    delete(p2_smplfmt_cbox);
    delete(p2_smplfmt_lb);
    delete(p2_pixfmt_cbox);
    delete(p2_pixfmt_lb);
    delete(p2_icodec_cbox);
    delete(p2_icodec_lb);
    delete(p2_acodec_cbox);
    delete(p2_acodec_lb);
    delete(p2_vcodec_cbox);
    delete(p2_vcodec_lb);
    delete(p2_grid);

    delete(p1_table);
    delete(p1_model);
    delete(p1_img0);
    delete(p1_grid);

    delete(p0_fn_gen_rand_btn);
    delete(p0_fn_gen_def_btn);
    delete(p0_fn_gen_box);
    delete(p0_fn_cbox);
    delete(p0_fn_edit);
    delete(p0_fn_grid);

    delete(p0_filenames_view);
    delete(p0_filenames_list_model);
    delete(p0_downl_btn);
    delete(p0_upl_btn);
    delete(p0_grid);

    delete(page2);
    delete(page1);
    delete(page0);

    delete(p2_btn);
    delete(p1_btn);
    delete(p0_btn);

    delete(sett_btn);
    delete(prof_btn);
    delete(reg_btn);
    delete(log_btn);

    delete(pages_widget);
    delete(page_btns_box);
    delete(auth_box);
    delete(main_grid);
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void MainWidget::open_page(unsigned int page_index) {
    pages_widget->setCurrentIndex(page_index);
}

void MainWidget::upload_file()
{
    //  Open file dialog
    //

    QString uploaded_path = QFileDialog::getOpenFileName(this, tr("Select file :)"), "/",
        tr(converter::fdiag_upl_exts));

    if (uploaded_path.isEmpty()) {
        return;
    }

    upl_path = uploaded_path.toStdString();

    //  Set filename entry's text
    //

    std::size_t fn_index = upl_path.rfind('/') + 1;
    std::size_t ext_index = upl_path.rfind('.');

    std::size_t fn_len = ext_index - fn_index;

    p0_fn_edit->setText(upl_path.substr(fn_index, fn_len).c_str());

    //  Get supported extensions
    //
    std::string full_fn = upl_path.substr(fn_index);
    const char *full_filename = full_fn.c_str();

    fdata = converter::get_supported_extensions(full_filename);

    p0_fn_cbox->clear();


    for (unsigned int i = 0; fdata.extensions[i] != NULL; ++i) {
        p0_fn_cbox->addItem(fdata.extensions[i]);
    }

    //  Append filename to filename's list
    //

    QModelIndex fst_index = p0_filenames_list_model->index(0, 0);
    p0_filenames_list_model->setData(fst_index, upl_path.c_str());

    //  Enable Page1
    //

    if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_IMAGE) {
        if (!p1_img0->isVisible()) {
            p1_img0->setVisible(true);
        }
        p1_img0_pixmap = QPixmap(upl_path.c_str());

        p1_img0->setPixmap(p1_img0_pixmap.scaled(p1_img0->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
        p1_img0->setToolTip(upl_path.c_str());
    }
    else if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_VIDEO) {
        p1_img0->setVisible(false);
    }
    else if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_AUDIO) {
        p1_img0->setVisible(false);
    }

    //  Enable Page2
    //
    if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_VIDEO) {
        p2_vcodec_lb->show();
        p2_vcodec_cbox->show();

        p2_acodec_lb->show();
        p2_acodec_cbox->show();

        p2_pixfmt_lb->show();
        p2_pixfmt_cbox->show();

        p2_smplfmt_lb->show();
        p2_smplfmt_cbox->show();

        p2_icodec_lb->hide();
        p2_icodec_cbox->hide();
    }
    else if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_IMAGE) {
        p2_icodec_lb->show();
        p2_icodec_cbox->show();

        p2_pixfmt_lb->show();
        p2_pixfmt_cbox->show();

        p2_vcodec_lb->hide();
        p2_vcodec_cbox->hide();

        p2_acodec_lb->hide();
        p2_acodec_cbox->hide();

        p2_smplfmt_lb->hide();
        p2_smplfmt_cbox->hide();
    }
    else /* MEDIA_AUDIO */ {
        p2_acodec_lb->show();
        p2_acodec_cbox->show();

        p2_smplfmt_lb->show();
        p2_smplfmt_cbox->show();

        p2_icodec_lb->hide();
        p2_icodec_cbox->hide();

        p2_vcodec_lb->hide();
        p2_vcodec_cbox->hide();

        p2_pixfmt_lb->hide();
        p2_pixfmt_cbox->hide();
    }

    //
    //

    struct converter::file_data idata = converter::get_file_data(upl_path.c_str());

    if (idata.metadata.size() > 0) {
        QList<QStandardItem*> keys_list;
        QList<QStandardItem*> values_list;

        for (const std::pair<const std::string, const std::string>& pair : idata.metadata) {

            QStandardItem *head_item = new QStandardItem(pair.first.c_str());
            head_item->setToolTip(pair.first.c_str());
            keys_list.append(head_item);

            QStandardItem *val_item = new QStandardItem(pair.second.c_str());
            val_item->setToolTip(pair.second.c_str());
            values_list.append(val_item);
        }

        p1_model->clear();
        p1_model->appendColumn(keys_list);
        p1_model->appendColumn(values_list);

        p1_model->setHeaderData(0, Qt::Horizontal, "Keys");
        p1_model->setHeaderData(1, Qt::Horizontal, "Values");
        p1_table->horizontalHeader()->setStretchLastSection(true);

        if (p1_table->isHidden() && idata.metadata.size() > 0) {
            p1_table->show();
        }
    }
    else {
        p1_model->clear();
        if (!p1_table->isHidden()) {
            p1_table->hide();
        }
    }
}

void MainWidget::download_file()
{
    if (upl_path.empty()) {
        return;
    }

    //  Get outfile name
    //

    std::string downl_fn;
    if (p0_fn_edit->text().isEmpty()) {

        int fn_index = upl_path.rfind('/') + 1;
        int ext_index = upl_path.rfind('.') + 1;
        int downl_fn_len = ext_index - fn_index;

        downl_fn = upl_path.substr(fn_index, downl_fn_len);
    }
    else {
        downl_fn = (p0_fn_edit->text() + '.' + p0_fn_cbox->currentText()).toStdString();
    }

    //  Get output path with dialog
    //

    std::string download_path;

    if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_IMAGE) {
        download_path = QFileDialog::getSaveFileName(this, tr("Select path"),
                                                              ("/" + downl_fn).c_str(), tr(converter::fdiag_img_exts)
                                                              ).toStdString();
    }
    else if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_VIDEO) {
        download_path = QFileDialog::getSaveFileName(this, tr("Select path"),
                                                              ("/" + downl_fn).c_str(), tr(converter::fdiag_video_exts)
                                                              ).toStdString();
    }
    else {/* fdata.media_type == converter::MEDIA_TYPE::MEDIA_AUDIO */
        download_path = QFileDialog::getSaveFileName(this, tr("Select path"),
                                                              ("/" + downl_fn).c_str(), tr(converter::fdiag_audio_exts)
                                                              ).toStdString();
    }

    if (download_path.empty()) {
        return;
    }

    downl_path = download_path;

    //  Get chosen codecs and pixfmt or/and smplfmt
    //

    if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_VIDEO) {
        switch (p0_fn_cbox->currentIndex()) {
        case 0: /*mp4*/
            cdata.max_width = 0;

            switch (p2_vcodec_cbox->currentIndex()) {
            case 0:/*H264*/
                cdata.video_codec = AV_CODEC_ID_H264;
                cdata.pixfmt = AV_PIX_FMT_YUV420P;
                break;
            case 1:/*H265/HEVC*/
                cdata.video_codec = AV_CODEC_ID_HEVC;
                cdata.pixfmt = AV_PIX_FMT_YUV420P;
                break;
            case 2:/*MPEG4*/
                cdata.video_codec = AV_CODEC_ID_MPEG4;
                cdata.pixfmt = AV_PIX_FMT_YUV420P;
                break;
            }

            if (p2_acodec_cbox->currentIndex() == 0) {
                cdata.audio_codec = AV_CODEC_ID_AAC;
                cdata.sample_rate = 48000;
                cdata.bit_rate = 96000;

                if (p2_smplfmt_cbox->currentIndex() == 0) {
                    cdata.smplfmt = AV_SAMPLE_FMT_FLTP;
                }
                else {
                    cdata.smplfmt = AV_SAMPLE_FMT_S16;
                }
            }
            else {
                cdata.audio_codec = AV_CODEC_ID_MP3;
                cdata.smplfmt = AV_SAMPLE_FMT_S16;
                cdata.sample_rate = 48000;
                cdata.bit_rate = 96000;
            }
            break;
        case 1: /*mov*/
            cdata.max_width = 0;

            switch (p2_vcodec_cbox->currentIndex()) {
            case 0:/*H264*/
                cdata.video_codec = AV_CODEC_ID_H264;
                cdata.pixfmt = AV_PIX_FMT_YUV420P;
                break;
            case 1:/*H265/HEVC*/
                cdata.video_codec = AV_CODEC_ID_HEVC;
                cdata.pixfmt = AV_PIX_FMT_YUV420P;
                break;
            case 2:/*MPEG4*/
                cdata.video_codec = AV_CODEC_ID_MPEG4;
                cdata.pixfmt = AV_PIX_FMT_YUV420P;
                break;
            }

            if (p2_acodec_cbox->currentIndex() == 0) {
                cdata.audio_codec = AV_CODEC_ID_AAC;
                cdata.sample_rate = 48000;
                cdata.bit_rate = 96000;

                if (p2_smplfmt_cbox->currentIndex() == 0) {
                    cdata.smplfmt = AV_SAMPLE_FMT_FLTP;
                }
                else {
                    cdata.smplfmt = AV_SAMPLE_FMT_S16;
                }
            }
            else {
                cdata.audio_codec = AV_CODEC_ID_MP3;
                cdata.smplfmt = AV_SAMPLE_FMT_S16;
            }
            break;
        case 2: /*mkv*/
            cdata.max_width = 0;
            switch (p2_vcodec_cbox->currentIndex()) {
            case 0:/*H264*/
                cdata.video_codec = AV_CODEC_ID_H264;
                cdata.pixfmt = AV_PIX_FMT_YUV420P;
                break;
            case 1:/*H265/HEVC*/
                cdata.video_codec = AV_CODEC_ID_HEVC;
                cdata.pixfmt = AV_PIX_FMT_YUV420P;
                break;
            case 2:/*MPEG4*/
                cdata.video_codec = AV_CODEC_ID_MPEG4;
                cdata.pixfmt = AV_PIX_FMT_YUV420P;
                break;
            }

            switch (p2_acodec_cbox->currentIndex()) {
            case 0:/*AAC*/
                cdata.audio_codec = AV_CODEC_ID_AAC;
                cdata.sample_rate = 48000;
                cdata.bit_rate = 96000;

                if (p2_acodec_cbox->currentIndex() == 0) {
                    cdata.smplfmt = AV_SAMPLE_FMT_FLTP;
                }
                else {
                    cdata.smplfmt = AV_SAMPLE_FMT_S16;
                }
                break;
            case 1:/*MP3*/
                cdata.audio_codec = AV_CODEC_ID_MP3;
                cdata.smplfmt = AV_SAMPLE_FMT_S16;
                cdata.sample_rate = 48000;
                cdata.bit_rate = 96000;
                break;
            case 2:/*OPUS*/
                cdata.audio_codec = AV_CODEC_ID_OPUS;
                cdata.sample_rate = 48000;
                cdata.bit_rate = 96000;

                if (p2_acodec_cbox->currentIndex() == 0) {
                    cdata.smplfmt = AV_SAMPLE_FMT_FLTP;
                }
                else {
                    cdata.smplfmt = AV_SAMPLE_FMT_S16;
                }
                break;
            case 3:/*FLAC*/
                cdata.audio_codec = AV_CODEC_ID_FLAC;
                cdata.sample_rate = 48000;
                cdata.bit_rate = 0;

                if (p2_acodec_cbox->currentIndex() == 0) {
                    cdata.smplfmt = AV_SAMPLE_FMT_S16;
                }
                else {
                    cdata.smplfmt = AV_SAMPLE_FMT_S32;
                }
                break;
            }

            break;
        case 3: /*webm*/
            cdata.max_width = 0;

            if (p2_vcodec_cbox->currentIndex() == 0) {/*VP9*/
                cdata.video_codec = AV_CODEC_ID_VP9;
            }
            else if (p2_vcodec_cbox->currentIndex() == 1) {/*VP8*/
                cdata.video_codec = AV_CODEC_ID_VP8;
            }
            else {/*AV1*/
                cdata.video_codec = AV_CODEC_ID_AV1;
            }

            if (p2_acodec_cbox->currentIndex() == 0) {/*OPUS*/
                cdata.audio_codec = AV_CODEC_ID_OPUS;
                cdata.sample_rate = 48000;
                cdata.bit_rate = 96000;

                if (p2_smplfmt_cbox->currentIndex() == 0) {/*FLTP*/
                    cdata.smplfmt = AV_SAMPLE_FMT_FLTP;
                }
                else {/*S16*/
                    cdata.smplfmt = AV_SAMPLE_FMT_S16;
                }
            }
            break;
        }


    }
    else if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_IMAGE) {
        switch (p0_fn_cbox->currentIndex()) {
            case 0: /*PNG*/
                cdata.audio_codec = AV_CODEC_ID_NONE;
                cdata.video_codec = AV_CODEC_ID_PNG;
                cdata.smplfmt = AV_SAMPLE_FMT_NONE;
                cdata.max_width = 0;
                cdata.sample_rate = 0;
                cdata.bit_rate = 0;

                switch (p2_pixfmt_cbox->currentIndex()) {
                case 0:/*RGBA*/
                    cdata.pixfmt = AV_PIX_FMT_RGBA;
                    break;
                case 1:/*RGB24*/
                    cdata.pixfmt = AV_PIX_FMT_RGB24;
                    break;
                case 2:/*GRAY8*/
                    cdata.pixfmt = AV_PIX_FMT_GRAY8;
                    break;
                }

                break;
            case 1: /*JPG*/
            case 2: /*JPEG*/
                cdata.audio_codec = AV_CODEC_ID_NONE;
                cdata.video_codec = AV_CODEC_ID_MJPEG;
                cdata.smplfmt = AV_SAMPLE_FMT_NONE;
                cdata.max_width = 0;
                cdata.sample_rate = 0;
                cdata.bit_rate = -1;

                switch (p2_pixfmt_cbox->currentIndex()) {
                case 0:/*YUV420P*/
                    cdata.pixfmt = AV_PIX_FMT_YUVJ420P;
                    break;
                }
                break;
            case 3: /*ICO*/
                cdata.audio_codec = AV_CODEC_ID_NONE;
                cdata.smplfmt = AV_SAMPLE_FMT_NONE;
                cdata.max_width = 256;
                cdata.sample_rate = 0;
                cdata.bit_rate = 0;

                if (p2_icodec_cbox->currentIndex() == 0) {
                    cdata.video_codec = AV_CODEC_ID_PNG;

                    switch (p2_pixfmt_cbox->currentIndex()) {
                    case 0:/*RGBA*/
                        cdata.pixfmt = AV_PIX_FMT_RGBA;
                        break;
                    }
                }
                else {
                    cdata.video_codec = AV_CODEC_ID_BMP;

                    if (p2_pixfmt_cbox->currentIndex() == 0) {
                        cdata.pixfmt = AV_PIX_FMT_BGRA;
                    }
                }
                break;
            case 4: /*bmp*/
                cdata.audio_codec = AV_CODEC_ID_NONE;
                cdata.video_codec = AV_CODEC_ID_BMP;
                cdata.smplfmt = AV_SAMPLE_FMT_NONE;
                cdata.max_width = 0;
                cdata.sample_rate = 0;
                cdata.bit_rate = 0;

                switch (p2_pixfmt_cbox->currentIndex()) {
                case 0:/*BGRA*/
                    cdata.pixfmt = AV_PIX_FMT_BGRA;
                    break;
                case 1:/*BGR24*/
                    cdata.pixfmt = AV_PIX_FMT_BGR24;
                    break;
                case 2:/*BGR8*/
                    cdata.pixfmt = AV_PIX_FMT_BGR8;
                    break;
                case 3:/*GRAY8*/
                    cdata.pixfmt = AV_PIX_FMT_GRAY8;
                    break;
                case 4:/*MONOBLACK*/
                    cdata.pixfmt = AV_PIX_FMT_MONOBLACK;
                    break;
                }
                break;
        }
    }
    else {/*MEDIA_AUDIO*/
        switch (p0_fn_cbox->currentIndex()) {
        case 0: /*MP3*/
            cdata.audio_codec = AV_CODEC_ID_MP3;
            cdata.video_codec = AV_CODEC_ID_NONE;            
            cdata.pixfmt = AV_PIX_FMT_NONE;
            cdata.smplfmt = AV_SAMPLE_FMT_S16;
            cdata.max_width = 0;
            cdata.sample_rate = 48000;
            cdata.bit_rate = 96000;
            break;
        case 1: /*OPUS*/
            cdata.audio_codec = AV_CODEC_ID_OPUS;
            cdata.video_codec = AV_CODEC_ID_NONE;            
            cdata.pixfmt = AV_PIX_FMT_NONE;
            cdata.max_width = 0;
            cdata.sample_rate = 48000;
            cdata.bit_rate = 96000;

            if (p2_acodec_cbox->currentIndex() == 0) {
                cdata.smplfmt = AV_SAMPLE_FMT_FLTP;
            }
            else {
                cdata.smplfmt = AV_SAMPLE_FMT_S16;
            }
            break;
        case 2: /*WAV*/
            cdata.audio_codec = AV_CODEC_ID_PCM_S16LE;
            cdata.video_codec = AV_CODEC_ID_NONE;            
            cdata.pixfmt = AV_PIX_FMT_NONE;
            cdata.max_width = 0;
            cdata.sample_rate = 48000;
            cdata.bit_rate = 0;

            if (p2_acodec_cbox->currentIndex() == 0) {
                cdata.smplfmt = AV_SAMPLE_FMT_S16;
            }
            else {
                cdata.smplfmt = AV_SAMPLE_FMT_FLT;
            }
            break;
        case 3: /*AAC*/
            cdata.audio_codec = AV_CODEC_ID_AAC;
            cdata.video_codec = AV_CODEC_ID_NONE;
            cdata.pixfmt = AV_PIX_FMT_NONE;
            cdata.max_width = 0;
            cdata.sample_rate = 48000;
            cdata.bit_rate = 96000;

            if (p2_acodec_cbox->currentIndex() == 0) {
                cdata.smplfmt = AV_SAMPLE_FMT_FLTP;
            }
            else {
                cdata.smplfmt = AV_SAMPLE_FMT_S16;
            }
            break;
        case 4: /*FLAC*/
            cdata.audio_codec = AV_CODEC_ID_FLAC;
            cdata.video_codec = AV_CODEC_ID_NONE;
            cdata.pixfmt = AV_PIX_FMT_NONE;
            cdata.max_width = 0;
            cdata.sample_rate = 48000;
            cdata.bit_rate = 0;

            if (p2_acodec_cbox->currentIndex() == 0) {
                cdata.smplfmt = AV_SAMPLE_FMT_S16;
            }
            else {
                cdata.smplfmt = AV_SAMPLE_FMT_S32;
            }
            break;
        }
    }

    //  Convertation
    //

    convert_file = std::async(std::launch::async,
               [&] {MainWidget::convert(upl_path, downl_path, cdata);});
}

void MainWidget::convert(const std::string& inpath, const std::string& outpath,
                         const converter::convertation_data& data)
{
    int res = 0;

    res = converter::convert(inpath.c_str(), outpath.c_str(), &data,
        [this](char *data, long long data_len){this->show_banner_out(data, data_len);});
    if (res < 0) {
        fprintf(stderr, "Failed to convert file.\n");
    }
}

void MainWidget::gen_def_fn()
{
    size_t fn_index = upl_path.rfind('/') + 1;
    size_t ext_index = upl_path.rfind('.');

    size_t fn_len = ext_index - fn_index;

    p0_fn_edit->setText(upl_path.substr(fn_index, fn_len).c_str());
}

void MainWidget::gen_rand_fn()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution dst(0, 15);

    char fn_str[37];

    for (int i = 0; i < RAND_FN_LEN; ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            fn_str[i] = '-';
            continue;
        }

        unsigned char rand_val = dst(mt);
        if (rand_val > 9) {
            rand_val = ASCII_FST_LET_POS + rand_val - 10;
        }
        else {
            rand_val = ASCII_FST_NUM_POS + rand_val;
        }
        fn_str[i] = rand_val;
    }
    fn_str[36] = '\0';

    p0_fn_edit->setText(fn_str);
}

void MainWidget::open_reg_win()
{
    QDialog     *reg_win_diag = nullptr;
    QGridLayout *main_grid    = nullptr;
    QVBoxLayout *main_vbox    = nullptr;
    QLabel      *log_lb       = nullptr;
    QLineEdit   *log_ed       = nullptr;
    QLabel      *log_err_lb   = nullptr;
    QLabel      *pass_lb      = nullptr;
    QLineEdit   *pass_ed      = nullptr;
    QLabel      *pass_err_lb  = nullptr;
    QLabel      *email_lb     = nullptr;
    QLineEdit   *email_ed     = nullptr;
    QLabel      *email_err_lb = nullptr;
    QPushButton *reg_btn      = nullptr;

    reg_win_diag = new QDialog(this);
    reg_win_diag->setFixedSize(300, 280);
    reg_win_diag->setWindowTitle("register_window");

    main_grid = new QGridLayout();
    reg_win_diag->setLayout(main_grid);

    main_vbox = new QVBoxLayout();
    main_grid->setContentsMargins(48, 12, 48, 0);
    main_grid->addLayout(main_vbox, 0, 0, 1, 1, Qt::AlignCenter);

    //  Login
    //
    log_lb = new QLabel("*login:");
    main_vbox->addWidget(log_lb);

    log_ed = new QLineEdit();
    main_vbox->addWidget(log_ed);

    log_err_lb = new QLabel("");
    log_err_lb->setObjectName("error-label");
    main_vbox->addWidget(log_err_lb);

    //  Password
    //
    pass_lb = new QLabel("*password:");
    main_vbox->addWidget(pass_lb);

    pass_ed = new QLineEdit();
    main_vbox->addWidget(pass_ed);

    pass_err_lb = new QLabel("");
    pass_err_lb->setObjectName("error-label");
    main_vbox->addWidget(pass_err_lb);

    //  Email
    //
    email_lb = new QLabel("email:");
    main_vbox->addWidget(email_lb);

    email_ed = new QLineEdit();
    main_vbox->addWidget(email_ed);

    email_err_lb = new QLabel("");
    email_err_lb->setObjectName("error-label");
    main_vbox->addWidget(email_err_lb);

    //  REG_BTN
    //
    reg_btn = new QPushButton("register");
    reg_btn->setMaximumSize(75, 25);
    connect(reg_btn, &QPushButton::clicked, this,
        [log_ed, pass_ed, email_ed, log_err_lb,
         pass_err_lb, email_err_lb, reg_win_diag, this]() mutable
        {
            this->reg_user(log_ed, pass_ed, email_ed,
                log_err_lb, pass_err_lb, email_err_lb,
                reg_win_diag);
        }
    );
    main_vbox->addWidget(reg_btn, 1, Qt::AlignCenter);


    reg_win_diag->exec();

    delete(email_err_lb);
    delete(email_ed);
    delete(email_lb);

    delete(pass_err_lb);
    delete(pass_ed);
    delete(pass_lb);

    delete(log_err_lb);
    delete(log_ed);
    delete(log_lb);

    delete(reg_btn);
    delete(main_vbox);
    delete(main_grid);
    delete(reg_win_diag);
}

void MainWidget::open_log_win() {}

void MainWidget::open_prof_win()
{
    QDialog     *prof_win_diag = nullptr;
    QGridLayout *main_grid = nullptr;
    QPushButton *logout_btn = nullptr;

    prof_win_diag = new QDialog(this);
    prof_win_diag->setFixedSize(300, 280);
    prof_win_diag->setWindowTitle("profile");

    main_grid = new QGridLayout();
    prof_win_diag->setLayout(main_grid);

    logout_btn = new QPushButton("logout");
    connect(logout_btn, &QPushButton::clicked, this, [prof_win_diag, this](){logout_user(prof_win_diag);});
    main_grid->addWidget(logout_btn, 0, 0, Qt::AlignCenter);

    prof_win_diag->exec();

    delete(logout_btn);
    delete(main_grid);
    delete(prof_win_diag);
}

void MainWidget::reg_user(QLineEdit *log_ed, QLineEdit *pass_ed,
                          QLineEdit *email_ed, QLabel *log_err_lb,
                          QLabel *pass_err_lb, QLabel *email_err_lb,
                          QDialog *diag)
{
    const QByteArray login_byte_arr = log_ed->text().toUtf8();
    const QByteArray password_byte_arr = pass_ed->text().toUtf8();
    const QByteArray email_byte_arr = email_ed->text().toUtf8();
    const char *login = login_byte_arr.data();
    const char *password = password_byte_arr.data();
    const char *email = email_byte_arr.data();
    int res = 0;

    if (!login || std::strlen(login) == 0) {
        log_err_lb->setText("login can\'t be empty!");
        res = 1;
    }
    else if (login[0] == ' ') {
        log_err_lb->setText("login can\'t start with a "
                            "whitespace");
        res = 1;
    }
    else if (std::strlen(login) < 3) {
        log_err_lb->setText("login length must be > 2");
        res = 1;
    }
    else if (std::strlen(login) > 20) {
        log_err_lb->setText("login length must be < 21");
        res = 1;
    }
    else {
        log_err_lb->setText("");
    }


    if (!password || std::strlen(password) == 0) {
        pass_err_lb->setText("password can\'t be empty!");
        res = 1;
    }
    else if (password[0] == ' ') {
        pass_err_lb->setText("password can\'t start with a "
                             "whitespace");
        res = 1;
    }
    else if (std::strlen(password) < 3) {
        pass_err_lb->setText("password length must be > 2");
        res = 1;
    }
    else if (std::strlen(password) > 20) {
        pass_err_lb->setText("password length must be < 21");
        res = 1;
    }
    else {
        pass_err_lb->setText("");
    }

    if (email && std::strlen(email) != 0) {
        if (email[0] == ' ') {
            email_err_lb->setText("email can\'t start with a "
                                 "whitespace");
            return;
        }
        else if (std::strlen(email) < 9) {
            email_err_lb->setText("email length must be > 9");
            return;
        }
        else if (std::strlen(email) > 40) {
            email_err_lb->setText("email length must be < 41");
            return;
        }

        const char first_chr = email[0];
        if (!(first_chr >= 65 && first_chr <= 90)
            && !(first_chr >= 97 && first_chr <= 122)
            && !(first_chr >= 48 && first_chr <= 57)) {
            email_err_lb->setText("email has to start with an "
                                  "alphabet or number");
            return;
        }

        char *doggy_chr = std::strchr(email, '@');
        if (!doggy_chr) {
            email_err_lb->setText("email has to contain \"@\"");
            return;
        }

        char *dot_chr = std::strrchr(email, '.');
        if (!dot_chr || doggy_chr + 1 >= dot_chr) {
            email_err_lb->setText("email has to contain \".\"");
            return;
        }
        else if (doggy_chr + 1 >= dot_chr) {
            email_err_lb->setText("email has to contain \"@\""
                                  " after the \".\"!");
            return;
        }

        if (strlen(email) - 1 - (dot_chr - email) == 0) {
            email_err_lb->setText("email can\'t has \".\" at the end!");
            return;
        }

        email_err_lb->setText("");
    }
    else {
        email_err_lb->setText("");
    }

    if (res == 1) {
        return;
    }

    //  Requesting server's database
    //

    Server::Requester::register_user(login, password,
                                     email, log_err_lb,
                                     reg_btn, log_btn,
                                     prof_btn, sett_btn,
                                     diag);
}

void MainWidget::logout_user(QDialog *diag)
{
    reg_btn->setHidden(false);
    log_btn->setHidden(false);
    prof_btn->setHidden(true);
    sett_btn->setHidden(true);

    diag->done(0);

    Sessions::Manager::delete_session();
}

void MainWidget::show_banner(char* data, long long data_len)
{
    QDialog     *banner_win_diag = nullptr;
    QGridLayout *banner_grid     = nullptr;
    QPixmap      banner_pixmp;
    QLabel      *banner_lb       = nullptr;

    if (!data) {
        return;
    }

    banner_win_diag = new QDialog(this);
    banner_win_diag->setWindowTitle("banner2006");
    banner_win_diag->setFixedSize(640, 360);

    banner_grid = new QGridLayout();
    banner_win_diag->setLayout(banner_grid);

    banner_lb = new QLabel();
    banner_lb->setScaledContents(true);
    banner_grid->addWidget(banner_lb, 0, 0);

    banner_pixmp.loadFromData(
        reinterpret_cast<const uchar*>(data),
        data_len);
    free(data);

    banner_lb->setPixmap(banner_pixmp.scaled(banner_lb->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
    banner_win_diag->exec();

    delete(banner_lb);
    delete(banner_grid);
    delete(banner_win_diag);
}

void MainWidget::show_banner_out(char* data, long long data_len)
{
    QMetaObject::invokeMethod(this, "show_banner",
                              Qt::QueuedConnection,
                              Q_ARG(char*, data),
                              Q_ARG(long long, data_len));
}

void MainWidget::out_fmt_changed()
{
    if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_IMAGE) {
        p2_icodec_cbox->clear();
        p2_pixfmt_cbox->clear();

        switch (p0_fn_cbox->currentIndex()) {
            case 0: /*PNG*/
                p2_icodec_cbox->addItem("AV_CODEC_ID_PNG");
                break;
            case 1: /*JPG*/
            case 2: /*JPEG*/
                p2_icodec_cbox->addItem("AV_CODEC_ID_MJPEG");
                break;
            case 3: /*ICO*/
                p2_icodec_cbox->addItem("AV_CODEC_ID_PNG");
                p2_icodec_cbox->addItem("AV_CODEC_ID_BMP");
                break;
            case 4: /*bmp*/
                p2_icodec_cbox->addItem("AV_CODEC_ID_BMP");
                break;
        }
    }
    else if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_AUDIO) {
        p2_acodec_cbox->clear();
        p2_smplfmt_cbox->clear();

        switch (p0_fn_cbox->currentIndex()) {
        case 0: /*MP3*/
            p2_acodec_cbox->addItem("AV_CODEC_ID_MP3");
            break;
        case 1: /*OPUS*/
            p2_acodec_cbox->addItem("AV_CODEC_ID_OPUS");
            break;
        case 2: /*WAV*/
            p2_acodec_cbox->addItem("AV_CODEC_ID_PCM_S16LE");
            break;
        case 3: /*AAC*/
            p2_acodec_cbox->addItem("AV_CODEC_ID_AAC");
            break;
        case 4: /*FLAC*/
            p2_acodec_cbox->addItem("AV_CODEC_ID_FLAC");
            break;
        }
    }
    else /* MEDIA_VIDEO */ {
        p2_vcodec_cbox->clear();
        p2_pixfmt_cbox->clear();
        p2_acodec_cbox->clear();
        p2_smplfmt_cbox->clear();

        switch (p0_fn_cbox->currentIndex()) {
        case 0: /*mp4*/
            p2_vcodec_cbox->addItem("AV_CODEC_ID_H264");
            p2_vcodec_cbox->addItem("AV_CODEC_ID_HEVC");
            p2_vcodec_cbox->addItem("AV_CODEC_ID_MPEG4");

            p2_acodec_cbox->addItem("AV_CODEC_ID_AAC");
            p2_acodec_cbox->addItem("AV_CODEC_ID_MP3");
            break;
        case 1: /*mov*/
            p2_vcodec_cbox->addItem("AV_CODEC_ID_H264");
            p2_vcodec_cbox->addItem("AV_CODEC_ID_HEVC");
            p2_vcodec_cbox->addItem("AV_CODEC_ID_MPEG4");

            p2_acodec_cbox->addItem("AV_CODEC_ID_AAC");
            p2_acodec_cbox->addItem("AV_CODEC_ID_MP3");
            break;
        case 2: /*mkv*/
            p2_vcodec_cbox->addItem("AV_CODEC_ID_H264");
            p2_vcodec_cbox->addItem("AV_CODEC_ID_HEVC");
            p2_vcodec_cbox->addItem("AV_CODEC_ID_MPEG4");

            p2_acodec_cbox->addItem("AV_CODEC_ID_AAC");
            p2_acodec_cbox->addItem("AV_CODEC_ID_MP3");
            p2_acodec_cbox->addItem("AV_CODEC_ID_OPUS");
            p2_acodec_cbox->addItem("AV_CODEC_ID_FLAC");
            break;
        case 3: /*webm*/
            p2_vcodec_cbox->addItem("AV_CODEC_ID_VP9");
            p2_vcodec_cbox->addItem("AV_CODEC_ID_VP8");
            p2_vcodec_cbox->addItem("AV_CODEC_ID_AV1");

            p2_acodec_cbox->addItem("AV_CODEC_ID_OPUS");
            break;
        }
    }
}

void MainWidget::vcodec_cbox_changed()
{
    p2_pixfmt_cbox->clear();

    switch (p2_vcodec_cbox->currentIndex()) {
    case 0: /*mp4*/
        if (p2_vcodec_cbox->currentIndex() == 0) {/*H264*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        else if (p2_vcodec_cbox->currentIndex() == 1) {/*HEVC (H265)*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        else {/*MPEG4*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        break;
    case 1: /*mov*/
        if (p2_vcodec_cbox->currentIndex() == 0) {/*H264*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        else if (p2_vcodec_cbox->currentIndex() == 1) {/*HEVC (H265)*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        else {/*MPEG4*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        break;
    case 2: /*mkv*/
        if (p2_vcodec_cbox->currentIndex() == 0) {/*H264*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        else if (p2_vcodec_cbox->currentIndex() == 1) {/*HEVC (H265)*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        else {/*MPEG4*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        break;
    case 3: /*webm*/
        if (p2_vcodec_cbox->currentIndex() == 0) {/*VP9*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        else if (p2_vcodec_cbox->currentIndex() == 1) {/*VP8*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        else {/*AV1*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUV420P");
        }
        break;
    }
}

void MainWidget::acodec_cbox_changed()
{
    p2_smplfmt_cbox->clear();

    if (fdata.media_type == converter::MEDIA_TYPE::MEDIA_AUDIO) {
        switch (p0_fn_cbox->currentIndex()) {
        case 0: /*MP3*/
            p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
            break;
        case 1: /*OPUS*/
            p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_FLTP");
            p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
            break;
        case 2: /*WAV*/
            p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
            p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_FLT");
            break;
        case 3: /*AAC*/
            p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_FLTP");
            p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
            break;
        case 4: /*FLAC*/
            p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
            p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S32");
            break;
        }
    }
    else {/*MEDIA_TYPE::VIDEO*/
        switch (p0_fn_cbox->currentIndex()) {
        case 0: /*mp4*/
            if (p2_acodec_cbox->currentIndex() == 0) {/*AAC*/
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_FLTP");
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
            }
            else {/*MP3*/
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
            }
            break;
        case 1: /*mov*/
            if (p2_acodec_cbox->currentIndex() == 0) {/*AAC*/
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_FLTP");
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
            }
            else {/*MP3*/
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
            }
            break;
        case 2: /*mkv*/
            switch (p2_acodec_cbox->currentIndex()) {
            case 0:/*AAC*/
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_FLTP");
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
                break;
            case 1:/*MP3*/
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
                break;
            case 2:/*OPUS*/
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_FLTP");
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
                break;
            case 3:/*FLAC*/
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S32");
                break;
            }
            break;
        case 3: /*webm*/
            if (p2_acodec_cbox->currentIndex() == 0) {/*OPUS*/
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_FLTP");
                p2_smplfmt_cbox->addItem("AV_SAMPLE_FMT_S16");
            }
            break;
        }
    }
}

void MainWidget::icodec_cbox_changed()
{
    p2_pixfmt_cbox->clear();

    switch (p0_fn_cbox->currentIndex()) {
    case 0: /*PNG*/
        p2_pixfmt_cbox->addItem("AV_PIX_FMT_RGBA");
        p2_pixfmt_cbox->addItem("AV_PIX_FMT_RGB24");
        p2_pixfmt_cbox->addItem("AV_PIX_FMT_GRAY8");
        break;
    case 1: /*JPG*/
    case 2: /*JPEG*/
        p2_pixfmt_cbox->addItem("AV_PIX_FMT_YUVJ420P");
        break;
    case 3: /*ICO*/
        if (p2_icodec_cbox->currentIndex() == 0) {/*PNG_CODEC*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_RGBA");
        }
        else {/*BMP_CODEC*/
            p2_pixfmt_cbox->addItem("AV_PIX_FMT_BGRA");
            //p2_pixfmt_cbox->addItem("AV_PIX_FMT_BGR24");
        }
        break;
    case 4: /*bmp*/
        p2_pixfmt_cbox->addItem("AV_PIX_FMT_BGRA");
        p2_pixfmt_cbox->addItem("AV_PIX_FMT_BGR24");
        p2_pixfmt_cbox->addItem("AV_PIX_FMT_BGR8");
        p2_pixfmt_cbox->addItem("AV_PIX_FMT_GRAY8");
        p2_pixfmt_cbox->addItem("AV_PIX_FMT_MONOBLACK");
        break;
    }
}
