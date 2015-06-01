/*
* @file    software-quality-model.cpp
* @brief   
* @date    Date Created:: 2015-05-26 08-32
* @version $Revision:: 1#$
* @author  Dovgal Kostiantyn
* @copyright (c) The MIT License 2015
*/

#include "software-quality-model.h"
#include "ui_software-quality-model.h"

#include <algorithm>
#include <string>

#include <Netica.h>

SQM_client::SQM_client(QWidget* ip_parent)
    : QMainWindow(ip_parent)
    , mp_ui(std::make_shared<Ui::SQM_client>())
{
    mp_ui->setupUi(this);
    environ_ns* env = NewNeticaEnviron_ns (NULL, NULL, NULL);

}

SQM_client::~SQM_client()
{}
