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
double dfsDist[11] = {0.0};

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

    //Add node alternate constructor
    void addNode(int n)
    {
        node *tmp = new node;
        tmp->id = n;
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

    //Display function
    static void displayQueue(node *head)
    {;
        if(head == NULL)
        {
            cout << "NULL" << endl;
        }
        else
        {
            printf("%d->", head->id);
            if(head->next != NULL) displayQueue(head->next);
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

    //Remove head of List
    void removeHead()
    {
        node* tmp = head;
        if (head != NULL)
        {
            head = head->next;
            delete tmp;
        }
        else
        {
            head = NULL;
            tail = NULL;
        }   
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

//Function to initialize hardcoded project 2 adjacency matrix
void adjMtx(List list, double array[][11])
{
    node *a = list.getHead();
    node *b = list.getHead();
    node* itr = list.getHead();
    int i,j,count = 0;

    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 11; j++)
        {
            array[i][j] = 0.0;
        }
    }

    //Set path disconnection indicators used in following loop
    array[0][4] = -1.0;
    array[1][3] = -1.0;
    array[2][5] = -1.0;
    array[3][7] = -1.0;
    array[4][8] = -1.0;
    array[5][8] = -1.0;
    array[6][10] = -1.0;
    

    //Set hardcoded distances for paths
    for (i = 0; i < 10; i++)
    {
        b = list.getHead();

        for (j = 0; j < 11; j++)
        {
            if (array[i][j] != -1.0)
            {
                array[i][j] = getDistance(a,b);
                b = b->next;
            }
            else{
                break;
            }
        }
        a = a->next;
    }

    //Correct false paths
    array[0][4] = 0.0;
    array[1][3] = 0.0;
    array[2][5] = 0.0;
    array[3][7] = 0.0;
    array[4][8] = 0.0;
    array[4][5] = 0.0;
    array[5][6] = 0.0;
    array[5][8] = 0.0;
    array[6][7] = 0.0;
    array[6][10] = 0.0;
    array[8][9] = 0.0;

    //Correct redundant paths
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 11; j++)
        {
            while (array[i][j] != 0.0)
            {
                array[i][j] = 0.0;
                j++;
            }
            break;
        }
    }
}

//Breadth-first search algorithm to find least number of hops
void BFSjumps(double array[][11])
{
    int i,j = 0;
    //Discovered location queue
    List q;
    //Color of location
    int color[11] = {0};
    //Number of jumps along path
    int jumps[11] = {0};
    //Preceding location
    int pre[11] = {0};

    q.addNode(0);
    color[0] = 1;

    //Execute through queue
    while (q.getHead() != NULL)
    {
        i = q.getHead()->id;
        //printf("i = %d\n", i);
        q.removeHead();

        if (i < 10)
        {
            //For all paths from node
            for (j = 0; j < 11; j++)
            {
                //If a path exists
                if (array[i][j] != 0.0)
                {
                    //If new node has not been discovered
                    if (color[j] == 0)
                    {
                        color[j] = 1;
                        pre[j] = i;
                        jumps[j] = jumps[i] + 1;
                        q.addNode(j);
                    }
                    //If node has been discovered
                    else
                    {
                        if (jumps[i] < jumps[pre[j]])
                        {
                            pre[j] = i;
                            jumps[j] = jumps[i] + 1;
                        }
                    }
                }
            } 
            //q.displayQueue(q.getHead());
        }
    }   

    printf("%d->%d->%d->%d->11\n", pre[pre[pre[pre[10]]]]+1, pre[pre[pre[10]]]+1,pre[pre[10]]+1, pre[10]+1);
    printf("Number of jumps: %d\n", jumps[10]);
}

//Breadth-first search algorithm to find total distance
void BFSdist(double array[][11])
{
    int i,j = 0;
    //Discovered location queue
    List q;
    //Color of location
    int color[11] = {0};
    //Number of jumps along path
    double dist[11] = {0};
    //Preceding location
    int pre[11] = {0};

    q.addNode(0);
    color[0] = 1;

    //Execute through queue
    while (q.getHead() != NULL)
    {
        i = q.getHead()->id;
        //printf("i = %d\n", i);
        q.removeHead();

        if (i < 10)
        {
            //For all paths from node
            for (j = 0; j < 11; j++)
            {
                //If a path exists
                if (array[i][j] != 0.0)
                {
                    //If new node has not been discovered
                    if (color[j] == 0)
                    {
                        color[j] = 1;
                        pre[j] = i;
                        dist[j] = dist[i] + array[i][j];
                        q.addNode(j);
                    }
                    //If node has been discovered
                    else
                    {
                        if ((dist[i] + array[i][j]) < dist[j])
                        {
                            pre[j] = i;
                            dist[j] = dist[i] + array[i][j];
                        }
                    }
                }
            } 
            //q.displayQueue(q.getHead());
        }
    }   

    printf("%d->%d->%d->%d->%d->11\n", pre[pre[pre[pre[pre[10]]]]]+1, pre[pre[pre[pre[10]]]]+1, pre[pre[pre[10]]]+1,pre[pre[10]]+1, pre[10]+1);
    printf("Total distance: %.2f\n", dist[10]);
}

//Depth-first search algorithm to find show path to end location
void DFS(double array[][11], int visited[11], int start)
{
    if (visited[10] == 0) printf("%d->", start+1);
    int i = 0;
    visited[start] = 1;
    for (i = 0; i < 11; i++)
    {
        if(array[start][i] != 0.0 && visited[i] == 0)
        {
            dfsDist[i] = dfsDist[start] + array[start][i];
            DFS(array, visited, i);
        }
    }
}

//Depth-first search algorithm to find show full path
void DFSfull(double array[][11], int visited[11], int start)
{
    printf("%d->", start+1);
    int i = 0;
    visited[start] = 1;
    for (i = 0; i < 11; i++)
    {
        if(array[start][i] != 0.0 && visited[i] == 0)
        {
            dfsDist[i] = dfsDist[start] + array[start][i];
            DFSfull(array, visited, i);
        }
    }
}

//Main method
int main(int argc, char *argv[])
{
    //Create location List
    List locations;

    //Open tsp file
    string fname = argv[1];
    ifstream tsp;
    tsp.open(fname);

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

    //Initialize location directional adjacency matrix w/ 10 rows and 11 columns
    double arr[10][11];
    adjMtx(locations, arr);

    using namespace std::chrono;

    printf("\nBFS to find minimum jumps:\n");
    high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    BFSjumps(arr);
    high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count() << " us" << endl;
    
    printf("\nBFS to find minimum distance:\n");
    high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();
    BFSdist(arr);
    high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count() << " us" << endl;
    
    int color[11] = {0};
    printf("\nDFS path:\n");
    high_resolution_clock::time_point t5 = std::chrono::high_resolution_clock::now();
    DFS(arr, color, 0);
    high_resolution_clock::time_point t6 = std::chrono::high_resolution_clock::now();
    printf("\n");
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t6-t5).count() << " us" << endl;
    printf("Total Distance: %.2f\n", dfsDist[10]);
    
    printf("\nFull DFS search path: ");
    int visited[11] = {0};
    high_resolution_clock::time_point t7 = std::chrono::high_resolution_clock::now();
    DFSfull(arr, visited, 0);
    high_resolution_clock::time_point t8 = std::chrono::high_resolution_clock::now();
    printf("\n");
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t8-t7).count() << " us" << endl;
    printf("\n");


    /* //Display adjacency matrix
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            printf("%.2f\t", arr[i][j]);
        }
        printf("\n");
    } */

    /* //Determine optimal path through brute force procedure and measure execution time
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
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(done-start).count() << " ms" << endl; */

    tsp.close();
    
    return 0;
}