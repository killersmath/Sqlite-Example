#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QSqlTableModel>
#include <QListView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(createConnection()){
        QSqlQueryModel *recipeModel = new QSqlQueryModel(this);
        QSqlQueryModel *detailsModel = new QSqlQueryModel(this);
        recipeModel->setQuery("SELECT name as 'Recipe' FROM recipes ORDER BY name");

        ui->listView->setModel(recipeModel);
        ui->tableView->setModel(detailsModel);

        connect(ui->listView, &QListView::clicked, [detailsModel](const QModelIndex& index){
            if(!index.isValid())
                return;

            QByteArray recipeName = index.data(Qt::DisplayRole).toByteArray();

            detailsModel->setQuery("select RP.ingredient as 'Ingredient', RP.amount as 'Amount' from recipes_details as RP INNER JOIN recipes as R ON R.id = rp.id_recipe AND R.name = '"+recipeName+"'");
        });
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::createConnection()
{
    /* FROM SQLITE DOCUMENTATION */
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
                              qApp->tr("Unable to establish a database connection.\n"
                                       "This example needs SQLite support. Please read "
                                       "the Qt SQL driver documentation for information how "
                                       "to build it.\n\n"
                                       "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;

    query.exec("create table recipes ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "name  varchar(40) UNIQUE NOT NULL"
               ");");

    query.exec("create table recipes_details ("
               "id_recipe INTEGER REFERENCES recipes(id),"
               "ingredient varchar(30) NOT NULL,"
               "amount int NOT NULL"
               ");");

    query.exec("INSERT INTO recipes (name) VALUES"
               "('Recipe A'),"
               "('Recipe B'),"
               "('Recipe C'),"
               "('Recipe D');");

    query.exec("INSERT INTO recipes_details (id_recipe, ingredient, amount) VALUES"
               "(1, 'Ingredient 1', 3),"
               "(1, 'Ingredient 2', 5),"
               "(2, 'Ingredient 1', 5),"
               "(3, 'Ingredient 1', 10);");

    return true;
}
