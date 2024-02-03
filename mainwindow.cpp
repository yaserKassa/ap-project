#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "iomanager.h"
#include "newtaskdialog.h"
#include "createdatafiledialog.h"
#include <QHeaderView>
#include<Qmenu>
#include<Qmenubar>
#include<QVBoxLayout>
MainWindow::MainWindow()
{
    // This is the default path, but if it does not exist, you can change it into the program
    path = "/home/carlos/Escritorio/database.txt";

    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    // MenuBar
    createMenus();

    // Filters
    QHBoxLayout *filtersLayout = new QHBoxLayout;
    createFilters(filtersLayout);
    //filtersLayout->setContentsMargins();

    //Table
    table = new QTableWidget(this);
    initializeTable();

    QVBoxLayout *layout = new QVBoxLayout;
    //layout -> setMargin(8);
    layout -> addLayout(filtersLayout);
    layout -> addWidget(table);
    layout -> setStretch(0, 0);
    layout -> setStretch(1, 0);
    layout -> setStretch(2, 1);

    widget -> setLayout(layout);

    setWindowTitle("ToDo List QT");
    setMinimumSize(480, 320);
    resize(580, 480);
}

void MainWindow::createMenus()
{
    addTaskAct = new QAction(tr("&Add Task"), this);
    if(IOManager::exists(path) == false){
        addTaskAct->setEnabled(false);
    }
    addTaskAct -> setShortcuts(QKeySequence::New);
    connect(addTaskAct, &QAction::triggered, this, &MainWindow::addTask);

    newDatafileAct = new QAction(tr("&Create New Data File"), this);
    connect(newDatafileAct, &QAction::triggered, this, &MainWindow::newDatafile);

    changeDBAct = new QAction(tr("&Change Data File"), this);
    connect(changeDBAct, &QAction::triggered, this, &MainWindow::changeDB);

    exitProgramAct = new QAction(tr("&Exit"), this);
    connect(exitProgramAct, &QAction::triggered, this, &MainWindow::exitProgram);

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(addTaskAct);
    fileMenu->addAction(newDatafileAct);
    fileMenu->addAction(changeDBAct);
    fileMenu->addAction(exitProgramAct);
}


void MainWindow::createFilters(QHBoxLayout *layout)
{
    rb_all = new QRadioButton;
    rb_overdue = new QRadioButton;
    rb_today = new QRadioButton;
    rb_thisweek = new QRadioButton;
    cb_completed = new QCheckBox;

    rb_all->setText(tr("&All"));
    rb_all->setChecked(true);
    rb_overdue->setText(tr("&Overdue"));
    rb_today->setText(tr("&Today"));
    rb_thisweek->setText(tr("&This Week"));
    cb_completed->setText(tr("&Not Completed"));

    connect(rb_all,SIGNAL(clicked()),this,SLOT(filter()));
    connect(rb_overdue,SIGNAL(clicked()),this,SLOT(filter()));
    connect(rb_today,SIGNAL(clicked()),this,SLOT(filter()));
    connect(rb_thisweek,SIGNAL(clicked()),this,SLOT(filter()));
    connect(cb_completed,SIGNAL(clicked()),this,SLOT(filter()));

    QHBoxLayout *cbLayout = new QHBoxLayout;
    cbLayout -> setAlignment(Qt::AlignRight);
    cbLayout -> addWidget(cb_completed);

    layout->addWidget(rb_all);
    layout->addWidget(rb_overdue);
    layout->addWidget(rb_today);
    layout->addWidget(rb_thisweek);
    layout->addLayout(cbLayout);

    layout -> setStretch(0, 0);
    layout -> setStretch(1, 0);
    layout -> setStretch(2, 0);
    layout -> setStretch(3, 0);
    layout -> setStretch(4, 1);
}

void MainWindow::initializeTable()
{
    table -> setColumnCount(5);
    tableHeader << "Finished" << "DueDate" << "Title" << "% Complete" << "Description";
    table -> setHorizontalHeaderLabels(tableHeader);
    table -> horizontalHeader()->setStretchLastSection(true);
    table -> verticalHeader() -> setVisible(false);
    table -> setEditTriggers(QAbstractItemView::NoEditTriggers);
    table -> setSelectionBehavior(QAbstractItemView::SelectRows);
    table -> setSelectionMode(QAbstractItemView::SingleSelection);
    table -> setStyleSheet("QTableView {selection-background-color: #E0F7FA; selection-color: #000000;}");

    //inserting data
    updateTable(IOManager::readFile(path));

    connect( table, SIGNAL( cellDoubleClicked (int, int) ),
     this, SLOT( cellSelected( int, int ) ) );
}

void MainWindow::addTask()
{
    NewTaskDialog dlg;
    dlg.setModal(true);
    dlg.setPath(path);
    dlg.setOrigin(this);
    dlg.exec();
}

void MainWindow::newDatafile()
{
    CreateDatafileDialog dlg;
    dlg.setModal(true);
    dlg.setOrigin(this);
    dlg.exec();
}

void MainWindow::changeDB()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setNameFilter(tr("Text (*.txt)"));
    dlg.setViewMode(QFileDialog::List);

    QStringList fileNames;
    if (dlg.exec()){
        fileNames = dlg.selectedFiles();
        this->path = ((QString)fileNames.at(0)).toUtf8().constData();
        this->filter();
    }

    addTaskAct->setEnabled(true);
}

void MainWindow::exitProgram()
{
    exit(0);
}

void MainWindow::cellSelected(int nRow, int nCol)
{
    NewTaskDialog dlg;
    dlg.setModal(true);
    dlg.setData(table->item(nRow, 1)->text().toUtf8().constData(), table->item(nRow, 2)->text().toUtf8().constData(), table->item(nRow, 3)->text().toUtf8().constData(), table->item(nRow, 4)->text().toUtf8().constData());
    dlg.setPath(path);
    dlg.setOrigin(this);
    dlg.exec();
}

void MainWindow::updateTable(vector<string*> data){
    table -> clearContents();
    table -> setRowCount(data.size());
    unsigned int i;
    for(i=0; i<data.size(); i++){
        QCheckBox *qcb = new QCheckBox;
        qcb->setStyleSheet(QStringLiteral("QCheckBox::indicator {subcontrol-position: center;}"));
        // Set the checkbox as not editable and check it if necessary
        qcb->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        qcb->setFocusPolicy(Qt::NoFocus);
        qcb -> setChecked(stoi(data.at(i)[2]) == 100);

        table -> setAlternatingRowColors(true);
        table -> setCellWidget(i, 0, qcb);
        QTableWidgetItem *qdd = new QTableWidgetItem(QString::fromStdString(data.at(i)[0]));
        qdd -> setTextAlignment(Qt::AlignCenter);
        table -> setItem(i, 1, qdd);
        table -> setItem(i, 2, new QTableWidgetItem(QString::fromStdString(data.at(i)[1])));
        QTableWidgetItem *qtwi = new QTableWidgetItem(QString::fromStdString(data.at(i)[2]));
        qtwi -> setTextAlignment(Qt::AlignCenter);
        table -> setItem(i, 3, qtwi);
        table -> setItem(i, 4, new QTableWidgetItem(QString::fromStdString(data.at(i)[3])));
    }
}


void MainWindow::filter()
{
    vector<string*> data = IOManager::readFile(path);
    bool completed = cb_completed->isChecked();

    time_t now = time(0);
    tm cur = *gmtime(&now);

    unsigned int i;
    for(i=0; i<data.size(); i++){
        int year = stoi(IOManager::split(data.at(i)[0], '/')[0]);
        int month = stoi(IOManager::split(data.at(i)[0], '/')[1]);
        int day = stoi(IOManager::split(data.at(i)[0], '/')[2]);

        if(completed){
            if(stoi(data.at(i)[2]) == 100){
                data.erase(data.begin()+i);
                i--;
                continue;
            }
        }

        if(rb_today->isChecked()){
            if(cur.tm_year+1900 != year || cur.tm_mon+1 != month || cur.tm_mday != day){
                data.erase(data.begin()+i);
                i--;
            }
        }else if(rb_thisweek->isChecked()){
            time_t auxd = time(0);
            tm *auxdate = gmtime(&auxd);
            auxdate->tm_mday = day;
            auxdate->tm_mon = month - 1;
            auxdate->tm_year = year - 1900;
            auxd = mktime(auxdate);
            auxdate =  gmtime(&auxd);
            tm cp = *auxdate;

            if(getWeekNumber(cur) != getWeekNumber(cp) || cur.tm_year != cp.tm_year){
                data.erase(data.begin()+i);
                i--;
            }
        }else if(rb_overdue->isChecked()){
            if(cur.tm_year+1900 < year || (cur.tm_year+1900 == year && cur.tm_mon+1 < month) || (cur.tm_year+1900 == year && cur.tm_mon+1 == month && cur.tm_mday <= day)){
                data.erase(data.begin()+i);
                i--;
            }
        }
    }

    updateTable(data);
}


int MainWindow::getWeekNumber(tm t)
{
    time_t nnow = time(0);
    tm *mauxdate = gmtime(&nnow);
    mauxdate->tm_mday = 0;
    mauxdate->tm_mon = 0;
    mauxdate->tm_year = mauxdate->tm_year;
    nnow = mktime(mauxdate);
    mauxdate =  gmtime(&nnow);

    int julian = t.tm_yday;  // Jan 1 = 1, Jan 2 = 2, etc...
    int dow = t.tm_wday;     // Sun = 0, Mon = 1, etc...
    int dowJan1 = mauxdate->tm_wday;   // find out first of year's day
    int weekNum = ((julian + 6) / 7);   // probably better.  CHECK THIS LINE. (See comments.)
    if (dow < dowJan1){                 // adjust for being after Saturday of week #1
        ++weekNum;
    }
    return weekNum;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete rb_all;
    delete rb_overdue;
    delete rb_today;
    delete rb_thisweek;
    delete cb_completed;
    delete table;
    delete addTaskAct;
    delete changeDBAct;
    delete exitProgramAct;
    delete fileMenu;
}
