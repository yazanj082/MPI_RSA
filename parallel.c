#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <mpi.h>
#define N 250
// Function prototypes
void primefiller();
int pickrandomprime();
void setkeys();
long long int encrypt(double message);
long long int decrypt(int encrypted_text);
int* encoder(const char* message, int* size);
char* decoder(const int* encoded, int size);

// Global variables
int *prime;
int public_key;
int private_key;
int n;
int rank, size,arraySize;
// Function to fill the primes array using the Sieve of Eratosthenes

bool isPrime(int num) {
    if (num <= 1) {
        return false;
    }
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

// Function to generate prime numbers in a given range and store them in an array
void generatePrimesInRange(int start, int end, int primes[]) {
    int count =0;
    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            primes[(count)++] = i;
        }
    }
}
void primefiller(){
    int section = (N/size);
    int localPrime[section];
    int start = rank*section;
    int end = rank*section + section;
    generatePrimesInRange(start,end,localPrime);
    prime = (int*)malloc(size * section * sizeof(int));
    arraySize = N - N%size;
    MPI_Gather(localPrime,section,MPI_INT,prime,section,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(prime,arraySize,MPI_INT,0,MPI_COMM_WORLD);
    
}
int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}
// Function to pick a random prime number and remove it from the array
int pickrandomprime() {
    int k = rand() % arraySize;
    while (!prime[k]) {
        k = rand() % arraySize;
    }
    int result = prime[k];
    prime[k] = 0;
    return result;
}

// Function to set public and private keys
void setkeys() {
    int prime1,prime2;

    prime1 = pickrandomprime();
    prime2 = pickrandomprime();

    
    n = prime1 * prime2;
    int fi = (prime1 - 1) * (prime2 - 1);
    int e = 2;
    int d = 2;

    MPI_Request recv_request;
    
    int e1 = e + rank;
    int buff = 0;
    int recv_flag = 0;
    while (1) {
        if (gcd(e1, fi) == 1)
        {
            for (int dest = 0; dest < size; dest++) {
                if(dest == rank)
                {continue;}
                MPI_Request request;
                MPI_Isend(&e1, 1, MPI_INT, dest, 0, MPI_COMM_WORLD,&request);
            }
            break;
        }
        MPI_Irecv(&e1, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recv_request);
        MPI_Test(&recv_request, &recv_flag, MPI_STATUS_IGNORE);
        if(recv_flag){
            
            break;
        }
        e1 += size-1;
    }

    MPI_Request recv_request1;
    int d1 = d + rank;
    
    int recv_flag1 = 0;
    while (1) {
        if ((d1 * e1) % fi == 1)
        {
            for (int dest = 0; dest < size; dest++) 
            {
                if(dest == rank)
                continue;
                MPI_Request request;
                MPI_Isend(&d1, 1, MPI_INT, dest, 1, MPI_COMM_WORLD,&request);
            }
            break;

        }
        MPI_Irecv(&d1, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recv_request1);
        MPI_Test(&recv_request1, &recv_flag1, MPI_STATUS_IGNORE);
        if(recv_flag1){
            break;
        }
        d1 += size-1;
    }
    
    if(rank == 0){
        private_key = d1;
        public_key = e1;
    }
    //MPI_Barrier(MPI_COMM_WORLD);
    
}

// Function to encrypt the given number
long long int encrypt(double message) {
    int e = public_key;
    long long int encrypted_text = 1;
    while (e--) {
        encrypted_text *= message;
        encrypted_text %= n;
    }
    return encrypted_text;
}

// Function to decrypt the given number
long long int decrypt(int encrypted_text) {
    int d = private_key;
    long long int decrypted = 1;
    while (d--) {
        decrypted *= encrypted_text;
        decrypted %= n;
    }
    return decrypted;
}

// Function to encode the message
int* encoder(const char* message, int* size) {
    *size = strlen(message);
    int* form = (int*)malloc(*size * sizeof(int));

    for (int i = 0; i < *size; i++) {
        form[i] = encrypt((int)message[i]);
    }

    return form;
}

// Function to decode the message
char* decoder(const int* encoded, int size) {
    char* s = (char*)malloc((size + 1) * sizeof(char));

    for (int i = 0; i < size; i++) {
        s[i] = decrypt(encoded[i]);
    }

    s[size] = '\0';
    return s;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Access the string argument using argv[1]
    const char* message = argv[1];
    
    primefiller();
    setkeys();

    
    // Calling the encoding function
    int size;
    
    if(rank == 0){
    int* coded = encoder(message, &size);
    printf("Initial message:\n%s\n\n", message);
    printf("The encoded message (encrypted by public key):\n");
    for (int i = 0; i < size; i++) {
        printf("%d", coded[i]);
    }

    printf("\n\nThe decoded message (decrypted by private key):\n%s\n", decoder(coded, size));
    }
    // Free allocated memory
    //free(coded);
    MPI_Finalize();
    return 0;
}
