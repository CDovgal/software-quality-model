/*
* @file    software-quality-model.h
* @brief   
* @date    Date Created:: 2015-05-26 08-35
* @version $Revision:: 1#$
* @author  Dovgal Kostiantyn
* @copyright (c) The MIT License 2015
*/

#pragma once

#include <memory>

#include <QMainWindow>

namespace Ui 
{
    class SQM_client;
}

class SQM_client : public QMainWindow
{
  Q_OBJECT
public:
    explicit SQM_client(QWidget* parent = nullptr);
    ~SQM_client();

    void build_network(bool);
  
private:
    std::shared_ptr<Ui::SQM_client> mp_ui;
};
