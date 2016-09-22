#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

#define MAXSIZE 100

int Graph[ MAXSIZE ][ MAXSIZE ];
int Set_Max_Cut[MAXSIZE];

void read_input( int * num_nodes, int * num_conections );
void ramdom_solution( int num_nodes );
void new_neighbour( int *best_solution, int *neighbour_solution, int num_nodes, int change_percent );
void print_solution( int num_nodes );
void read_param();
void simulated_annealing( int num_nodes );


int cut_sum( int num_nodes, int *set_cut );

int main(int argc, char const *argv[]) {
    int num_nodes, num_conections;

    srand(time(NULL));

    read_input( &num_nodes, &num_conections );
    ramdom_solution( num_nodes );

    simulated_annealing( num_nodes );

    //print_solution( num_nodes );
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


void print_solution( int num_nodes ) {

    for ( int i = 0; i < num_nodes; i++) {
        printf("(%d,%d) ", i, Set_Max_Cut[i] );
    }
    puts("");
}

void read_param() {
    /*
	FILE *file = NULL;
	char str_tmp[100];
	file = fopen("config", "r");

	if( file == NULL ) {
		printf("No se pudo leer el archivo: %s\n", name);
		exit(EXIT_FAILURE);
	}

	//fscanf(file, "%s %d", str_tmp, &POPULATION_SIZE);

	fclose(file);
    */
}

int cut_sum( int num_nodes, int *set_cut ) {
    /* Calcula la suma del total de aristas que cruzan por el corte */
    int sum = 0;

    for ( int i = 0; i < num_nodes; i++ ) {
        /* Si el nodo i pertenece a la partición 1 */
        if( set_cut[ i ] == 1 ) {
            for( int j=0; j < num_nodes; j++ ) {
                /* Si existe una conexión con el nodo j y el nodo j pertenece a la partición 2 */
                if( Graph[ i ][ j ] != 0 && set_cut[ j ] == 0 ) {
                    sum += Graph[ i ][ j ];
                }
            }
        }
    }

    return sum;
}

void simulated_annealing( int num_nodes ) {
    /*
        T representa la tempreatura
        K representa el numero de interaciones máximo por etapa, donde una etapa es un nivel de temperatura
        A representa el número máximo de aceptaciones que se hacen en cada etapa
    */

    double T, K, A;
    double k = 0, a = 0;
    int neighbour_solution[MAXSIZE];
    double FRZN, r, alfa, p = 1.05;
    int change_percent;
    int *S0, *S1, SumS0, SumS1, *tmp;

    FRZN = 0.0000001;
    alfa = 0.99;
    K = 30;
    A = 300;
    change_percent = 15;

    for (int i = 0; i < num_nodes; i++) {
        neighbour_solution[ i ] = Set_Max_Cut[ i ];
    }
    /*
        En problemas como TSP, donde el input son los nodos de un grafo y las soluciones posibles
        son distintas formas de recorrer estos nodos, pude tomarse T0 proporcional
        a la raiz cuadrada de la cantidad de nodos
    */
    T = sqrt( num_nodes );
    T = 2000;
    S0 = &Set_Max_Cut[0];
    S1 = &neighbour_solution[0];
    int MAX = 0;
    /*
        Criterio de parada
            T < Epsilon / [ln (#S – 1)/Teta]
        o
            A/K < FRZN
        Donde FRZN es un parámetro de congelamiento
        Conforme aumenta K,  A/K la cantidad de aceptaciones respecto de la cantidad de
        iteraciones es muy chica, esto da la idea de que cambiar de configuracion es muy dificil
    */

    /*
    while( T > 0.01 ) {
        new_neighbour( S1, num_nodes, change_percent );
        SumS0 = cut_sum( num_nodes, S0 );
        SumS1 = cut_sum( num_nodes, S1 );

        double asd = pow( exp( 1 ), (double)(SumS0 - SumS1) / T );

        T = T * alfa;

        printf("T: %.4f     Exp: %.4f\n", T, asd );
    }

    return;
    //*/
    while( (A / K) > FRZN ) {

        //printf("\nCriterio de parada: %.4f FRZN: %.4f Temp: %.4f\n", A / K, FRZN, T);
        //sleep( 1 );

        while( k < K && a < A ) {
            //sleep(1);
            new_neighbour( S0, S1, num_nodes, change_percent );
            SumS0 = cut_sum( num_nodes, S0 );
            SumS1 = cut_sum( num_nodes, S1 );

            MAX = max( MAX, SumS0);
            MAX = max( MAX, SumS1);

            //printf("\nK: %.3f , k: %.3f , A: %.3f , a: %.3f\n", K, k, A, a );
            //printf("SumS0: %d     SumS1: %d\n", SumS0, SumS1 );
            if(  SumS0 < SumS1 ) {
                /* ACepta nueva solución */
                tmp = S1;
                S1 = S0;
                S0 = tmp;
                /* Incrementamos soluciones aceptadas */
                a++;
            }
            else {
                r = rand() % 1000; r = r/1000;
                //printf("Exp: %.4f R: %.4f\n", pow( exp( 1 ), (double)(SumS1 - SumS0) / T ), r );
                if( r < pow( exp( 1 ), (double)(SumS1 - SumS0) / T ) ) {
                    /* ACepta nueva solución */
                    tmp = S1;
                    S1 = S0;
                    S0 = tmp;
                    /* Incrementamos soluciones aceptadas */
                    a++;
                    //puts("SI");
                }
                else {
                    //puts("NO");
                }
            }
            k++;
        }

        /*
            Factor de enfriamiento
            T_next = alfa * T   => Factor de enfriamiento geométrico, alfa < 1, muy cercano a 1
            T_next = 1 / ( 1 + beta * T ), beta es un real positivo muy cercano a cero
        */
        T = alfa * T;
        /*
            A medida que T disminuye se supone que al sistema le resulta mas dificil alcanzar un equilibrio
            porque es mas dificultoso el movimiento, entonces hay que esperar mas tiempo,
            esto se traduce en aumentar K.
            Parametro de aumento de K (p, se usan valores alrededor de 1,05)
        */
        K = p * K;
        a=0;
        k=0;
    }

    printf("Suma %d MAX %d T: %.4f\n", cut_sum( num_nodes, S0 ), MAX, T );

    printf("%.4f\n", exp(1) );

}

void new_neighbour( int *best_solution, int *neighbour_solution, int num_nodes, int change_percent ) {
    int num_changes = num_nodes * change_percent / 100;

    for( int i=0; i < num_nodes; i++ ) {
        neighbour_solution[i] = best_solution[i];
    }

    for( int i = 0; i < num_changes; i++ ) {
        neighbour_solution[ rand() % num_nodes ] ^= 1;
    }
}
