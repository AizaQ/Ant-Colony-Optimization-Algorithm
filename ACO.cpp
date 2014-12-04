#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
using namespace std;
#define MAX_Ants 15     //We are going to use all these ants, at least 25
#define MAX_Cities 20   //We will still be testing for 4, 5 or 6 cities only
#define NOTHING -1
#undef INFINITY
#define INFINITY 32765

//int how_many_paths(int map[MAX_Cities][], int v);

double alpha = -1.5;
double beta = 1.1;
double Pheromone_Decay_Factor=0.1;
double Pheromone_Bonus_Factor=0.15;
//Take no. of cities in country
int no_of_cities;
//Take the map and distance of each city from each city
int map_of_city[MAX_Cities][MAX_Cities]={0};

struct Best_tour;

class Ant
{
    private:

        int generate_next_city();
        bool Travel_On();

    public:
        int cities_travelled;
        int what_have_i_travelled[MAX_Cities];
        double Tour_value;
        static double pheromone[MAX_Cities][MAX_Cities];
        Ant()
        {
            cities_travelled=0;
            Tour_value=0;
        }
        Ant(Ant &a)
        {
            cities_travelled=a.cities_travelled;
            for(int i=0; i<cities_travelled; i++)
                what_have_i_travelled[i]=a.what_have_i_travelled[i];
        }


        bool live_your_tour();
        void add_initial_city(int k)
        {
            what_have_i_travelled[cities_travelled++]=k;
        }

        static void init_pheromone()
        {
            for(int i=0; i<MAX_Cities; i++)
                for(int j=0; j<MAX_Cities; j++)
                    pheromone[i][j]=1.0;
        }

        friend void copy(Best_tour *b, int visited_cities[], int no_of_cities, double tour_value);
};

double Ant::pheromone[MAX_Cities][MAX_Cities];

int Ant::generate_next_city()
{
    int next_city = NOTHING, last = what_have_i_travelled[cities_travelled-1], flag;
    double max_weight_till_now = -INFINITY;

    for(int i=0; i<no_of_cities; i++)
    {
        flag=1;
        for(int j=0; j<cities_travelled; j++)
        {
            if(i == what_have_i_travelled[j])
            {
                flag=0;
                break;
            }
        }
        if(flag)
        {
            double weight = pow(map_of_city[last][i], alpha) *  pow(pheromone[last][i], beta);
            if(weight > max_weight_till_now)
            {
                max_weight_till_now = weight;
                next_city = i;
            }
        }
    }

    return next_city;
}

bool Ant::Travel_On()
{
    int next_city = generate_next_city();
    if(next_city != NOTHING)
    {
        int current_distance = map_of_city[what_have_i_travelled[cities_travelled-1]][next_city];
        Tour_value += current_distance;
        what_have_i_travelled[cities_travelled++]=next_city;
        return true;
    }
    return false;
}


bool Ant::live_your_tour()
{
    Tour_value=0;
    while(Travel_On());
    if(cities_travelled==no_of_cities)
    {
        Tour_value += map_of_city[what_have_i_travelled[0]][what_have_i_travelled[cities_travelled-1]];
        what_have_i_travelled[cities_travelled++]=what_have_i_travelled[0];
        return true;
    }
    return false;
}

typedef struct Best_tour
{
    int visited_cities[MAX_Cities];
    double tour_value;
}Best_tour;

void copy(Best_tour *b, int visited_cities[], int no_of_cities, double tour_value);






int main()
{
    //Take no. of cities in country
    cout<<"Enter No. of Cities: ";cin>>no_of_cities;    //No. of Cities


    //Take the map and distance of each city from each city
    for(int i=0; i<no_of_cities; i++)
        for(int j=i+1; j<no_of_cities; j++)
        {
            cout<<"Enter Value of path "<<i+1<<" to "<<j+1<<" : "; cin>>map_of_city[i][j];
            map_of_city[j][i]=map_of_city[i][j];
        }

    //Now we are going to find the solution for TSP
    //1. We have some no. of ants
    //2. We are going to choose initial city of each ant randomly
    //3. Also we have Pheromone Table which will be initialized to some value
    //4. Initially Best tour will be NULL or initializes to some least values

    Best_tour B;
    B.tour_value=32766;
    int visited_cities[MAX_Cities]={0};
    double worst_tour_value= INFINITY;
    copy(&B, visited_cities, no_of_cities, worst_tour_value);

    Ant::init_pheromone();

    const int no_of_ants=MAX_Ants;

    Ant ant_number[no_of_ants];

    //Now all initialization part is done
    //here we are starting the actual algorithm of ACO for solving TSP

    for(int i=0; i<no_of_ants; i++)
        ant_number[i].add_initial_city(rand()%no_of_cities);

    for(int i=0; i<no_of_ants; i++)
    {
        bool success = ant_number[i].live_your_tour();
        int flag=0;

        if(success)
        {
            double delta = ant_number[i].Tour_value - B.tour_value;

            if(delta<0)
            {
                copy(&B, ant_number[i].what_have_i_travelled, ant_number[i].cities_travelled, ant_number[i].Tour_value);
                flag=1;
            }
        }

        if(1)
        {
            bool road_is_in_best_path[MAX_Cities][MAX_Cities]={false};
            for(int i=0; i<no_of_cities-1; i++)
                road_is_in_best_path[B.visited_cities[i]][B.visited_cities[i+1]]=true;

            for(int i=0; i<no_of_cities; i++)
                for(int j=0; j<no_of_cities; j++)
                {
                    if(road_is_in_best_path[i][j])
                        Ant::pheromone[i][j] += Ant::pheromone[i][j] * Pheromone_Bonus_Factor;
                    else
                        Ant::pheromone[i][j] *= (1 - Pheromone_Decay_Factor);
                }
        }
    }

    cout<<"Best Path is ";
    for(int i=0; i<=no_of_cities; i++)
        cout<<" "<<B.visited_cities[i];
    cout<<"\nTour value is "<<B.tour_value<<endl;

    cout<<"\nFinal Pheromone Table is:"<<endl;
    for(int i=0; i<no_of_cities; i++)
    {
        for(int j=0; j<no_of_cities; j++)
        {
            printf("%.4lf ", Ant::pheromone[i][j]);;
        }
        cout<<endl;
    }
    return 0;
}


void copy(Best_tour *b, int visited_cities[], int no_of_cities, double tour_value)
{
    if(b->tour_value > tour_value)
    {
        b->tour_value = tour_value;
        for(int i=0; i < no_of_cities; i++)
            b->visited_cities[i]=visited_cities[i];
    }
}












/*int how_many_paths(int map[MAX][], int v)
{
    int cnt=0;
    for(i=0; i<MAX; i++)
        if(map[v][i]>0)
            cnt++;
    return cnt;
}*/


/*void find_solution(Ant zero, int map[MAX][], int total_nodes, int startnode, int endnode)
{
    if(startnode==endnode)
    {
        //this ant has reached the destination
        //note this path, distance, ant
        //destroy the ant
        return;
    }
    int totalPaths=how_many_paths(map, startnode);
    if(totalPaths==0)
    {
        //this is useless ant
        //remove complete pheromone from this path
        //destroy the ant
        return;
    }
    for(int i=0; i<totalPaths; i++)
    {
        //avoid revisiting same city
        //create new ant with same values as zero ant
        //and ask her to find the solution
    }
    //destroy zero ant
}*/

/*
int main()
{
    Ant::init_pheramone();
    Ant zero;
    int map[MAX][MAX]={0};
    int n;
    cin>>n;
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            cout<<i+1<<" to "<<j+1<<" : "; cin>>map[i][j];
            map[j][i]=map[i][j];
        }
        map[i][i]=0;
    }
    int startnode, endnode;
    cout<<"Enter Start Node: \t";
    cin>>startnode;
    cout<<"Enter End Node: \t";
    cin>>endnode;

    find_solution(zero, map, n, startnode, endnode);

    return 0;
}*/
