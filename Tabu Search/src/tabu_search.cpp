#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

#define MAXSIZE 100

#define MAX_ITERATIONS 10000
#define NUM_NEIGHBOR 50
#define MAXTABU_SIZE 500

int change_percent = 90;

void read_input( int * num_nodes, int * num_conections );
void ramdom_solution( int num_nodes );
void print_solution( int num_nodes );

int cut_sum( int num_nodes, vector<bool> S );

bool containsTabuList( vector<bool> S, map<vector<bool>,bool> tabu_list );

vector<bool> tabu_search( int num_nodes );
vector<bool> new_neighbour( int num_nodes, vector<bool> S, int change_percent );
vector<bool> best_candidate( int num_nodes, vector<vector<bool> > candidate_list );

int Graph[ MAXSIZE ][ MAXSIZE ];
vector<bool> Set_Max_Cut;

int main(int argc, char const *argv[]) {
    int num_nodes, num_conections;

    Set_Max_Cut.resize( MAXSIZE );
    srand(time(NULL));

    read_input( &num_nodes, &num_conections );
    ramdom_solution( num_nodes );

    print_solution( num_nodes );
    Set_Max_Cut = tabu_search( num_nodes );
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

/* Genera solución inicial random */
void ramdom_solution( int num_nodes ) {

    for ( int i = 0; i < num_nodes; i++ ) {
        Set_Max_Cut[i] = rand() % 2;
    }
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

/* Besto solution from a vector of solutions */
vector<bool> best_candidate( int num_nodes, vector<vector<bool> > candidate_list ) {
    vector<bool> S = candidate_list[0];
    int maxSum  =  cut_sum( num_nodes, candidate_list[ 0 ] );

    for( int i = 1, sum_tmp; i < candidate_list.size(); i++ ) {
        sum_tmp = cut_sum( num_nodes, candidate_list[ i ] );
        if( sum_tmp > maxSum ) {
            maxSum = sum_tmp;
            S = candidate_list[ i ];
        }
    }

    return S;
}

/* Return true if a solution is contain in tabulist */
bool containsTabuList( vector<bool> S, map<vector<bool>,bool> tabu_list ) {
    if( tabu_list.count( S ) ) {
        return true;
    }
    return false;
}

/* Return a neighbor from a solution */
vector<bool> new_neighbour( int num_nodes, vector<bool> S, int change_percent ) {
    int num_changes = num_nodes * change_percent / 100;

    for( int i = 0, pos; i < num_changes; i++ ) {
        pos = rand() % num_nodes;
        S[ pos ] = (int)S[pos] ^ 1;
    }
    return S;
}

/* Implementation of Tabu Search algorithm */
vector<bool> tabu_search( int num_nodes ) {

    map<vector<bool>, bool> tabu_list;
    map<vector<bool>, bool> :: iterator it;

    vector<bool> S = Set_Max_Cut;
    vector<bool> SBest = S;

    for( int i = 0; i < MAX_ITERATIONS; i++ ) {

        vector<vector<bool> > candidate_list;
        vector<vector<bool> > neighborhood;

        /* Getting neighbor */
        for( int j = 0; j < NUM_NEIGHBOR; j++ ) {
            neighborhood.push_back( new_neighbour( num_nodes, S, change_percent ) );
        }

        for( int j = 0; j < NUM_NEIGHBOR; j++ ) {
            if( ! containsTabuList( neighborhood[ j ], tabu_list ) ) {
                candidate_list.push_back( neighborhood[ j ] );
            }
        }

        S = best_candidate( num_nodes, candidate_list );

        if( cut_sum( num_nodes, S ) > cut_sum( num_nodes, SBest ) ) {
            tabu_list[ S ] = 1;
            if( tabu_list.size() == MAXTABU_SIZE ) {
                it = tabu_list.begin();
                tabu_list.erase( it );
            }
            SBest = S;
        }
    }

    return SBest;
}

/* Print the current solution stered in Set_Max_Cut */
void print_solution( int num_nodes ) {

    for ( int i = 0; i < num_nodes; i++) {
        printf("(%d,%d) ", i, (int)Set_Max_Cut[i] );
    }
    puts("");
    printf("%d\n", cut_sum( num_nodes, Set_Max_Cut ) );
}
