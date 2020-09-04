#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <chrono>

using namespace std;

//Global optimum path variables
double minDist = 999999999999.99;
int num;
vector<int> optPath(num);

// Location structure
struct node
{
    int id;
    double x;
    double y;
    struct node *next;
};

// Linked List structure to hold locations
class List
{
private: 
    node *head, *tail;

public:

    //Default constructor
    List()
    {
        head = NULL;
        tail = NULL;
    }

    //Add node
    void addNode(int n, double xloc, double yloc)
    {
        node *tmp = new node;
        tmp->id = n;
        tmp->x = xloc;
        tmp->y = yloc;
        tmp->next = NULL;

        if(head == NULL)
        {
            head = tmp;
            tail = tmp;
        }
        else
        {
            tail->next = tmp;
            tail = tail->next;
        }
    }

    //Get function for head of List
    node* getHead()
    {
        return head;
    }

    //Get function for head of List
    node* getTail()
    {
        return tail;
    }

    //Display function
    static void display(node *head)
    {;
        if(head == NULL)
        {
            cout << "NULL" << endl;
        }
        else
        {
            printf("%d\t%f\t%f\n", head->id, head->x, head->y);
            if(head->next != NULL) display(head->next);
        }
    }

    //Insert at front of List
    void front(int n, double xloc, double yloc)
    {
        node *tmp = new node;
        tmp->id = n;
        tmp->x = xloc;
        tmp->y = yloc;
        tmp -> next = head;
        head = tmp;
    }

    //Insert after a node in List
    void after(node *a, int n, double xloc, double yloc)
    {
        node* tmp = new node;
        tmp->id = n;
        tmp->x = xloc;
        tmp->y = yloc;
        tmp->next = a->next;
        a->next = tmp;
    }
};

//Distance calulation function
double getDistance(node *a, node *b)
{
    double distance;
    double d1 = pow((b->x - a->x),2);
    double d2 = pow((b->y - a->y),2);
    distance = sqrt(d1 + d2);

    return distance;
}

//Function to swap index values
void swap(vector<int> &v, int a, int b)
{
    int tmp;
    tmp = v[a];
    v[a] = v[b];
    v[b] = tmp;
}

//Get the next permutation and return permutation total distance
void nextPermutation(List l, vector<int> &v, int n, int i){
    double dist = 0;
    int j;
    node *a = l.getHead();
    node *b = l.getHead();

    //If at end of vector
    if (i == n)
    {
        //Send nodes to get distance function in current permutation order
        for(j = 0; j < n - 1; j++)
        {
            a = l.getHead();
            b = l.getHead();

            while(a->id != v[j]) {a = a->next;}
            while(b->id != v[j+1]) {b = b->next;}
            dist += getDistance(a,b);
        }

        //Add distance from last location to starting location
        a = b;
        b = l.getHead();
        while(b->id != v[0]) {b = b->next;}
        dist += getDistance(a,b);
        
        //Update global min distance if neccessary and save order
        if(dist < minDist)
        {
            minDist = dist;
            optPath = v;
        } 
    }
    else
    {
        for (j = i; j < n; j++)
        {
            swap(v, i, j);
            nextPermutation(l, v, n, i + 1);

            swap (v, i, j);
        }
    }
}

//Brute force method
void bruteForce(List l)
{
    //Declate function variables
    int n = l.getTail()->id;        //Number of locations
    num = n;                        //Update global size variable
    List lnew = l;                  //Create copy of list for manipulation
    vector<int> arr(n);             //Location id vector

    for (int i = 0; i < n; i++)     //Initialze order vector
    {
        arr[i] = i + 1;
    }

    //Calculate distance for all following possible permutations
    nextPermutation(lnew, arr, n, 0);

}

//Main method
int main()
{
    //Create location List
    List locations;

    //Open tsp file
    ifstream tsp;
    tsp.open("Random12.tsp");

    //Error check if file opens
    if(!tsp.is_open())
    {
        cerr << "Failed to open file\n";
        return 1;
    }
    //Read file for coordinates
    else{
        for(string str; getline(tsp, str);)
        {
            //Loop until coordinates are reached
            if(str.compare("NODE_COORD_SECTION") == 0)
            {
                //Loop until EOF inserting locations into linked List
                while(!tsp.eof())
                {
                    int id;
                    double x,y; 
                    tsp >> id;
                    tsp >> x;
                    tsp >> y;

                    //Check for EOF after reads to avoid reading last line twice
                    if(tsp.eof()) break;

                    locations.addNode(id,x,y); 
                }
            }
        }
    }

    //Determine optimal path through brute force procedure and measure execution time
    auto start = std::chrono::high_resolution_clock::now();
    bruteForce(locations);
    auto done = std::chrono::high_resolution_clock::now();

    printf("Min distance: %f\n", minDist);
    printf("Optimal path: ");
    for (int i = 0; i < num; i++)
    {
        printf("%d->", optPath[i]);
    }
    printf("%d\n", optPath[0]);
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(done-start).count() << " ms" << endl;

    tsp.close();
    
    return 0;
}
