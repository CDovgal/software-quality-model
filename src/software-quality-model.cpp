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
#include <Windows.h>
#include <functional>
//#include "SEL.h"

#using "D:/NET4.0/BayesServer.dll"
#using "D:/NET4.0/BayesServer.Inference.dll"

using namespace BayesServer;
using namespace BayesServer::Inference;
using namespace BayesServer::Inference::RelevanceTree;


SQM_client::SQM_client(QWidget* ip_parent)
    : QMainWindow(ip_parent)
    , mp_ui(std::make_shared<Ui::SQM_client>())
{
    mp_ui->setupUi(this);

    auto b = connect(mp_ui->mp_button_build, &QPushButton::clicked, this, &SQM_client::build_network, Qt::BlockingQueuedConnection);

    int a = 0;
}

void SQM_client::build_network(bool)
{ 
    
   
    auto network = gcnew Network("Demo");

    //add the nodes (variables)

    auto aTrue = gcnew State("True");
    auto aFalse = gcnew State("False");
    auto a = gcnew Node("A", aTrue, aFalse);

    auto bTrue = gcnew State("True");
    auto bFalse = gcnew State("False");
    auto b = gcnew Node("B", bTrue, bFalse);

    auto cTrue = gcnew State("True");
    auto cFalse = gcnew State("False");
    auto c = gcnew Node("C", cTrue, cFalse);

    auto dTrue = gcnew State("True");
    auto dFalse = gcnew State("False");
    auto d = gcnew Node("D", dTrue, dFalse);

    network->Nodes->Add(a);
    network->Nodes->Add(b);
    network->Nodes->Add(c);
    network->Nodes->Add(d);

    // add some directed links

    network->Links->Add(gcnew Link(a, b));
    network->Links->Add(gcnew Link(a, c));
    network->Links->Add(gcnew Link(b, d));
    network->Links->Add(gcnew Link(c, d));

    // at this point we have fully specified the structural (graphical) specification of the Bayesian Network.

    // We must define the necessary probability distributions for each node.

    // Each node in a Bayesian Network requires a probability distribution conditioned on it's parents.

    // NewDistribution() can be called on a Node to create the appropriate probability distribution for a node
    // or it can be created manually.

    // The interface IDistribution has been designed to represent both discrete and continuous autoiables,

    // As we are currently dealing with discrete distributions, we will use the
    // Table class.

    // To access the discrete part of a distribution, we use IDistribution.Table.

    // The Table class is used to define distributions over a number of discrete variables.

    auto tableA = a->NewDistribution()->Table;     // access the table property of the Distribution

    // IMPORTANT
    // Note that calling Node.NewDistribution() does NOT assign the distribution to the node.
    // A distribution cannot be assigned to a node until it is correctly specified.
    // If a distribution becomes invalid  (e.g. a parent node is added), it is automatically set to null.

    array<BayesServer::State^>^ array_temp = gcnew array<BayesServer::State^>(1);

    array_temp[0] = aTrue;
    tableA[array_temp] = 0.1;

    array_temp[0] = aFalse;
    tableA[array_temp] = 0.9;

    // now tableA is correctly specified we can assign it to Node A;
    a->Distribution = tableA;


    // node B has node A as a parent, therefore its distribution will be P(B|A)

    auto tableB = b->NewDistribution()->Table;

    array_temp = gcnew array<BayesServer::State^>(2);

    array_temp[0] = aTrue;
    array_temp[1] = bTrue;
    tableB[array_temp] = 0.2;


    array_temp[0] = aTrue;
    array_temp[1] = bFalse;
    tableB[array_temp] = 0.8;

    array_temp[0] = aFalse;
    array_temp[1] = bTrue;

    tableB[array_temp] = 0.15;


    array_temp[0] = aFalse;
    array_temp[1] = bFalse;
    tableB[array_temp] = 0.85;

    b->Distribution = tableB;


    // specify P(C|A)
    auto tableC = c->NewDistribution()->Table;

    array_temp[0] = aTrue;
    array_temp[1] = cTrue;
    tableC[array_temp] = 0.3;

    array_temp[0] = aTrue;
    array_temp[1] = cFalse;
    tableC[array_temp] = 0.7;

    array_temp[0] = aFalse;
    array_temp[1] = cTrue;
    tableC[array_temp] = 0.4;

    array_temp[0] = aFalse;
    array_temp[1] = cFalse;
    tableC[array_temp] = 0.6;
    c->Distribution = tableC;


    // specify P(D|B,C)
    auto tableD = d->NewDistribution()->Table;

    // we could specify the values individually as above, or we can use a TableIterator as follows
    System::Collections::Generic::IList<Node^>^ array_temp2 = gcnew System::Collections::Generic::List<Node^>(3);
    array_temp2[0] = b;
    array_temp2[1] = c;
    array_temp2[2] = d;
    auto iteratorD = gcnew TableIterator(tableD, array_temp2);

    array<double>^ array_temp3 = gcnew array<double>(8);
    array_temp3[0] = 0.4;
    array_temp3[1] = 0.6;
    array_temp3[2] = 0.55;
    array_temp3[3] = 0.45;
    array_temp3[4] = 0.32;
    array_temp3[5] = 0.68;
    array_temp3[6] = 0.01;
    array_temp3[7] = 0.99;

    iteratorD->CopyFrom(array_temp3);
    d->Distribution = tableD;


    // The network is now fully specified

    // If required the network can be saved...

    if (false)   // change this to true to save the network
    {
        network->Save("fileName.bayes");  // replace 'fileName.bayes' with your own path
    }

    // Now we will calculate P(A|D=True), i.e. the probability of A given the evidence that D is true

    // use the factory design pattern to create the necessary inference related objects
    auto factory = gcnew RelevanceTreeInferenceFactory();
    auto inference = factory->CreateInferenceEngine(network);
    auto queryOptions = factory->CreateQueryOptions();
    auto queryOutput = factory->CreateQueryOutput();

    // we could have created these objects explicitly instead, but as the number of algorithms grows
    // this makes it easier to switch between them

    inference->Evidence->SetState(dTrue);  // set D = True

    auto queryA = gcnew Table(a);
    inference->QueryDistributions->Add(queryA);
    inference->Query(queryOptions, queryOutput); // note that this can raise an exception (see help for details)

    //Console.WriteLine("P(A|D=True) = {" + queryA[aTrue] + "," + queryA[aFalse] + "}.");

    // Expected output ...
    // P(A|D=True) = {0.0980748663101604,0.90192513368984}

    // to perform another query we reuse all the objects

    // now lets calculate P(A|D=True, C=True)
    inference->Evidence->SetState(cTrue);

    // we will also return the log-likelihood of the case
    queryOptions->LogLikelihood = true; // only request the log-likelihood if you really need it, as extra computation is involved

    inference->Query(queryOptions, queryOutput);
    //Console.WriteLine(string.Format("P(A|D=True, C=True) = {{{0},{1}}}, log-likelihood = {2}.", queryA[aTrue], queryA[aFalse], queryOutput.LogLikelihood.Value));


    array<BayesServer::State^>^ array_temp4 = gcnew array<BayesServer::State^>(1);
    array_temp4[0] = aTrue;
    //return queryA[array_temp4];
    mp_ui->mp_label_result->setText(QString::number(queryA[array_temp4]) + " Done!");
}

SQM_client::~SQM_client()
{}
