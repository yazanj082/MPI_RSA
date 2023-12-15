#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Function prototypes
void primefiller();
int pickrandomprime();
void setkeys();
long long int encrypt(double message);
long long int decrypt(int encrypted_text);
long long int* encoder(const char* message, int* size);
char* decoder(const long long int* encoded, int size);

// Global variables
int prime[250]; // Using an array instead of set
long long  public_key;
long long private_key;
long long n;

// Function to fill the primes array using the Sieve of Eratosthenes
void primefiller() {
    memset(prime, 1, sizeof(prime));
    prime[0] = prime[1] = 0;
    for (int i = 2; i < 250; i++) {
        if (prime[i]) {
            for (int j = i * 2; j < 250; j += i) {
                prime[j] = 0;
            }
        }
    }
}
int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

// Function to pick a random prime number and remove it from the array
int pickrandomprime() {
    int k = rand() % 250;
    while (!prime[k]) {
        k = rand() % 250;
    }
    prime[k] = 0;
    return k;
}

// Function to set public and private keys
void setkeys(long long prime1,long long prime2) {
    n = prime1 * prime2;
    long long fi = (prime1 - 1) * (prime2 - 1);
    int e = 2;
    while (1) {
        if (gcd(e, fi) == 1)
            break;
        e++;
    }
    public_key = e;
    long long d = 2;
    while (1) {
        if ((d * e) % fi == 1)
            break;
        d++;
    }
    private_key = d;
    
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
long long int* encoder(const char* message, int* size) {
    *size = strlen(message);
    long long int* form = (long long int*)malloc(*size * sizeof(long long int));

    for (int i = 0; i < *size; i++) {
        form[i] = encrypt((long long int)message[i]);
    }

    return form;
}

// Function to decode the message
char* decoder(const long long int* encoded, int size) {
    char* s = (char*)malloc((size + 1) * sizeof(char));

    for (int i = 0; i < size; i++) {
        s[i] = decrypt(encoded[i]);
    }

    s[size] = '\0';
    return s;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <string_argument>\n", argv[0]);
        return 1;
    }

    // Access the string argument using argv[1]
    const char* message = argv[1];
    setkeys(4001,4003);
    printf("the public key (%lld)\nthe private key (%lld)\n, the n is : (%lld)\n",public_key,private_key,n);
    // Calling the encoding function
    int size;
    long long int* coded = encoder(message, &size);

    printf("Initial message:\n%s\n\n", message);
    printf("The encoded message (encrypted by public key):\n");
    for (int i = 0; i < size; i++) {
        printf("%lld", coded[i]);
    }

    printf("\n\nThe decoded message (decrypted by private key):\n%s\n", decoder(coded, size));

    // Free allocated memory
    free(coded);

    return 0;
}
