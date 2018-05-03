﻿#ifndef FORMCENTER_H
#define FORMCENTER_H

#include <QWidget>
#include "storemanager.h"
#include "personmanager.h"
#include "ordermanager.h"
#include "systemmanager.h"

#pragma execution_character_set("utf-8")
namespace Ui {
class FormCenter;
}

class FormCenter : public QWidget
{
    Q_OBJECT

public:
    explicit FormCenter(QWidget *parent = 0);
    ~FormCenter();

private slots:
    void on_order_btn_clicked();

    void on_store_btn_clicked();

    void on_person_btn_clicked();

    void on_set_Btn_clicked();

    void on_exit_clicked();

    void clearChecked();

private:
    Ui::FormCenter *ui;
    OrderManager    m_order;
    StoreManager    m_store;
    PersonManager   m_person;
    SystemManager   m_sys;
};

#endif // FORMCENTER_H
