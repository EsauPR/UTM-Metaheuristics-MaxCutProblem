#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

#define MAXSIZE 100

#define POPULATION_SIZE 100
#define NUM_GENERATIONS 1000
#define SELECTION_PERCENTAGE 50 /* 0 to 100 */
#define MUTATION_PERCENTAGE 20 /* 0 to 100 */
#define CROSSOVER_PROBABILITY 80
#define MUTATION_PROBABILITY 15 /* 0 to 100 */

void read_input( int * num_nodes, int * num_conections );
void print_solution( int num_nodes );

int cut_sum( int num_nodes, vector<bool> S );

vector<bool> random_solution( int num_nodes );
vector<bool> genetic_algorithm( int num_nodes );

int Graph[ MAXSIZE ][ MAXSIZE ];
vector<bool> Set_Max_Cut;


int main(int argc, char const *argv[]) {

    int num_nodes, num_conections;

    Set_Max_Cut.resize( MAXSIZE );
    srand(time(NULL));

    read_input( &num_nodes, &num_conections );

    Set_Max_Cut = genetic_algorithm( num_nodes );
    print_solution( num_nodes );

    return 0;
}

/* Lee la entrade del problema */
void read_input( int * num_nodes, int * num_conections ) {
    int nodei, nodej, weight;

    scanf("%d %d", num_nodes, num_conections);

    for ( int i = 0; i < *num_conections; i++) {
        scanf("%d %d %d", &nodei, &nodej, &weight );
        Graph[ nodei - 1 ][ nodej - 1 ] = weight;
    }
}

/* Return random solution*/
vector<bool> random_solution( int num_nodes ) {
    vector<bool> S;
    S.resize( MAXSIZE );
    for ( int i = 0; i < num_nodes; i++ ) {
        S[i] = rand() % 2;
    }
    return S;
}

/* return the sum of max cut from any solution */
int cut_sum( int num_nodes, vector<bool> S ) {
    /* Calcula la suma del total de aristas que cruzan por el corte */
    int sum = 0;

    for ( int i = 0; i < num_nodes; i++ ) {
        /* Si el nodo i pertenece a la partición 1 */
        if( S[ i ] == 1 ) {
            for( int j = 0; j < num_nodes; j++ ) {
                /* Si existe una conexión con el nodo j y el nodo j pertenece a la partición 2 */
                if( Graph[ i ][ j ] != 0 && S[ j ] == 0 ) {
                    sum += Graph[ i ][ j ];
                }
            }
        }
    }

    return sum;
}

/* Print the current solution stered in Set_Max_Cut */
void print_solution( int num_nodes ) {

    for ( int i = 0; i < num_nodes; i++) {
        printf("(%d,%d) ", i, (int)Set_Max_Cut[i] );
    }
    puts("");
    printf("%d\n", cut_sum( num_nodes, Set_Max_Cut ) );
}

vector<bool> genetic_algorithm( int num_nodes ) {
    vector<bool> SBest = random_solution( num_nodes );
    int Best_Sum = cut_sum( num_nodes, SBest );

    vector<bool> population1[ POPULATION_SIZE ];
    vector<bool> population2[ POPULATION_SIZE ];

    vector<bool> * population;
    vector<bool> * population_tmp;
    vector<bool> * swap_tmp;

    population = population1;
    population_tmp = population2;

    /* Initial population */
    for (int i = 0; i < POPULATION_SIZE; i++) {
        population[ i ] = random_solution( num_nodes );
    }

    /* Generations */
    for (int i = 0; i < NUM_GENERATIONS; i++) {

        /* population ordered by sum of MaxCut */
        vector<pair<int,int> > population_ordered;
        for (int j = 0; j < POPULATION_SIZE; j++) {
            population_ordered.push_back( make_pair( cut_sum( num_nodes, population[j] ) , j ) );
        }
        sort( population_ordered.begin(), population_ordered.end() );

        /* if best parent has a better solution */
        if( population_ordered[ POPULATION_SIZE - 1 ].first > Best_Sum  ) {
            Best_Sum = population_ordered[ POPULATION_SIZE - 1 ].first;
            SBest = population[ population_ordered[ POPULATION_SIZE - 1 ].second ];
        }

		/* while has not reached the size of population */
        for (int j = 0; j < POPULATION_SIZE; j+=2) {
            /* Select some best Parents between selection percentaje*/
    		int parent1 = random() % (POPULATION_SIZE * SELECTION_PERCENTAGE / 100);
    		int parent2 = random() % (POPULATION_SIZE * SELECTION_PERCENTAGE / 100);
    		parent1 = population_ordered[parent1].second;
    		parent2 = population_ordered[parent2].second;

            /* crossover range */
    		int a = random() % num_nodes;
    		int b = a + random() % ( num_nodes - a);

            /* if there are not crossover probability then continuea */
            if( ( random() % 100 + 1 ) < CROSSOVER_PROBABILITY ) {
                j-=2;
                continue;
            }

            population_tmp[ j ] = population[ parent1 ];
            population_tmp[j+1] = population[ parent2 ];

            /* crossover == swap the section a to b */
            for (int k = a; k <= b; k++) {
                population_tmp[ j ][ k ] = (int)population_tmp[ j ][ k ] ^ (int)population_tmp[j+1][ k ];
                population_tmp[j+1][ k ] = (int)population_tmp[ j ][ k ] ^ (int)population_tmp[j+1][ k ];
                population_tmp[ j ][ k ] = (int)population_tmp[ j ][ k ] ^ (int)population_tmp[j+1][ k ];
            }
        }

        /* Mutation */
        for (int j = 0; j < POPULATION_SIZE; j++) {
            /* If there is a mutation */
            if( ( random() % 100 + 1 ) < MUTATION_PROBABILITY ) {
                int num_swap = random() % (num_nodes * MUTATION_PERCENTAGE) / 100;

                for( int k = 0; k < num_swap; k++ ) {
                    int pos = random() % num_nodes;
                    population_tmp[ j ][ pos ] = (int)population_tmp[ j ][ pos ] ^ 1;
                }
            }
        }

        /* Swap the last generation */
        swap_tmp = population;
        population = population_tmp;
        population_tmp = swap_tmp;
    }

    /* For the las generation */
    /* population ordered by sum of MaxCut */
    vector<pair<int,int> > population_ordered;
    for (int i = 0; i < POPULATION_SIZE; i++) {
        population_ordered.push_back( make_pair( cut_sum( num_nodes, population[i] ) , i ) );
    }
    sort( population_ordered.begin(), population_ordered.end() );

    /* if best parent has a better solution */
    if( population_ordered[ POPULATION_SIZE - 1 ].first > Best_Sum  ) {
        Best_Sum = population_ordered[ POPULATION_SIZE - 1 ].first;
        SBest = population[ population_ordered[ POPULATION_SIZE - 1 ].second ];
    }

    return SBest;
}
