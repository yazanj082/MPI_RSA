#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <setjmp.h>


// Global variables
long long  public_key;
long long  private_key;
long long n;
int total_threads;
int arraySize;
pthread_mutex_t lock;
long long public_key_candidate = 0;
long long private_key_candidate = 0;

int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

typedef struct {
    long long prime1;
    long long prime2;
    int thread_id;
    int total_threads;
} KeyThreadArgs;

void* SetPublicKeyThread(void* args) {
    KeyThreadArgs* data = (KeyThreadArgs*)args;
    
    long long fi = (data->prime1 - 1) * (data->prime2 - 1);

    int e = 2;  // Starting value for e
    int e1 = e + data->thread_id;  // Unique starting point for each thread
    int count = 0;

    while (1) {
        if (gcd(e1, fi) == 1) {
            pthread_mutex_lock(&lock);
            if (public_key_candidate == 0 || e1 < public_key_candidate) {
                public_key_candidate = e1;
            }
            pthread_mutex_unlock(&lock);
            break;
        }
        e1 += data->total_threads;
        count += 1;
        if (count % 50 == 0) {
            pthread_mutex_lock(&lock);
            if (public_key_candidate > 0) {
                pthread_mutex_unlock(&lock);
                break;
            }
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}
void* SetPrivateKeyThread(void* args) {
    KeyThreadArgs* data = (KeyThreadArgs*)args;
    long long fi = (data->prime1 - 1) * (data->prime2 - 1);

    int d = 2;  // Starting value for d
    int d1 = d + data->thread_id;  // Unique starting point for each thread
    int count = 0;

    while (1) {
        if ((d1 * public_key) % fi == 1) {
            pthread_mutex_lock(&lock);
            if (private_key_candidate == 0 || d1 < private_key_candidate) {
                private_key_candidate = d1;
            }
            pthread_mutex_unlock(&lock);
            break;
        }
        d1 += data->total_threads;
        count += 1;
        if (count % 100 == 0) {
            pthread_mutex_lock(&lock);
            if (private_key_candidate > 0) {
                pthread_mutex_unlock(&lock);
                break;
            }
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}
typedef struct {
    double message;
    long long int* partial_results;
    int start;
    int end;
    long long int n; // Modulus n needs to be shared among threads
} EncryptThreadArgs;
void* encrypt_portion(void* args) {
    EncryptThreadArgs* data = (EncryptThreadArgs*)args;
    long long int encrypted_text = 1;

    for (int i = data->start; i < data->end; i++) {
        encrypted_text *= data->message;
        encrypted_text %= data->n;
    }

    data->partial_results[data->start] = encrypted_text; // Store result in the first position of each segment
    return NULL;
}
long long int encrypt(double message) {
    long long int* partial_results = (long long int*)malloc(total_threads * sizeof(long long int));
    pthread_t threads[total_threads];
    EncryptThreadArgs args[total_threads];

    int section = public_key / total_threads;
    if (total_threads > public_key) section = 1;

    for (int i = 0; i < total_threads; ++i) {
        args[i].message = message;
        args[i].partial_results = partial_results;
        args[i].start = i * section;
        args[i].end = args[i].start + section;
        args[i].n = n;

        if (i == total_threads - 1) {
            args[i].end += public_key % total_threads;
        }

        pthread_create(&threads[i], NULL, encrypt_portion, (void*)&args[i]);
    }

    // Wait for threads to complete and combine results
    long long int result = 1;
    for (int i = 0; i < total_threads; ++i) {
        pthread_join(threads[i], NULL);
        result *= partial_results[args[i].start]; // Combine results
        result %= n;
    }

    free(partial_results);
    return result;
}
typedef struct {
    int encrypted_text;
    long long int* partial_results;
    int start;
    int end;
    long long int n; // Modulus n is shared among threads
} DecryptThreadArgs;
void* decrypt_portion(void* args) {
    DecryptThreadArgs* data = (DecryptThreadArgs*)args;
    long long int decrypted = 1;

    for (int i = data->start; i < data->end; i++) {
        decrypted *= data->encrypted_text;
        decrypted %= data->n;
    }

    data->partial_results[data->start] = decrypted; // Store result in the first position of each segment
    return NULL;
}
long long int decrypt(int encrypted_text) {
    long long int* partial_results = (long long int*)malloc(total_threads * sizeof(long long int));
    pthread_t threads[total_threads];
    DecryptThreadArgs args[total_threads];

    int section = private_key / total_threads;
    if (total_threads > private_key) section = 1;

    for (int i = 0; i < total_threads; ++i) {
        args[i].encrypted_text = encrypted_text;
        args[i].partial_results = partial_results;
        args[i].start = i * section;
        args[i].end = args[i].start + section;
        args[i].n = n;

        if (i == total_threads - 1) {
            args[i].end += private_key % total_threads;
        }

        pthread_create(&threads[i], NULL, decrypt_portion, (void*)&args[i]);
    }

    // Wait for threads to complete and combine results
    long long int result = 1;
    for (int i = 0; i < total_threads; ++i) {
        pthread_join(threads[i], NULL);
        result *= partial_results[args[i].start]; // Combine results
        result %= n;
    }

    free(partial_results);
    return result;
}
// Function to encode the message
long long int* encoder(const char* message, int* size_arr) {
    *size_arr = strlen(message);
    long long int* form = (long long int*)malloc(*size_arr * sizeof(long long int));

    for (int i = 0; i < *size_arr; i++) {
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
void SetKeys(long long prime1,long long prime2){
    n = prime1 *prime2;
    // Thread creation for SetPublicKeyThread
    pthread_t threadsPublic[total_threads];
    KeyThreadArgs argsPublic[total_threads];
    for (int i = 0; i < total_threads; ++i) {
        argsPublic[i].prime1 = prime1;
        argsPublic[i].prime2 = prime2;
        argsPublic[i].thread_id = i;
        argsPublic[i].total_threads = total_threads;
        pthread_create(&threadsPublic[i], NULL, SetPublicKeyThread, (void*)&argsPublic[i]);
    }

    // Wait for threads to complete
    for (int i = 0; i < total_threads; ++i) {
        pthread_join(threadsPublic[i], NULL);
    }

    // Now, Public_key_candidate holds the smallest suitable d1 value
    public_key = public_key_candidate;


        // Thread creation for SetPrivateKeyThread
    pthread_t threads[total_threads];
    KeyThreadArgs args[total_threads];
    for (int i = 0; i < total_threads; ++i) {
        args[i].prime1 = prime1;
        args[i].prime2 = prime2;
        args[i].thread_id = i;
        args[i].total_threads = total_threads;
        pthread_create(&threads[i], NULL, SetPrivateKeyThread, (void*)&args[i]);
    }

    // Wait for threads to complete
    for (int i = 0; i < total_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Now, private_key_candidate holds the smallest suitable d1 value
    private_key = private_key_candidate;



}
int main(int argc, char* argv[]) {
    // Initialize mutex
    pthread_mutex_init(&lock, NULL);

    // Assuming prime1, prime2, and public_key are given
    long long prime1 = 4001;
    long long prime2 = 4003;
    total_threads = 1; // Example thread count
    
    const char* message = "hgfhfghdddfhfghdddddddddddddhfhfghdddddddddddddhfhfghdddddddddddddhddddddddddhgd";

    SetKeys(4001
    ,4003);
    int size;
    long long int* coded = encoder(message, &size);
    char* decoded = decoder(coded, size);
    
    printf("Initial message:\n%s\n\n", message);
    printf("The encoded message (encrypted by public key):\n");
    for (int i = 0; i < size; i++) {
        printf("%lld", coded[i]);
    }

    printf("\n\nThe decoded message (decrypted by private key):\n%s\n",decoded );
    
    // Free allocated memory
    free(coded);
    free(decoded);
    // Cleanup
    pthread_mutex_destroy(&lock);

    return 0;
}

