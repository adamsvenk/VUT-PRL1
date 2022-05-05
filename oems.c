/**
 * @file oems.c
 * @author Adam Švenk (xsvenk00@stud.fit.vutbr.cz)
 * @brief 
 * @version 1.0
 * @date 2022-04-10
 */

#include <stdio.h>
#include <stdint.h>
#include <mpi.h>

#define INPUT_FILE "numbers"
#define MASTER_RANK 0
#define NUMBER_SIZE 1
#define NUMBER_COUNT 8

// Získanie maxima z dvoch hodnôt
int max(int a, int b) {
    return (a > b ) ? a : b;
}

// Získanie minima z dvoch hodnôt
int min(int a, int b) {
    return (a > b ) ? b : a;
}

int main(int argc, char *argv[]) {
    int rank;

    // Inicializácia exekučného prostredia
    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
        fprintf(stderr, "MPI Error\n");
    }

    // Získanie identifikátora procesu rank
    if (MPI_Comm_rank(MPI_COMM_WORLD, &rank) != MPI_SUCCESS) {
        fprintf(stderr, "MPI Error\n");
    }

    // Všetky načítané hodnoty
    uint8_t all_numbers[8];
    
    // Výstupnéh hodnoty každého z procesov
    uint8_t number1, number2, swap;

    // Status, do ktorého sa uloží výsledok operácie MPI_Recv
    MPI_Status status;

    // Vykoná len prvý proces
    if (rank == MASTER_RANK) {
        // Získanie handleru na otvorenie vstupného súboru
        FILE *fp = fopen(INPUT_FILE, "r");

        // Overenie, či získanie handleru prebehlo v poriadku
        if (fp == NULL) {
            fprintf(stderr, "Error while opening a file\n");
        }

        // Načítanie 8 vstupných hodnôt
        fread(&all_numbers, NUMBER_SIZE, NUMBER_COUNT, fp);

        // Vypísanie 8 načítaných vstupných hodnôt
        for (uint8_t i = 0; i < 8; i++) {
            printf("%d ", all_numbers[i]);
        }

        printf("\n");

        // Odoslanie načítaných hodnôt procesom s identifikátorom 1 - 3 (procesor 0 už hodnoty má)
        for (int i = 1; i < 4; i++) {
            MPI_Send(&all_numbers, 8, MPI_UINT8_T, i, 0, MPI_COMM_WORLD);
        }
    }

    // Prijatie prvotných hodnôt procesmi 1 - 3
    if (rank == 1 || rank == 2 || rank == 3) {
        MPI_Recv(&all_numbers, 8, MPI_UINT8_T, MASTER_RANK, 0, MPI_COMM_WORLD, &status);
    }

    // Zoradenie vstupných hodnôt procesmi 0 - 3
    if (rank == 0 || rank == 1 || rank == 2 || rank == 3) {
        number1 = all_numbers[rank * 2];
        number2 = all_numbers[rank * 2 + 1];

        swap = number1;
        number1 = max(swap, number2);
        number2 = min(swap, number2);
    }

    // Odoslanie a prijatie výmeny medzi procesmi 0,1,4,5
    if (rank == 0 || rank == 1 || rank == 4 || rank == 5) {
        // Komunikácia medzi procesmi
        switch(rank) {
            case 0:
                MPI_Send(&number1, 1, MPI_UINT8_T, 4, 1, MPI_COMM_WORLD);
                MPI_Send(&number2, 1, MPI_UINT8_T, 5, 1, MPI_COMM_WORLD);
                break;
            case 1:
                MPI_Send(&number1, 1, MPI_UINT8_T, 4, 2, MPI_COMM_WORLD);
                MPI_Send(&number2, 1, MPI_UINT8_T, 5, 2, MPI_COMM_WORLD);
                break;
            case 4:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 0, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 1, 2, MPI_COMM_WORLD, &status);
                break;
            case 5:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 0, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 1, 2, MPI_COMM_WORLD, &status);
                break;
        }
    }

    // Odoslanie a prijatie výmeny medzi procesmi 2,3,6,7
    if (rank == 2 || rank == 3 || rank == 6 || rank == 7) {
        // Komunikácia medzi procesmi
        switch(rank) {
            case 2:
                MPI_Send(&number1, 1, MPI_UINT8_T, 6, 1, MPI_COMM_WORLD);
                MPI_Send(&number2, 1, MPI_UINT8_T, 7, 1, MPI_COMM_WORLD);
                break;
            case 3:
                MPI_Send(&number1, 1, MPI_UINT8_T, 6, 2, MPI_COMM_WORLD);
                MPI_Send(&number2, 1, MPI_UINT8_T, 7, 2, MPI_COMM_WORLD);
                break;
            case 6:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 2, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 3, 2, MPI_COMM_WORLD, &status);
                break;
            case 7:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 2, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 3, 2, MPI_COMM_WORLD, &status);
                break;
        }
    }

    // Odoslanie a prijatie výmeny medzi procesmi 4,5,8
    if (rank == 4 || rank == 5 || rank == 8) {
        // Zoradenie vstupných hodnôt
        if (rank == 4 || rank == 5) {
            swap = number1;
            number1 = max(swap, number2);
            number2 = min(swap, number2);
        }

        // Komunikácia medzi procesmi
        switch (rank) {
            case 4:
                MPI_Send(&number2, 1, MPI_UINT8_T, 8, 1, MPI_COMM_WORLD);
                break;
            case 5:
                MPI_Send(&number1, 1, MPI_UINT8_T, 8, 2, MPI_COMM_WORLD);
                break;
            case 8:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 4, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 5, 2, MPI_COMM_WORLD, &status);
                break;
        }
    }

    // Odoslanie a prijatie výmeny medzi procesmi 6,7,9
    if (rank == 6 || rank == 7 || rank == 9) {
        // Zoradenie vstupných hodnôt
        if (rank == 6 || rank == 7) {
            swap = number1;
            number1 = max(swap, number2);
            number2 = min(swap, number2);

        }

        // Komunikácia medzi procesmi
        switch (rank) {
            case 6:
                MPI_Send(&number2, 1, MPI_UINT8_T, 9, 1, MPI_COMM_WORLD);
                break;
            case 7:
                MPI_Send(&number1, 1, MPI_UINT8_T, 9, 2, MPI_COMM_WORLD);
                break;
            case 9:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 6, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 7, 2, MPI_COMM_WORLD, &status);
                break;

        }
    }

    // Odoslanie a prijatie výmeny medzi procesmi 4,5,6,7,8,9,10,11,12,13
    if (rank == 4 || rank == 5 || rank == 6 || rank == 7 || rank == 8 || rank == 9 || rank == 10 || rank == 11 || rank == 12 || rank == 13) {
        // Zoradenie vstupných hodnôt
        if (rank == 4 || rank == 5 || rank == 6 | rank == 7 | rank == 8 | rank == 9) {
            swap = number1;
            number1 = max(swap, number2);
            number2 = min(swap, number2);
        }

        // Komunikácia medzi procesmi
        switch (rank) {
            case 4:
                MPI_Send(&number1, 1, MPI_UINT8_T, 10, 1, MPI_COMM_WORLD);
                break;
            case 5:
                MPI_Send(&number2, 1, MPI_UINT8_T, 13, 1, MPI_COMM_WORLD);
                break;
            case 6:
                MPI_Send(&number1, 1, MPI_UINT8_T, 10, 2, MPI_COMM_WORLD);
                break;
            case 7:
                MPI_Send(&number2, 1, MPI_UINT8_T, 13, 2, MPI_COMM_WORLD);
                break;
            case 8:
                MPI_Send(&number1, 1, MPI_UINT8_T, 12, 1, MPI_COMM_WORLD);
                MPI_Send(&number2, 1, MPI_UINT8_T, 11, 1, MPI_COMM_WORLD);
                break;
            case 9:
                MPI_Send(&number1, 1, MPI_UINT8_T, 12, 2, MPI_COMM_WORLD);
                MPI_Send(&number2, 1, MPI_UINT8_T, 11, 2, MPI_COMM_WORLD);
                break;
            case 10:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 4, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 6, 2, MPI_COMM_WORLD, &status);
                break;
            case 11:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 8, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 9, 2, MPI_COMM_WORLD, &status);
                break;
            case 12:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 8, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 9, 2, MPI_COMM_WORLD, &status);
                break;
            case 13:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 5, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 7, 2, MPI_COMM_WORLD, &status);
                break;
        }
    }

    // Odoslanie a prijatie výmeny medzi procesmi 10,11,14
    if (rank == 10 || rank == 11 || rank == 14) {
        // Zoradenie vstupných hodnôt
        if (rank == 10 || rank == 11) {
            swap = number1;
            number1 = max(swap, number2);
            number2 = min(swap, number2);
        }

        // Komunikácia medzi procesmi
        switch (rank) {
            case 10:
                MPI_Send(&number2, 1, MPI_UINT8_T, 14, 1, MPI_COMM_WORLD);
                MPI_Send(&number1, 1, MPI_UINT8_T, MASTER_RANK, 8, MPI_COMM_WORLD);
                break;
            case 11:
                MPI_Send(&number1, 1, MPI_UINT8_T, 14, 2, MPI_COMM_WORLD);
                break;
            case 14:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 10, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 11, 2, MPI_COMM_WORLD, &status);
                break;
        }
    }

    // Odoslanie a prijatie výmeny medzi procesmi 12,13,15
    if (rank == 12 || rank == 13 || rank == 15) {
        // Zoradenie vstupných hodnôt
        if (rank == 12 || rank == 13) {
            swap = number1;
            number1 = max(swap, number2);
            number2 = min(swap, number2);
        }

        // Komunikácia medzi procesmi
        switch (rank) {
            case 12:
                MPI_Send(&number2, 1, MPI_UINT8_T, 15, 1, MPI_COMM_WORLD);
                break;
            case 13:
                MPI_Send(&number1, 1, MPI_UINT8_T, 15, 2, MPI_COMM_WORLD);

                MPI_Send(&number2, 1, MPI_UINT8_T, MASTER_RANK, 1, MPI_COMM_WORLD);
                break;
            case 15:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 12, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 13, 2, MPI_COMM_WORLD, &status);
                break;
        }
    }

    // Odoslanie a prijatie výmeny medzi procesmi 11,12,14,15,16,17,18
    if (rank == 11 || rank == 12 || rank == 14 || rank == 15 || rank == 16 || rank == 17 || rank == 18) {
        // Zoradenie vstupných hodnôt
        if (rank == 11 || rank == 12 || rank == 14 || rank == 15) {
            swap = number1;
            number1 = max(swap, number2);
            number2 = min(swap, number2);
        }

        // Komunikácia medzi procesmi
        switch (rank) {
            case 11:
                MPI_Send(&number2, 1, MPI_INT8_T, 18, 1, MPI_COMM_WORLD);
                break;
            case 12:
                MPI_Send(&number1, 1, MPI_INT8_T, 16, 2, MPI_COMM_WORLD);
                break;
            case 14:
                MPI_Send(&number1, 1, MPI_INT8_T, 16, 1, MPI_COMM_WORLD);
                MPI_Send(&number2, 1, MPI_INT8_T, 17, 1, MPI_COMM_WORLD);
                break;
            case 15:
                MPI_Send(&number1, 1, MPI_INT8_T, 17, 2, MPI_COMM_WORLD);
                MPI_Send(&number2, 1, MPI_INT8_T, 18, 2, MPI_COMM_WORLD);
                break;
            case 16:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 14, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 12, 2, MPI_COMM_WORLD, &status);
                
                // Zoradenie vstupných hodnôt
                swap = number1;
                number1 = max(swap, number2);
                number2 = min(swap, number2);

                MPI_Send(&number2, 1, MPI_UINT8_T, MASTER_RANK, 6, MPI_COMM_WORLD);
                MPI_Send(&number1, 1, MPI_UINT8_T, MASTER_RANK, 7, MPI_COMM_WORLD);
                break;
            case 17:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 14, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 15, 2, MPI_COMM_WORLD, &status);

                // Zoradenie vstupných hodnôt
                swap = number1;
                number1 = max(swap, number2);
                number2 = min(swap, number2);

                MPI_Send(&number2, 1, MPI_UINT8_T, MASTER_RANK, 4, MPI_COMM_WORLD);
                MPI_Send(&number1, 1, MPI_UINT8_T, MASTER_RANK, 5, MPI_COMM_WORLD);
                break;
            case 18:
                MPI_Recv(&number1, 1, MPI_UINT8_T, 11, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&number2, 1, MPI_UINT8_T, 15, 2, MPI_COMM_WORLD, &status);

                // Zoradenie vstupných hodnôt
                swap = number1;
                number1 = max(swap, number2);
                number2 = min(swap, number2);

                MPI_Send(&number2, 1, MPI_UINT8_T, MASTER_RANK, 2, MPI_COMM_WORLD);
                MPI_Send(&number1, 1, MPI_UINT8_T, MASTER_RANK, 3, MPI_COMM_WORLD);
                break;
        }
    }

    // Prijatie výsledných hodnôt prvým procesom
    if (rank == MASTER_RANK) {
        uint8_t por1, por2, por3, por4, por5, por6, por7, por8;
        MPI_Recv(&por1, 1, MPI_UINT8_T, 13, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&por2, 1, MPI_UINT8_T, 18, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&por3, 1, MPI_UINT8_T, 18, 3, MPI_COMM_WORLD, &status);
        MPI_Recv(&por4, 1, MPI_UINT8_T, 17, 4, MPI_COMM_WORLD, &status);
        MPI_Recv(&por5, 1, MPI_UINT8_T, 17, 5, MPI_COMM_WORLD, &status);
        MPI_Recv(&por6, 1, MPI_UINT8_T, 16, 6, MPI_COMM_WORLD, &status);
        MPI_Recv(&por7, 1, MPI_UINT8_T, 16, 7, MPI_COMM_WORLD, &status);
        MPI_Recv(&por8, 1, MPI_UINT8_T, 10, 8, MPI_COMM_WORLD, &status);

        // Vypísanie prijatých zoradených hodnôt
        printf("%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", por1, por2, por3, por4, por5, por6, por7, por8);
    }   

    // Ukončenie exekučného prostredia MPI

    MPI_Finalize();

    // Ukončenie programu
    return 0;
}