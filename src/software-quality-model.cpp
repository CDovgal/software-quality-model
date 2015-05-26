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

#include <dlib/bayes_utils.h>
#include <dlib/graph_utils.h>
#include <dlib/graph.h>
#include <dlib/directed_graph.h>

#include <algorithm>
#include <string>


SQM_client::SQM_client(QWidget* ip_parent)
    : QMainWindow(ip_parent)
    , mp_ui(std::make_shared<Ui::SQM_client>())
{
    mp_ui->setupUi(this);
    using namespace dlib;
    using namespace dlib::bayes_node_utils;
    directed_graph<bayes_node>::kernel_1a_c bn;
}

SQM_client::~SQM_client()
{}
