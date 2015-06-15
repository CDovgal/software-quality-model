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
#include <chrono>


SQM_client::SQM_client(QWidget* ip_parent)
    : QMainWindow(ip_parent)
    , mp_ui(std::make_shared<Ui::SQM_client>())
{
    mp_ui->setupUi(this);
    connect(mp_ui->mp_button_calculate, &QAbstractButton::pressed, this, &SQM_client::build_network);

    connect(mp_ui->mp_button_doxygen, &QAbstractButton::pressed, [&] ()
        {
            mp_ui->mp_dsqi->setValue(0.9);
            mp_ui->mp_average_size->setValue(150);
            mp_ui->mp_code_coverage->setValue(0.75);
            mp_ui->mp_cyclomatic->setValue(3.6);
            mp_ui->mp_comment_ratio->setValue(0.09);
            mp_ui->mp_coupling->setValue(0.5);
            mp_ui->mp_LCOM4->setValue(0.9);
            build_network();
        });

    connect(mp_ui->mp_button_activemq, &QAbstractButton::pressed, [&] ()
        {
            mp_ui->mp_dsqi->setValue(0.7);
            mp_ui->mp_average_size->setValue(100);
            mp_ui->mp_code_coverage->setValue(0.16);
            mp_ui->mp_cyclomatic->setValue(17);
            mp_ui->mp_comment_ratio->setValue(0.15);
            mp_ui->mp_coupling->setValue(0.168);
            mp_ui->mp_LCOM4->setValue(0.7);
            build_network();
        });

    connect(mp_ui->mp_use_lcom4,        &QCheckBox::stateChanged, [&](bool i_state) {mp_ui->mp_LCOM4->setEnabled(i_state); });
    connect(mp_ui->mp_use_coupling,     &QCheckBox::stateChanged, [&](bool i_state) {mp_ui->mp_coupling->setEnabled(i_state); });
    connect(mp_ui->mp_use_comment,      &QCheckBox::stateChanged, [&](bool i_state) {mp_ui->mp_comment_ratio->setEnabled(i_state); });
    connect(mp_ui->mp_use_cyclomatic,   &QCheckBox::stateChanged, [&](bool i_state) {mp_ui->mp_cyclomatic->setEnabled(i_state); });
    connect(mp_ui->mp_use_testcoverage, &QCheckBox::stateChanged, [&](bool i_state) {mp_ui->mp_code_coverage->setEnabled(i_state); });
    connect(mp_ui->mp_use_module_size,  &QCheckBox::stateChanged, [&](bool i_state) {mp_ui->mp_average_size->setEnabled(i_state); });
    connect(mp_ui->mp_use_dsqi,         &QCheckBox::stateChanged, [&](bool i_state) {mp_ui->mp_dsqi->setEnabled(i_state); });
    
}

SQM_client::~SQM_client()
{}

int SQM_client::dsqi()
{
    if (mp_ui->mp_dsqi->value() <= 0.75)
        return 0;

    if (mp_ui->mp_dsqi->value() <= 0.9)
        return 1;

    return 2;
}

int SQM_client::average_module_size()
{
    if (mp_ui->mp_average_size->value() < 100 || mp_ui->mp_average_size->value() > 1600)
        return 0;

    if (mp_ui->mp_average_size->value() > 600)
        return 1;

    return 2;

}

int SQM_client::code_coverage()
{
    if (mp_ui->mp_code_coverage->value() < 0.1)
        return 0;

    if (mp_ui->mp_code_coverage->value() < 0.5)
        return 1;

    return 2;
}

int SQM_client::cyclomatic_complexity()
{
    if (mp_ui->mp_cyclomatic->value() > 12)
        return 0;

    if (mp_ui->mp_cyclomatic->value() > 6)
        return 1;

    return 2;
}

int SQM_client::comment_ratio()
{
    if (mp_ui->mp_comment_ratio->value() > 0.4)
      return 0;
  
    return 1;
}

int SQM_client::coupling()
{
    if (mp_ui->mp_coupling->value() > 0.8)
      return 0;
  
      return 1;
}

int SQM_client::cohesion()
{
    if (mp_ui->mp_LCOM4->value() > 5)
      return 0;
  
    return 1;
}

void SQM_client::build_network()
{
#ifndef _DEBUG
    try
    {
#endif
        auto start = std::chrono::system_clock::now();
        using namespace dlib;
        using namespace std;
        using namespace dlib::bayes_node_utils;
        directed_graph<bayes_node>::kernel_1a_c bn;

        // Use an enum to make some more readable names for our nodes.
        enum nodes
        {
            Maintenance = 0,
            Analysis,
            QA,
            Implementation,
            Understanding,
            Design,  //5
            CodeReading,
            Testing,
            Modification,
            I_DesignQuality,
            I_ModuleSize,   //10
            I_ImplementationAccuracy,
            I_CodeCoverage,
            I_Cohesion,
            I_Coupling,
            I_CommentRatio,      // 15

            NumberOfNodes
        };

        // The next few blocks of code setup our bayesian network.

        // The first thing we do is tell the bn object how many nodes it has
        // and also add the three edges.  Again, we are using the network
        // shown in ASCII art at the top of this file.
        bn.set_number_of_nodes(16);


        // add_edge - from, to
        bn.add_edge(Analysis, Maintenance);                                                   bn.add_edge(QA, Maintenance);                  bn.add_edge(Implementation, Maintenance);
        bn.add_edge(Understanding, Analysis);

        bn.add_edge(Design, Understanding);     bn.add_edge(CodeReading, Understanding);        bn.add_edge(Testing, QA);                      bn.add_edge(Modification, Implementation);


        bn.add_edge(I_DesignQuality, Design);    bn.add_edge(I_ModuleSize, CodeReading);         bn.add_edge(I_ImplementationAccuracy, Testing);  bn.add_edge(I_Cohesion, Modification);
        /*************************************************************************************/ bn.add_edge(I_CodeCoverage, Testing);            bn.add_edge(I_Coupling, Modification);
        /**************************************************************************************************************************************/ bn.add_edge(I_CommentRatio, Modification);;;;;;;;;;;;;


        // Now we inform all the nodes in the network that they are binary
        // nodes.  That is, they only have two possible values.

        for (int i = 0; i < 16; ++i)
        {
            set_node_num_values(bn, i, 3);
        }

        assignment parent_state;
        // Now we will enter all the conditional probability information for each node.
        // Each node's conditional probability is dependent on the state of its parents.  
        // To specify this state we need to use the assignment object.  This assignment 
        // object allows us to specify the state of each nodes parents. 


        // Here we specify that p(B=1) = 0.01
        // parent_state is empty in this case since B is a root node.


        // init indicators
        set_node_probability(bn, I_DesignQuality, 2, parent_state, 0.1);
        set_node_probability(bn, I_DesignQuality, 1, parent_state, 0.15);
        set_node_probability(bn, I_DesignQuality, 0, parent_state, 0.75);


        set_node_probability(bn, I_ModuleSize, 2, parent_state, 0.4);
        set_node_probability(bn, I_ModuleSize, 1, parent_state, 0.3);
        set_node_probability(bn, I_ModuleSize, 0, parent_state, 0.3);


        set_node_probability(bn, I_ImplementationAccuracy, 2, parent_state, 0.5);
        set_node_probability(bn, I_ImplementationAccuracy, 1, parent_state, 0.3);
        set_node_probability(bn, I_ImplementationAccuracy, 0, parent_state, 0.2);


        set_node_probability(bn, I_CodeCoverage, 2, parent_state, 0.8);
        set_node_probability(bn, I_CodeCoverage, 1, parent_state, 0.1);
        set_node_probability(bn, I_CodeCoverage, 0, parent_state, 0.1);


        set_node_probability(bn, I_Coupling, 2, parent_state, 0.6);
        set_node_probability(bn, I_Coupling, 1, parent_state, 0.3);
        set_node_probability(bn, I_Coupling, 0, parent_state, 0.1);


        set_node_probability(bn, I_Cohesion, 2, parent_state, 0.2);
        set_node_probability(bn, I_Cohesion, 1, parent_state, 0.5);
        set_node_probability(bn, I_Cohesion, 0, parent_state, 0.3);


        set_node_probability(bn, I_CommentRatio, 2, parent_state, 0.4);
        set_node_probability(bn, I_CommentRatio, 1, parent_state, 0.3);
        set_node_probability(bn, I_CommentRatio, 0, parent_state, 0.3);



        ///      Design ==>  I_DesignQuality
        parent_state.add(I_DesignQuality, 2);
        set_node_probability(bn, Design, 2, parent_state, 0.5);
        set_node_probability(bn, Design, 1, parent_state, 0.35);
        set_node_probability(bn, Design, 0, parent_state, 0.15);

        parent_state[I_DesignQuality] = 1;
        set_node_probability(bn, Design, 2, parent_state, 0.5);
        set_node_probability(bn, Design, 1, parent_state, 0.3);
        set_node_probability(bn, Design, 0, parent_state, 0.2);

        parent_state[I_DesignQuality] = 0;
        set_node_probability(bn, Design, 2, parent_state, 0.1);
        set_node_probability(bn, Design, 1, parent_state, 0.8);
        set_node_probability(bn, Design, 0, parent_state, 0.1);
        /////////////////////////////////////////////////////


        ///      CodeReading ==>  I_ModuleSize 
        parent_state.clear();
        parent_state.add(I_ModuleSize, 2);
        set_node_probability(bn, CodeReading, 2, parent_state, 0.4);
        set_node_probability(bn, CodeReading, 1, parent_state, 0.3);
        set_node_probability(bn, CodeReading, 0, parent_state, 0.3);

        parent_state[I_ModuleSize] = 1;
        set_node_probability(bn, CodeReading, 2, parent_state, 0.3);
        set_node_probability(bn, CodeReading, 1, parent_state, 0.4);
        set_node_probability(bn, CodeReading, 0, parent_state, 0.3);

        parent_state[I_ModuleSize] = 0;
        set_node_probability(bn, CodeReading, 2, parent_state, 0.3);
        set_node_probability(bn, CodeReading, 1, parent_state, 0.3);
        set_node_probability(bn, CodeReading, 0, parent_state, 0.4);
        /////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////////////
        ///      Modification ==>  Implementation 
        parent_state.clear();
        parent_state.add(Modification, 2);
        set_node_probability(bn, Implementation, 2, parent_state, 0.5);
        set_node_probability(bn, Implementation, 1, parent_state, 0.3);
        set_node_probability(bn, Implementation, 0, parent_state, 0.2);

        parent_state[Modification] = 1;
        set_node_probability(bn, Implementation, 2, parent_state, 0.2);
        set_node_probability(bn, Implementation, 1, parent_state, 0.5);
        set_node_probability(bn, Implementation, 0, parent_state, 0.3);

        parent_state[Modification] = 0;
        set_node_probability(bn, Implementation, 2, parent_state, 0.3);
        set_node_probability(bn, Implementation, 1, parent_state, 0.3);
        set_node_probability(bn, Implementation, 0, parent_state, 0.4);
        /////////////////////////////////////////////////////



        /// I_CommentRatio ==>
        /// I_Cohesion ===>         Modification
        /// I_Coupling ===>

        parent_state.clear();
        parent_state.add(I_CommentRatio, 0);
        parent_state.add(I_Cohesion, 0);
        parent_state.add(I_Coupling, 0);

        // (0,0,0)
        set_node_probability(bn, Modification, 2, parent_state, 0.1);
        set_node_probability(bn, Modification, 1, parent_state, 0.1);
        set_node_probability(bn, Modification, 0, parent_state, 0.8);

        // (0, 0, 1)
        parent_state[I_Coupling] = 1;
        set_node_probability(bn, Modification, 2, parent_state, 0.1);
        set_node_probability(bn, Modification, 1, parent_state, 0.2);
        set_node_probability(bn, Modification, 0, parent_state, 0.7);

        // (0, 0, 2)
        parent_state[I_Coupling] = 2;
        set_node_probability(bn, Modification, 2, parent_state, 0.15);
        set_node_probability(bn, Modification, 1, parent_state, 0.45);
        set_node_probability(bn, Modification, 0, parent_state, 0.4);

        // (0, 1, 0)
        parent_state[I_Cohesion] = 1;
        parent_state[I_Coupling] = 0;
        set_node_probability(bn, Modification, 2, parent_state, 0.2);
        set_node_probability(bn, Modification, 1, parent_state, 0.2);
        set_node_probability(bn, Modification, 0, parent_state, 0.6);

        // (0, 1, 1)
        parent_state[I_Coupling] = 1;
        set_node_probability(bn, Modification, 2, parent_state, 0.15);
        set_node_probability(bn, Modification, 1, parent_state, 0.25);
        set_node_probability(bn, Modification, 0, parent_state, 0.6);

        // (0, 1, 2)
        parent_state[I_Coupling] = 2;
        set_node_probability(bn, Modification, 2, parent_state, 0.2);
        set_node_probability(bn, Modification, 1, parent_state, 0.3);
        set_node_probability(bn, Modification, 0, parent_state, 0.5);

        // (0, 2, 0)
        parent_state[I_Cohesion] = 2;
        parent_state[I_Coupling] = 0;
        set_node_probability(bn, Modification, 2, parent_state, 0.25);
        set_node_probability(bn, Modification, 1, parent_state, 0.25);
        set_node_probability(bn, Modification, 0, parent_state, 0.5);

        // (0, 2, 1)
        parent_state[I_Coupling] = 1;
        set_node_probability(bn, Modification, 2, parent_state, 0.25);
        set_node_probability(bn, Modification, 1, parent_state, 0.35);
        set_node_probability(bn, Modification, 0, parent_state, 0.4);

        // (0, 2, 2)
        parent_state[I_Coupling] = 2;
        set_node_probability(bn, Modification, 2, parent_state, 0.25);
        set_node_probability(bn, Modification, 1, parent_state, 0.55);
        set_node_probability(bn, Modification, 0, parent_state, 0.2);

        // (1, 0, 0)
        parent_state[I_CommentRatio] = 1;
        parent_state[I_Cohesion] = 0;
        parent_state[I_Coupling] = 0;
        set_node_probability(bn, Modification, 2, parent_state, 0.1);
        set_node_probability(bn, Modification, 1, parent_state, 0.2);
        set_node_probability(bn, Modification, 0, parent_state, 0.7);

        // (1, 0, 1)
        parent_state[I_Coupling] = 1;
        set_node_probability(bn, Modification, 2, parent_state, 0.15);
        set_node_probability(bn, Modification, 1, parent_state, 0.25);
        set_node_probability(bn, Modification, 0, parent_state, 0.6);

        // (1, 0, 2)
        parent_state[I_Coupling] = 2;
        set_node_probability(bn, Modification, 2, parent_state, 0.15);
        set_node_probability(bn, Modification, 1, parent_state, 0.35);
        set_node_probability(bn, Modification, 0, parent_state, 0.5);

        // (1, 1, 0)
        parent_state[I_Cohesion] = 1;
        parent_state[I_Coupling] = 0;
        set_node_probability(bn, Modification, 2, parent_state, 0.15);
        set_node_probability(bn, Modification, 1, parent_state, 0.25);
        set_node_probability(bn, Modification, 0, parent_state, 0.6);

        // (1, 1, 1)
        parent_state[I_Coupling] = 1;
        set_node_probability(bn, Modification, 2, parent_state, 0.25);
        set_node_probability(bn, Modification, 1, parent_state, 0.35);
        set_node_probability(bn, Modification, 0, parent_state, 0.4);

        // (1, 1, 2)
        parent_state[I_Coupling] = 2;
        set_node_probability(bn, Modification, 2, parent_state, 0.25);
        set_node_probability(bn, Modification, 1, parent_state, 0.45);
        set_node_probability(bn, Modification, 0, parent_state, 0.3); 

        // (1, 2, 0)
        parent_state[I_Cohesion] = 2;
        parent_state[I_Coupling] = 0;
        set_node_probability(bn, Modification, 2, parent_state, 0.15);
        set_node_probability(bn, Modification, 1, parent_state, 0.35);
        set_node_probability(bn, Modification, 0, parent_state, 0.5);

        // (1, 2, 1)
        parent_state[I_Coupling] = 1;
        set_node_probability(bn, Modification, 2, parent_state, 0.35);
        set_node_probability(bn, Modification, 1, parent_state, 0.25);
        set_node_probability(bn, Modification, 0, parent_state, 0.4);

        // (1, 2, 2)
        parent_state[I_Coupling] = 2;
        set_node_probability(bn, Modification, 2, parent_state, 0.45);
        set_node_probability(bn, Modification, 1, parent_state, 0.45);
        set_node_probability(bn, Modification, 0, parent_state, 0.1);


        // (2, 0, 0)
        parent_state[I_CommentRatio] = 2;
        parent_state[I_Cohesion] = 0;
        parent_state[I_Coupling] = 0;
        set_node_probability(bn, Modification, 2, parent_state, 0.2);
        set_node_probability(bn, Modification, 1, parent_state, 0.2);
        set_node_probability(bn, Modification, 0, parent_state, 0.6);

        // (2, 0, 1)
        parent_state[I_Coupling] = 1;
        set_node_probability(bn, Modification, 2, parent_state, 0.15);
        set_node_probability(bn, Modification, 1, parent_state, 0.25);
        set_node_probability(bn, Modification, 0, parent_state, 0.6);

        // (2, 0, 2)
        parent_state[I_Coupling] = 2;
        set_node_probability(bn, Modification, 2, parent_state, 0.2);
        set_node_probability(bn, Modification, 1, parent_state, 0.25);
        set_node_probability(bn, Modification, 0, parent_state, 0.55);

        // (2, 1, 0)
        parent_state[I_Cohesion] = 1;
        parent_state[I_Coupling] = 0;
        set_node_probability(bn, Modification, 2, parent_state, 0.15);
        set_node_probability(bn, Modification, 1, parent_state, 0.25);
        set_node_probability(bn, Modification, 0, parent_state, 0.6);

        // (2, 1, 1)
        parent_state[I_Coupling] = 1;
        set_node_probability(bn, Modification, 2, parent_state, 0.2);
        set_node_probability(bn, Modification, 1, parent_state, 0.5);
        set_node_probability(bn, Modification, 0, parent_state, 0.3);

        // (2, 1, 2)
        parent_state[I_Coupling] = 2;
        set_node_probability(bn, Modification, 2, parent_state, 0.3);
        set_node_probability(bn, Modification, 1, parent_state, 0.5);
        set_node_probability(bn, Modification, 0, parent_state, 0.2);

        // (2, 2, 0)
        parent_state[I_Cohesion] = 2;
        parent_state[I_Coupling] = 0;
        set_node_probability(bn, Modification, 2, parent_state, 0.3);
        set_node_probability(bn, Modification, 1, parent_state, 0.3);
        set_node_probability(bn, Modification, 0, parent_state, 0.4);

        // (2, 2, 1)
        parent_state[I_Coupling] = 1;
        set_node_probability(bn, Modification, 2, parent_state, 0.4);
        set_node_probability(bn, Modification, 1, parent_state, 0.4);
        set_node_probability(bn, Modification, 0, parent_state, 0.2);

        // (2, 2, 2)
        parent_state[I_Coupling] = 2;
        set_node_probability(bn, Modification, 2, parent_state, 0.6);
        set_node_probability(bn, Modification, 1, parent_state, 0.3);
        set_node_probability(bn, Modification, 0, parent_state, 0.1);



        //////////////////////////////////////////////////////////////////////////////////////

        ///      Design     ==>  
        ///                         Understanding
        ///      CodeReading ==>  
        parent_state.clear();
        // (2,2)
        parent_state.add(Design, 2);
        parent_state.add(CodeReading, 2);
        set_node_probability(bn, Understanding, 2, parent_state, 0.7);
        set_node_probability(bn, Understanding, 1, parent_state, 0.21);
        set_node_probability(bn, Understanding, 0, parent_state, 0.09);

        // (1, 2)
        parent_state[Design] = 1;
        set_node_probability(bn, Understanding, 2, parent_state, 0.6);
        set_node_probability(bn, Understanding, 1, parent_state, 0.39);
        set_node_probability(bn, Understanding, 0, parent_state, 0.01);

        // (1, 1)
        parent_state[CodeReading] = 1;
        set_node_probability(bn, Understanding, 2, parent_state, 0.6);
        set_node_probability(bn, Understanding, 1, parent_state, 0.19);
        set_node_probability(bn, Understanding, 0, parent_state, 0.21);

        // (2, 1)
        parent_state[Design] = 2;
        set_node_probability(bn, Understanding, 2, parent_state, 0.75);
        set_node_probability(bn, Understanding, 1, parent_state, 0.24);
        set_node_probability(bn, Understanding, 0, parent_state, 0.01);

        // (2, 0)
        parent_state[CodeReading] = 0;
        set_node_probability(bn, Understanding, 2, parent_state, 0.4);
        set_node_probability(bn, Understanding, 1, parent_state, 0.3);
        set_node_probability(bn, Understanding, 0, parent_state, 0.3);

        // (1, 0)
        parent_state[Design] = 1;
        set_node_probability(bn, Understanding, 2, parent_state, 0.3);
        set_node_probability(bn, Understanding, 1, parent_state, 0.6);
        set_node_probability(bn, Understanding, 0, parent_state, 0.1);

        // (0, 0)
        parent_state[Design] = 0;
        set_node_probability(bn, Understanding, 2, parent_state, 0.1);
        set_node_probability(bn, Understanding, 1, parent_state, 0.1);
        set_node_probability(bn, Understanding, 0, parent_state, 0.8);

        // (0, 1)
        parent_state[CodeReading] = 1;
        set_node_probability(bn, Understanding, 2, parent_state, 0.15);
        set_node_probability(bn, Understanding, 1, parent_state, 0.5);
        set_node_probability(bn, Understanding, 0, parent_state, 0.35);

        // (0, 1)
        parent_state[CodeReading] = 2;
        set_node_probability(bn, Understanding, 2, parent_state, 0.3);
        set_node_probability(bn, Understanding, 1, parent_state, 0.3);
        set_node_probability(bn, Understanding, 0, parent_state, 0.4);
        /////////////////////////




        ///      I_ImplementationAccuracy     ==>  
        ///                                         Testing
        ///      I_CodeCoverage             ==>  
        parent_state.clear();
        // (2,2)
        parent_state.add(I_ImplementationAccuracy, 2);
        parent_state.add(I_CodeCoverage, 2);
        set_node_probability(bn, Testing, 2, parent_state, 0.9);
        set_node_probability(bn, Testing, 1, parent_state, 0.09);
        set_node_probability(bn, Testing, 0, parent_state, 0.01);

        // (1, 2)
        parent_state[I_ImplementationAccuracy] = 1;
        set_node_probability(bn, Testing, 2, parent_state, 0.4);
        set_node_probability(bn, Testing, 1, parent_state, 0.3);
        set_node_probability(bn, Testing, 0, parent_state, 0.3);

        // (1, 1)
        parent_state[I_CodeCoverage] = 1;
        set_node_probability(bn, Testing, 2, parent_state, 0.7);
        set_node_probability(bn, Testing, 1, parent_state, 0.19);
        set_node_probability(bn, Testing, 0, parent_state, 0.11);

        // (2, 1)
        parent_state[I_ImplementationAccuracy] = 2;
        set_node_probability(bn, Testing, 2, parent_state, 0.85);
        set_node_probability(bn, Testing, 1, parent_state, 0.1);
        set_node_probability(bn, Testing, 0, parent_state, 0.05);

        // (2, 0)
        parent_state[I_CodeCoverage] = 0;
        set_node_probability(bn, Testing, 2, parent_state, 0.1);
        set_node_probability(bn, Testing, 1, parent_state, 0.5);
        set_node_probability(bn, Testing, 0, parent_state, 0.4);

        // (1, 0)
        parent_state[I_ImplementationAccuracy] = 1;
        set_node_probability(bn, Testing, 2, parent_state, 0.2);
        set_node_probability(bn, Testing, 1, parent_state, 0.5);
        set_node_probability(bn, Testing, 0, parent_state, 0.3);

        // (0, 0)
        parent_state[I_ImplementationAccuracy] = 0;
        set_node_probability(bn, Testing, 2, parent_state, 0.1);
        set_node_probability(bn, Testing, 1, parent_state, 0.1);
        set_node_probability(bn, Testing, 0, parent_state, 0.8);

        // (0, 1)
        parent_state[I_CodeCoverage] = 1;
        set_node_probability(bn, Testing, 2, parent_state, 0.15);
        set_node_probability(bn, Testing, 1, parent_state, 0.5);
        set_node_probability(bn, Testing, 0, parent_state, 0.35);

        // (0, 1)
        parent_state[I_CodeCoverage] = 2;
        set_node_probability(bn, Testing, 2, parent_state, 0.3);
        set_node_probability(bn, Testing, 1, parent_state, 0.3);
        set_node_probability(bn, Testing, 0, parent_state, 0.4);
        /////////////////////////





        ///      Understanding ==>  Analysis
        parent_state.clear();
        parent_state.add(Understanding, 2);
        set_node_probability(bn, Analysis, 2, parent_state, 0.55);
        set_node_probability(bn, Analysis, 1, parent_state, 0.35);
        set_node_probability(bn, Analysis, 0, parent_state, 0.1);

        parent_state[Understanding] = 1;
        set_node_probability(bn, Analysis, 2, parent_state, 0.3);
        set_node_probability(bn, Analysis, 1, parent_state, 0.4);
        set_node_probability(bn, Analysis, 0, parent_state, 0.3);

        parent_state[Understanding] = 0;
        set_node_probability(bn, Analysis, 2, parent_state, 0.1);
        set_node_probability(bn, Analysis, 1, parent_state, 0.4);
        set_node_probability(bn, Analysis, 0, parent_state, 0.5);
        ////////////////////////////////


        ///      Testing ==>  QA
        parent_state.clear();
        parent_state.add(Testing, 2);
        set_node_probability(bn, QA, 2, parent_state, 0.5);
        set_node_probability(bn, QA, 1, parent_state, 0.3);
        set_node_probability(bn, QA, 0, parent_state, 0.2);

        parent_state[Testing] = 1;
        set_node_probability(bn, QA, 2, parent_state, 0.2);
        set_node_probability(bn, QA, 1, parent_state, 0.3);
        set_node_probability(bn, QA, 0, parent_state, 0.5);

        parent_state[Testing] = 0;
        set_node_probability(bn, QA, 2, parent_state, 0.1);
        set_node_probability(bn, QA, 1, parent_state, 0.3);
        set_node_probability(bn, QA, 0, parent_state, 0.6);
        ////////////////////////////////

        ///      QA ==>  
        ///                               Maintenance
        ///      Analysis ==> 
        ///      
        ///      Modification ====> 
        parent_state.clear();
        parent_state.add(Analysis, 0);
        parent_state.add(QA, 0);
        parent_state.add(Implementation, 0);

        // (0, 0, 0)
        set_node_probability(bn, Maintenance, 2, parent_state, 0.01);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.1);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.89);

        // (0, 0, 1)
        parent_state[Implementation] = 1;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.1);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.2);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.7);

        // (0, 0, 2)
        parent_state[Implementation] = 2;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.15);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.35);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.5);

        // (0, 1, 0)
        parent_state[QA] = 1;
        parent_state[Implementation] = 0;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.15);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.25);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.6);

        // (0, 1, 1)
        parent_state[Implementation] = 1;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.25);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.25);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.5);

        // (0, 1, 2)
        parent_state[Implementation] = 2;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.25);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.35);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.4);

        // (0, 2, 0)
        parent_state[QA] = 2;
        parent_state[Implementation] = 0;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.3);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.3);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.4);

        // (0, 2, 1)
        parent_state[Implementation] = 1;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.45);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.35);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.2);

        // (0, 2, 2)
        parent_state[Implementation] = 2;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.5);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.4);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.1);


        ///////

        // (1, 0, 0)
        parent_state[Analysis] = 1;
        parent_state[QA] = 0;
        parent_state[Implementation] = 0;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.15);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.25);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.6);

        // (1, 0, 1)
        parent_state[Implementation] = 1;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.15);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.35);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.5);

        // (1, 0, 2)
        parent_state[Implementation] = 2;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.2);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.25);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.55);

        // (1, 1, 0)
        parent_state[QA] = 1;
        parent_state[Implementation] = 0;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.15);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.25);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.6);

        // (1, 1, 1)
        parent_state[Implementation] = 1;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.2);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.15);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.65);

        // (1, 1, 2)
        parent_state[Implementation] = 2;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.2);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.2);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.6);

        // (1, 2, 0)
        parent_state[QA] = 2;
        parent_state[Implementation] = 0;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.2);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.35);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.45);

        // (1, 2, 1)
        parent_state[Implementation] = 1;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.35);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.35);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.3);

        // (1, 2, 2)
        parent_state[Implementation] = 2;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.6);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.3);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.1);



        ///////

        // (2, 0, 0)
        parent_state[Analysis] = 2;
        parent_state[QA] = 0;
        parent_state[Implementation] = 0;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.15);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.25);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.6);

        // (2, 0, 1)
        parent_state[Implementation] = 1;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.15);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.3);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.55);

        // (2, 0, 2)
        parent_state[Implementation] = 2;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.4);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.4);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.2);

        // (2, 1, 0)
        parent_state[QA] = 1;
        parent_state[Implementation] = 0;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.2);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.3);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.5);

        // (2, 1, 1)
        parent_state[Implementation] = 1;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.4);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.3);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.3);

        // (2, 1, 2)
        parent_state[Implementation] = 2;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.5);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.4);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.1);

        // (2, 2, 0)
        parent_state[QA] = 2;
        parent_state[Implementation] = 0;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.5);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.3);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.2);

        // (2, 2, 1)
        parent_state[Implementation] = 1;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.5);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.4);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.1);

        // (2, 2, 2)
        parent_state[Implementation] = 2;
        set_node_probability(bn, Maintenance, 2, parent_state, 0.5);
        set_node_probability(bn, Maintenance, 1, parent_state, 0.4);
        set_node_probability(bn, Maintenance, 0, parent_state, 0.1);



        //////////////////++++++++++

        // We have now finished setting up our bayesian network.  So let's compute some 
        // probability values.  The first thing we will do is compute the prior probability
        // of each node in the network.  To do this we will use the join tree algorithm which
        // is an algorithm for performing exact inference in a bayesian network.   

        // First we need to create an undirected graph which contains set objects at each node and
        // edge.  This long declaration does the trick.
        typedef dlib::set<unsigned long>::compare_1b_c set_type;
        typedef graph<set_type, set_type>::kernel_1a_c join_tree_type;
        join_tree_type join_tree;

        // Now we need to populate the join_tree with data from our bayesian network.  The next  
        // function calls do this.  Explaining exactly what they do is outside the scope of this
        // example.  Just think of them as filling join_tree with information that is useful 
        // later on for dealing with our bayesian network.  
        create_moral_graph(bn, join_tree);
        create_join_tree(join_tree, join_tree);

        // Now that we have a proper join_tree we can use it to obtain a solution to our
        // bayesian network.  Doing this is as simple as declaring an instance of
        // the bayesian_network_join_tree object as follows:
        bayesian_network_join_tree solution(bn, join_tree);


        //// now print out the probabilities for each node
        //cout << "Using the join tree algorithm:\n";
        //cout << "p(A=1) = " << solution.probability(A)(1) << endl;
        //cout << "p(A=0) = " << solution.probability(A)(0) << endl;
        //cout << "p(B=1) = " << solution.probability(B)(1) << endl;
        //cout << "p(B=0) = " << solution.probability(B)(0) << endl;
        //cout << "p(C=1) = " << solution.probability(C)(1) << endl;
        //cout << "p(C=0) = " << solution.probability(C)(0) << endl;
        //cout << "p(D=1) = " << solution.probability(D)(1) << endl;
        //cout << "p(D=0) = " << solution.probability(D)(0) << endl;
        //cout << "\n\n\n";


        // Now to make things more interesting let's say that we have discovered that the C 
        // node really has a value of 1.  That is to say, we now have evidence that 
        // C is 1.  We can represent this in the network using the following two function
        // calls.

        if (mp_ui->mp_use_coupling->isChecked())
        {
            set_node_value(bn, I_Coupling, coupling());
            set_node_as_evidence(bn, I_Coupling);
        }

        if (mp_ui->mp_use_comment->isChecked())
        {
            set_node_value(bn, I_CommentRatio, comment_ratio());
            set_node_as_evidence(bn, I_CommentRatio);
        }

        if (mp_ui->mp_use_cyclomatic->isChecked())
        {
            set_node_value(bn, I_ImplementationAccuracy, cyclomatic_complexity());
            set_node_as_evidence(bn, I_ImplementationAccuracy);
        }

        if (mp_ui->mp_use_testcoverage->isChecked())
        {
            set_node_value(bn, I_CodeCoverage, code_coverage());
            set_node_as_evidence(bn, I_CodeCoverage);
        }

        if (mp_ui->mp_use_module_size->isChecked())
        {
            set_node_value(bn, I_ModuleSize, average_module_size());
            set_node_as_evidence(bn, I_ModuleSize);
        }

        if (mp_ui->mp_use_dsqi->isChecked())
        {
            set_node_value(bn, I_DesignQuality, dsqi());
            set_node_as_evidence(bn, I_DesignQuality);
        }

        if (mp_ui->mp_use_lcom4->isChecked())
        {
            set_node_value(bn, I_Cohesion, cohesion());
            set_node_as_evidence(bn, I_Cohesion);
        }



        // Now we want to compute the probabilities of all the nodes in the network again
        // given that we now know that C is 1.  We can do this as follows:
        bayesian_network_join_tree solution_with_evidence(bn, join_tree);

        // now print out the probabilities for each node
        cout << "Using the join tree algorithm:\n";
        mp_ui->mp_results->setPlainText(
            QString::fromStdWString(L"Низької якості:      ") + QString::number(100 * solution_with_evidence.probability(Maintenance)(0)) + " %\n" +
            QString::fromStdWString(L"Середньої якості: ") + QString::number(100 * solution_with_evidence.probability(Maintenance)(1)) + " %\n" +
            QString::fromStdWString(L"Високої якості:      ") + QString::number(100 * solution_with_evidence.probability(Maintenance)(2)) + " %");


        // Note that when we made our solution_with_evidence object we reused our join_tree object.
        // This saves us the time it takes to calculate the join_tree object from scratch.  But
        // it is important to note that we can only reuse the join_tree object if we haven't changed
        // the structure of our bayesian network.  That is, if we have added or removed nodes or 
        // edges from our bayesian network then we must recompute our join_tree.  But in this example
        // all we did was change the value of a bayes_node object (we made node C be evidence)
        // so we are ok.





        // Next this example will show you how to use the bayesian_network_gibbs_sampler object
        // to perform approximate inference in a bayesian network.  This is an algorithm 
        // that doesn't give you an exact solution but it may be necessary to use in some 
        // instances.  For example, the join tree algorithm used above, while fast in many
        // instances, has exponential runtime in some cases.  Moreover, inference in bayesian
        // networks is NP-Hard for general networks so sometimes the best you can do is
        // find an approximation.
        // However, it should be noted that the gibbs sampler does not compute the correct
        // probabilities if the network contains a deterministic node.  That is, if any
        // of the conditional probability tables in the bayesian network have a probability
        // of 1.0 for something the gibbs sampler should not be used.


        // This Gibbs sampler algorithm works by randomly sampling possibles values of the
        // network.  So to use it we should set the network to some initial state.  

        //set_node_value(bn, A, 0);
        //set_node_value(bn, B, 0);
        //set_node_value(bn, D, 0);

        // We will leave the C node with a value of 1 and keep it as an evidence node.  
        /*

            // First create an instance of the gibbs sampler object
            bayesian_network_gibbs_sampler sampler;


            // To use this algorithm all we do is go into a loop for a certain number of times
            // and each time through we sample the bayesian network.  Then we count how
            // many times a node has a certain state.  Then the probability of that node
            // having that state is just its count/total times through the loop.

            // The following code illustrates the general procedure.
            unsigned long A_count = 0;
            unsigned long B_count = 0;
            unsigned long C_count = 0;
            unsigned long D_count = 0;

            unsigned long
            count_Maintenance_high,
            count_Maintenance_middle,
            count_Maintenance_low;




            // The more times you let the loop run the more accurate the result will be.  Here we loop
            // 2000 times.
            const long rounds = 20000;
            for (long i = 0; i < rounds; ++i)
            {
            sampler.sample_graph(bn);

            if (node_value(bn, Maintenance) == 2)
            ++count_Maintenance_high;
            }

            cout << "Using the approximate Gibbs Sampler algorithm:\n";

            cout << "p(Maintanence = high) = " << (double)count_Maintenance_high / (double)rounds << endl;
            //cout << "p(Maintanence = middle) = " << (double)count_Maintenance_middle / (double)rounds << endl;
            //cout << "p(Maintanence = low) = " << (double)count_Maintenance_low / (double)rounds << endl;

            */
        auto end = std::chrono::system_clock::now();

        cout << std::chrono::duration<double>(end - start).count() << endl;
#ifndef _DEBUG
    }
    catch (...)
    {
        mp_ui->mp_results->setPlainText("Internal error");
    }
#endif
}
