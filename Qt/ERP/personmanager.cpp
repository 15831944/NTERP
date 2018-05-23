﻿#include "personmanager.h"
#include "ui_personmanager.h"
#include "datacenter.h"
#include <QMessageBox>
#include "dialoguserprint.h"
#include "boost/thread.hpp"
#include <QDebug>

PersonManager::PersonManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PersonManager)
{
    ui->setupUi(this);
    tab_mode = QHeaderView::Stretch;
    newuser =NULL;
    ui->radioButton_ave->setChecked(true);
    connect(ui->radioButton_ave,SIGNAL(clicked(bool)),this,SLOT(changeCol()));
    connect(ui->radioButton_connent,SIGNAL(clicked(bool)),this,SLOT(changeCol()));
    connect(ui->radioButton_manue,SIGNAL(clicked(bool)),this,SLOT(changeCol()));
    connect(ui->tableWidget,SIGNAL(userClick(QString)),this,SLOT(userClick(QString)));

    connect(dataCenter::instance(),SIGNAL(sig_outEmployee(User,bool)),this,SLOT(outUserCb(User,bool)));
    connect(dataCenter::instance(),SIGNAL(sig_delEmployee(User,bool)),this,SLOT(delUserCb(User,bool)));

    connect(ui->tableWidget,SIGNAL(newUser()),this,SLOT(on_pushButton_newUser_clicked()));
    connect(ui->tableWidget,SIGNAL(modUser()),this,SLOT(on_pushButton_mod_clicked()));
    connect(ui->tableWidget,SIGNAL(outUser()),this,SLOT(on_pushButton_out_clicked()));
    connect(ui->tableWidget,SIGNAL(delUser()),this,SLOT(on_pushButton_del_clicked()));


    ui->pushButton_mod->setEnabled(false);
    ui->pushButton_out->setEnabled(false);
    ui->pushButton_del->setEnabled(false);
    updateData();
}

PersonManager::~PersonManager()
{
    delete ui;
}



void PersonManager::updateData()
{
    ui->tableWidget->initUser(dataCenter::instance()->employees());
}

void PersonManager::clearAllSelect()
{
    ui->tableWidget->clearSelection();
    ui->pushButton_mod->setEnabled(false);
    ui->pushButton_out->setEnabled(false);
    ui->pushButton_del->setEnabled(false);
    curUser.UID=="";
}


void PersonManager::new_employee()
{
    if(newuser==NULL){
        newuser = new DialogNewUser();
    }
    newuser->setModel(true);
    newuser->clearUI();
    if(newuser->exec()==123){
        User user = newuser->CurUser();
        ui->tableWidget->appendUser(user);
    }
}



void PersonManager::on_pushButton_newUser_clicked()
{
    new_employee();
}

void PersonManager::on_pushButton_mod_clicked()
{
    if(curUser.UID==""){
        return;
    }
    if(newuser==NULL){
        newuser = new DialogNewUser();
    }
    newuser->setModel(false);
    newuser->clearUI();
    newuser->initUI(curUser);
    if(newuser->exec()==123){
        User user = newuser->CurUser();
        ui->tableWidget->modUser(user);
    }
}

void PersonManager::userClick(QString UID)
{
    bool ok =false;
    curUser = dataCenter::instance()->getUser(UID,ok);
    if(curUser.Status=="-1"){
        ui->pushButton_mod->setEnabled(false);
        ui->pushButton_out->setEnabled(false);
        ui->pushButton_del->setEnabled(false);
    }
    if(curUser.Status=="0"){
        ui->pushButton_mod->setEnabled(true);
        ui->pushButton_out->setEnabled(true);
        ui->pushButton_del->setEnabled(true);
    }
    if(curUser.Status=="1"){
        ui->pushButton_mod->setEnabled(false);
        ui->pushButton_out->setEnabled(false);
        ui->pushButton_del->setEnabled(true);
    }

}


void PersonManager::on_pushButton_out_clicked()
{
    if(curUser.UID==""){
        return;
    }
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setText("您将离职员工:"+curUser.Name);
    msgBox.setInformativeText("是否继续操作?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Ok:
    {
        boost::thread t(boost::bind(&dataCenter::outUser,dataCenter::instance(),UserService::toJsonObject(curUser)));
        t.detach();
        dataCenter::instance()->showLoadding("正在网络请求...",5000,Qt::black);
        break;
    }
    case QMessageBox::Cancel:
        break;
    default:
        break;
    }
}

void PersonManager::on_pushButton_del_clicked()
{
    if(curUser.UID==""){
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setText("您将永久删除员工:"+curUser.Name+"的全部信息,");
    msgBox.setInformativeText("是否继续操作?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Ok:
    {
        boost::thread t(boost::bind(&dataCenter::delUser,dataCenter::instance(),UserService::toJsonObject(curUser)));
        t.detach();
        dataCenter::instance()->showLoadding("正在网络请求...",5000,Qt::black);
        break;
    }
    case QMessageBox::Cancel:
        break;
    default:
        break;
    }
}


void PersonManager::outUserCb(User user, bool ok)
{
    dataCenter::instance()->hideLoadding();
    if(ok){
        dataCenter::instance()->showMessage("离职成功!",4000);
        ui->tableWidget->modUser(user);
    }else{
        dataCenter::instance()->showMessage("离职失败!",4000);
    }
}

void PersonManager::delUserCb(User user, bool ok)
{
    dataCenter::instance()->hideLoadding();
    if(ok){
        dataCenter::instance()->showMessage("删除成功!",4000);
        ui->tableWidget->removeUser(user);
        ui->pushButton_mod->setEnabled(false);
        ui->pushButton_out->setEnabled(false);
        ui->pushButton_del->setEnabled(false);
    }else{
        dataCenter::instance()->showMessage("删除失败!",4000);
    }
}


void PersonManager::changeCol()
{
    if(ui->radioButton_ave->isChecked()){
        ui->tableWidget->setHeaderColModel(QHeaderView::Stretch);
    }
    if(ui->radioButton_connent->isChecked()){
        ui->tableWidget->setHeaderColModel(QHeaderView::ResizeToContents);

    }
    if(ui->radioButton_manue->isChecked()){
        ui->tableWidget->setHeaderColModel(QHeaderView::Interactive);
    }
    clearAllSelect();
}



void PersonManager::on_pushButton_export_clicked()
{
    DialogUserPrint  print;
    print.initData(dataCenter::instance()->employees());
    print.exec();
}

void PersonManager::on_pushButton_reflash_clicked()
{
    updateData();
}

