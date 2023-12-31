#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define MAX_COORDS 4096 // Maximum number of coordinates

// Global variables
double coords[MAX_COORDS][2]; //  coordinates read from file
double distanceMatrix[MAX_COORDS][MAX_COORDS]; //  calculated distance matrix
int numOfCoords; // umber of coordinates read from file

// Function prototypes
void readCoordinates(const char* filename);
void calculateDistanceMatrix();
void farthestInsertion(const char* outputFilename);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <coordinate_file_name> <output_file_name>\n", argv[0]);
        return 1;
    }

    const char* inputFilename = argv[1];
    const char* outputFilename = argv[2];

    readCoordinates(inputFilename);
    calculateDistanceMatrix();
    farthestInsertion(outputFilename);

    return 0;
}

// Function to read coordinates from file and populate the coords array
void readCoordinates(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    numOfCoords = 0;
    while (fscanf(file, "%lf,%lf", &coords[numOfCoords][0], &coords[numOfCoords][1]) == 2) {
        numOfCoords++;
    }

    fclose(file);
}

// Function to calculate the Euclidean distance between two points
double euclideanDistance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// Function to generate the distance matrix from the coordinates
void calculateDistanceMatrix() {
    for (int i = 0; i < numOfCoords; i++) {
        for (int j = 0; j < numOfCoords; j++) {
            if (i == j) {
                distanceMatrix[i][j] = 0.0;
            } else {
                distanceMatrix[i][j] = euclideanDistance(coords[i][0], coords[i][1], coords[j][0], coords[j][1]);
            }
        }
    }
}


void farthestInsertion(const char* outputFilename) {
    // Array to hold the tour
    int* tour = malloc(numOfCoords * sizeof(int));
    if (!tour) {
        perror("Memory allocation for tour failed");
        exit(EXIT_FAILURE);
    }

    // Boolean array to keep track of visited vertices
    int* visited = calloc(numOfCoords, sizeof(int));
    if (!visited) {
        free(tour);
        perror("Memory allocation for visited failed");
        exit(EXIT_FAILURE);
    }

    // Find the two farthest apart vertices to start the tour
    double maxDist = 0;
    for (int i = 0; i < numOfCoords; i++) {
        for (int j = i + 1; j < numOfCoords; j++) {
            if (distanceMatrix[i][j] > maxDist) {
                maxDist = distanceMatrix[i][j];
                tour[0] = i;
                tour[1] = j;
            }
        }
    }
    visited[tour[0]] = 1;
    visited[tour[1]] = 1;
    int tourSize = 2;

    while (tourSize < numOfCoords) {
        int farthest = -1;
        maxDist = 0;

        // farthest unvisited vertex from the current tour
        for (int i = 0; i < numOfCoords; i++) {
            if (!visited[i]) {
                for (int j = 0; j < tourSize; j++) {
                    if (distanceMatrix[i][tour[j]] > maxDist) {
                        maxDist = distanceMatrix[i][tour[j]];
                        farthest = i;
                    }
                }
            }
        }

        // best position to insert the farthest vertex
        double minIncrease = DBL_MAX;
        int positionToInsert = 0;
        for (int i = 0; i < tourSize; i++) {
            int next = (i + 1) % tourSize;
            double increase = distanceMatrix[tour[i]][farthest] + distanceMatrix[farthest][tour[next]] - distanceMatrix[tour[i]][tour[next]];
            if (increase < minIncrease) {
                minIncrease = increase;
                positionToInsert = next;
            }
        }

        // Insert the farthest vertex into the tour
        for (int i = tourSize; i > positionToInsert; i--) {
            tour[i] = tour[i - 1];
        }
        tour[positionToInsert] = farthest;
        visited[farthest] = 1;
        tourSize++;
    }

    // Write the tour to the output file
    FILE* file = fopen(outputFilename, "w");
    if (file == NULL) {
        perror("Error opening output file");
        free(tour);
        free(visited);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < numOfCoords; i++) {
        fprintf(file, "%d\n", tour[i]);
    }
    fclose(file);
    free(tour);
    free(visited);
}
