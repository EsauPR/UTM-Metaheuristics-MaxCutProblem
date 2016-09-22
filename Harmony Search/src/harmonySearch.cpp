#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
using namespace std;

#define MAXSIZE 5000
#define MEMORY_SIZE 500
#define CONSOLIDATION_RATE 0.85
#define PITCH_ADJUST_RATE 0.25
#define IMPROVISATION_MAX 10000

void read_input( int * num_nodes, int * num_conections );
void ramdom_solution( int num_nodes );
void initializeHarmonyMemory( int PITCH_NUM );
void print_solution( vector<bool> solution, int PITCH_NUM );

int cut_sum( int num_nodes, vector<bool> S );

bool adjustPitch( vector<bool> harmony, int PITCH );

vector<bool> newHarmony( int PITCH_NUM );
vector<bool> harmonySearch( int PITCH_NUM );


vector< pair< vector<bool>, int> > harmonyMemory;

int Graph[ MAXSIZE ][ MAXSIZE ];

int main(int argc, char const *argv[]) {
    int num_nodes, num_conections;
    srand(time(NULL));

    vector<bool> bestSolution;

    read_input( &num_nodes, &num_conections );

    bestSolution = harmonySearch( num_nodes );

    print_solution( bestSolution, num_nodes );

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

/* Print the current solution stered in Set_Max_Cut */
void print_solution( vector<bool> solution, int PITCH_NUM ) {
    /*
    for ( int i = 0; i < PITCH_NUM; i++) {
        printf("(%d,%d) ", i, (int)solution[i] );
    }
    puts("");
    //*/
    printf("%d\n", cut_sum( PITCH_NUM, solution ) );
}

/* return the sum of max cut from any solution */
int cut_sum( int num_nodes, vector<bool> S ) {
    /* Calcula la suma del total de aristas que cruzan por el corte */
    int sum = 0;

    for ( int i = 0; i < num_nodes; i++ ) {
        /* Si el nodo i pertenece a la partición 1 */
        if( S[ i ] == 1 ) {
            for( int j = i + 1; j < num_nodes; j++ ) {
                /* Si existe una conexión con el nodo j y el nodo j pertenece a la partición 2 */
                if( Graph[ i ][ j ] != 0 && S[ j ] == 0 ) {
                    sum += Graph[ i ][ j ];
                }
            }
        }
        else {
        	for( int j = i + 1; j < num_nodes; j++ ) {
                /* Si existe una conexión con el nodo j y el nodo j pertenece a la partición 1 */
                if( Graph[ i ][ j ] != 0 && S[ j ] == 1 ) {
                    sum += Graph[ i ][ j ];
                }
            }	
        }
    }

    return sum;
}

/* Return a neighbor from a solution */
vector<bool> newHarmony( int PITCH_NUM ) {
    vector<bool> S;
    S.resize( PITCH_NUM );
    for( int i = 0, pos; i < PITCH_NUM; i++ ) {
        S[i] = rand() % 2;
    }
    return S;
}

/* Initialize the harmony memory with random solutions and set the cost of each solution*/
void initializeHarmonyMemory( int PITCH_NUM ) {
    harmonyMemory.resize( MEMORY_SIZE );
    vector<bool> S;
    for(int i = 0; i < MEMORY_SIZE; i++) {
        S = newHarmony( PITCH_NUM );
        harmonyMemory[ i ] = make_pair( S, cut_sum( PITCH_NUM, S ) );
    }
}

/* return a random solution from the memory harmony */
bool selectRandomHarmony( int PITCH ) {
    return harmonyMemory[ random() % MEMORY_SIZE ].first[ PITCH ];
}

/* adjust pitch */
bool adjustPitch( vector<bool> harmony, int pitch ) {
    int sum_pitch1 = 0;
    int sum_pitch0 = 0;

    for (int i = 0; i < pitch; i++) {
        /* If there is a conection  */
        if( Graph[ pitch ][ i ] != 0 ) {
            /* if previous pitchs are in set0 */
            if( harmony[ i ] == true ) {
                sum_pitch1 += Graph[ pitch ][ i ];
            }
            /* if previous pitchs are in set1 */
            else {
                sum_pitch0 += Graph[ pitch ][ i ];
            }
        }
    }

    return sum_pitch1 >= sum_pitch0;
}

/* return random pitch */
bool randomPitch() {
    return random() % 2;
}

/* Return the worst solution from the Harmony memory */
int worstHarmony( int PITCH_NUM ) {
    int worstSolution = 0;
    int worstCost = cut_sum( PITCH_NUM, harmonyMemory[0].first );

    for(int i = 1, currentCost; i < MEMORY_SIZE; i++) {
        currentCost = cut_sum( PITCH_NUM, harmonyMemory[i].first );
        if( worstCost > currentCost ) {
            worstCost = currentCost;
            worstSolution = i;
        }
    }

    return worstSolution;
}

/* Return the best solution from the Harmony memory */
int bestHarmony( int PITCH_NUM ) {
    int bestSolution = 0;
    int bestCost = cut_sum( PITCH_NUM, harmonyMemory[0].first );

    for(int i = 1, currentCost; i < MEMORY_SIZE; i++) {
        currentCost = cut_sum( PITCH_NUM, harmonyMemory[i].first );
        if( bestCost < currentCost ) {
            bestCost = currentCost;
            bestSolution = i;
        }
    }

    return bestSolution;
}

/* Harmony search */
vector<bool> harmonySearch( int PITCH_NUM ) {

    initializeHarmonyMemory( PITCH_NUM );

    for(int i = 0; i < IMPROVISATION_MAX; i++ ) {
        vector<bool> harmony;
        harmony.resize( PITCH_NUM );

        for(int j = 0; j < PITCH_NUM; j++ ) {
            if( random() % 1000 < CONSOLIDATION_RATE * 1000) {
                bool randomHarmonyPitch = selectRandomHarmony( j );

                if( random() % 1000 < PITCH_ADJUST_RATE * 1000) {
                    randomHarmonyPitch = adjustPitch( harmony, randomHarmonyPitch );
                }
                harmony[ j ] = randomHarmonyPitch;
            }
            else {
                harmony[ j ] = randomPitch();
            }
        }

        int worstSolution = worstHarmony( PITCH_NUM );
        int worstCost = harmonyMemory[ worstSolution ].second;

        int currentCost = cut_sum( PITCH_NUM, harmony );

        if( currentCost >= worstCost ) {
            harmonyMemory[ worstSolution ] = make_pair( harmony, currentCost );
        }
    }

    return harmonyMemory[ bestHarmony( PITCH_NUM ) ].first;
}
