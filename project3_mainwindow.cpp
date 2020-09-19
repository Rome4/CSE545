#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QLabel>
#include <vector>
#include <vector>
#include <QString>
#include <math.h>
#include <algorithm>

using namespace::std;

vector<int> locID;
vector<double> xLoc;
vector<double> yLoc;
vector<int> inPath;
vector<int>addOrder;

int nextl = 0;
int flag = 0;
double small = 9999.99;

//Filename global variable
QString fileName = "C:/Users/MillemRomanLee/Documents/Classes/Fall 20/AI/Project3/Random40.tsp";

//FLAG = 2 FOR PATH FINDING POINTS CLOSEST TO ANY EDGE IN THE PATH
//FLAG = 3 FOR PATH FINDING POINTS CLOSEST TO CURRENT POINT
static int globalFlag = 3;

//Initial starting point global variable
//In terms of index, will be 1 less than location id
//int init = 0;

//If projected point is in line segment
bool isProjInSeg(QPoint p0, QPoint p1, QPoint p2)
{
    double slope = (p1.y()-p0.y()) / (p1.x()-p0.x());
    double r0 = p0.x() + (slope*p0.y());
    double r1 = p1.x() + (slope*p1.y());
    double r2 = p2.x() + (slope*p2.y());

    return r0 < r1 ? (r0 <= r2 && r2 <= r1) : (r1 <= r2 && r2 <= r0);
}

//Distance calulation function
double getDistancePointLine(QPoint p0, QPoint p1, QPoint p2)
{

    double slope = (p1.y()-p0.y()) / (p1.x()-p0.x());
    double intercept = p1.y() - (slope * p1.x());
    double a = slope * -1;
    double b = 1;
    double c = intercept * -1;

    double numerator = abs( (a*p2.x() + b*p2.y() + c) );
    double denominator = sqrt( pow(a,2) + pow(b,2) );
    double distance = numerator / denominator;

    return distance;
}

//Distance calulation function
double getDistance(double x1, double y1, double x2, double y2)
{
    double distance;
    double d1 = pow((x2 - x1),2);
    double d2 = pow((y2 - y1),2);
    distance = sqrt(d1 + d2);

    return distance;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->labelPath->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    ui->labelPath->setWordWrap(true);
    ui->labelDist->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    ui->labelDist->setWordWrap(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p( this );    
    int init;               //Initial Node, can be replaced by hardcoded global variable
    if(flag == 0)
    {
        // Read from file
        QFile inputFile(fileName);
        if (inputFile.open(QIODevice::ReadOnly))
        {
           QTextStream in(&inputFile);
           while (!in.atEnd())
           {
              QString line = in.readLine();
              if (line == "NODE_COORD_SECTION")
              {
                  while (!in.atEnd())
                  {
                      int id;
                      double x,y;
                      in >> id;
                      in >> x;
                      in >> y;

                      if(in.atEnd()) break;
                      locID.push_back(id);
                      xLoc.push_back(x);
                      yLoc.push_back(y);
                  }
              }
           }
           inputFile.close();
        }

        //p.scale(width()/100, height()/100);

        p.setWindow( -10, -10, 110, 110 );
        p.scale( 1.0, -1.0 );
        p.setPen(QPen(Qt::darkBlue,1));

        double originDist;
        for (int i=0; i < (int)locID.size(); i++)
        {
            QPoint rec(xLoc[i],yLoc[i]);
            //Find initial node by calculating closest to origin
            //Want a corner as initial to find the optimal path from an extremity
            originDist = getDistance(xLoc[i],yLoc[i],0,0);
            if(originDist < small)
            {
                init = i;
                small = originDist;
            }

            p.drawText(rec, QString::number(i+1));
        }

        //Add initial location to path
        inPath.push_back(init);
        addOrder.push_back(init);

        flag = 1;
        small = 9999.99;
    }
    else if(flag == 1)
    {
        //
    }
    //Modified heuristic. Edges.
    else if(flag == 2)
    {
        //p.scale(width()/100, height()/100);
        //p.setWindow( -5, 5, 110, -110 );
        p.scale( 6.0, 6.0 );

        p.setPen(QPen(Qt::darkBlue,1));
        QFont font = p.font();
        font.setPointSize(2);
        p.setFont(font);

        int closest;
        int i,j;

        //Redraw all locations
        for (i=0; i < (int)locID.size(); i++)
        {
            QPoint rec(xLoc[i],yLoc[i]);
            p.drawText(rec,QString::number(i+1));
        }

        //Finding closest point to initial point
        if((int)inPath.size() == 1)
        {
            for(i = 0; i < (int)locID.size(); i++)
            {
                double dist = getDistance(xLoc[inPath[0]],yLoc[inPath[0]],xLoc[i],yLoc[i]);
                if(dist < small && std::count(inPath.begin(),inPath.end(),i) == 0)
                {
                    small = dist;
                    nextl = i;
                    closest = inPath[0];
                }
            }
            inPath.push_back(nextl);
            addOrder.push_back(nextl);
        }
        //Finding closest point to an existing edge
        else if((int)inPath.size() != (int)locID.size())
        {

            for(i = 0; i < (int)locID.size(); i++)
            {
                if(std::count(inPath.begin(),inPath.end(),i) == 0)
                {
                    QPoint p2(xLoc[i], yLoc[i]);

                    for(j = 1; j < (int)inPath.size(); j++)
                    {
                        QPoint p0(xLoc[inPath[j-1]],yLoc[inPath[j-1]]);
                        QPoint p1(xLoc[inPath[j]],yLoc[inPath[j]]);

                        //Checking
                        if(isProjInSeg(p0,p1,p2) || isProjInSeg(p1,p0,p2))
                        {
                            double dist = getDistancePointLine(p0,p1,p2);
                            if(dist < small)
                            {
                                small = dist;
                                nextl = i;
                                closest = j;
                            }
                        }
                        else
                        {
                            double dist1 = getDistance(p0.x(),p0.y(),p2.x(),p2.y());
                            double dist2 = getDistance(p1.x(),p1.y(),p2.x(),p2.y());
                            double dist;
                            if (dist1 < dist2) dist = dist1;
                            else dist = dist2;
                            if(dist < small && std::count(inPath.begin(),inPath.end(),i) == 0)
                            {
                                small = dist;
                                nextl = i;
                                closest = j;
                            }
                        }
                    }
                }
            }

            std::vector<int>::iterator it = inPath.begin() + (closest);
            inPath.insert(it,nextl);
            addOrder.push_back(nextl);
        }

        p.setPen(QPen(Qt::lightGray,1));

        for(i = 1; i < (int)inPath.size(); i++)
        {
            QPoint a(xLoc[inPath[i-1]],yLoc[inPath[i-1]]);
            QPoint b(xLoc[inPath[i]],yLoc[inPath[i]]);
            p.drawLine(a,b);
        }

        if ((int)inPath.size()>=(int)locID.size())
        {
            QPoint a(xLoc[inPath.front()],yLoc[inPath.front()]);
            QPoint b(xLoc[inPath.back()],yLoc[inPath.back()]);
            p.drawLine(a,b);
        }

        flag = 1;
        small = 9999.99;
    }
    else if(flag == 3)
    {
        //p.setWindow( -5, 5, 110, -110 );
        p.scale( 6.0, 6.0 );
        p.setPen(QPen(Qt::darkBlue,1));
        QFont font = p.font();
        font.setPointSize(2);
        p.setFont(font);

        int i;

        //Redraw all locations
        for (i=0; i < (int)locID.size(); i++)
        {
            QPoint rec(xLoc[i],yLoc[i]);
            p.drawText(rec,QString::number(i+1));
        }

        //Find next location
        if((int)inPath.size() != (int)locID.size())
        {
            for(int i = 0; i < (int)locID.size(); i++)
            {

                double dist = getDistance(xLoc[inPath.back()],yLoc[inPath.back()],xLoc[i],yLoc[i]);
                if(dist < small && std::count(inPath.begin(),inPath.end(),i) == 0)
                {
                    small = dist;
                    nextl = i;
                }

            }

            inPath.push_back(nextl);
            addOrder.push_back(nextl);
        }

        p.setPen(QPen(Qt::lightGray,1));

        for(i = 1; i < (int)inPath.size(); i++)
        {
            QPoint a(xLoc[inPath[i-1]],yLoc[inPath[i-1]]);
            QPoint b(xLoc[inPath[i]],yLoc[inPath[i]]);
            p.drawLine(a,b);
        }

        if ((int)inPath.size()>=(int)locID.size())
        {
            QPoint a(xLoc[inPath.front()],yLoc[inPath.front()]);
            QPoint b(xLoc[inPath.back()],yLoc[inPath.back()]);
            p.drawLine(a,b);
        }


        flag = 1;
        small = 9999.99;
    }
}


void MainWindow::on_pushButton_clicked()
{

    flag = globalFlag;
    repaint();

}


void MainWindow::on_pushButton_Tour_clicked()
{
    //Display Route and Total Distance
    QString displayString = "Best Path: ";
    double totalDistance = 0;
    int i;
    for(i = 0; i < (int)inPath.size(); i++)
    {
        displayString.append(QString::number((inPath[i]+1)) + "->");
        if(i > 0)
        {
            totalDistance += getDistance(xLoc[inPath[i-1]],yLoc[inPath[i-1]],xLoc[inPath[i]],yLoc[inPath[i]]);
        }
    }
    displayString.append(QString::number(inPath[0]+1));
    totalDistance += getDistance(xLoc[inPath.back()],yLoc[inPath.back()],xLoc[inPath[0]],yLoc[inPath[0]]);
    ui->labelPath->setText(displayString);
    ui->labelDist->setText("Distance: " + QString::number(totalDistance));

}

void MainWindow::on_pushButton_Order_clicked()
{
    //Display add order and Total Distance
    QString displayString = "Add Order: ";
    double totalDistance = 0;
    int i;
    for(i = 0; i < (int)inPath.size(); i++)
    {
        displayString.append(QString::number((addOrder[i]+1)) + "->");
        if(i > 0)
        {
            totalDistance += getDistance(xLoc[inPath[i-1]],yLoc[inPath[i-1]],xLoc[inPath[i]],yLoc[inPath[i]]);
        }
    }
    displayString.append(QString::number(addOrder[0]+1));
    totalDistance += getDistance(xLoc[inPath.back()],yLoc[inPath.back()],xLoc[inPath[0]],yLoc[inPath[0]]);
    ui->labelPath->setText(displayString);
    ui->labelDist->setText("Distance: " + QString::number(totalDistance));
}
